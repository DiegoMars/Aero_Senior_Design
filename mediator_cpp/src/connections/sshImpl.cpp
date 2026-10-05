#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Prevents windows.h from loading old winsock.h
#endif

#include "ssh.h"
// #include <iostream>
// #include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <format>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <libssh2.h>

void sleep(int time) {
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

// ### Public ###
sshConnection::sshConnection(sshConnectionInfo connectionInfo){
    this->connectionInfo = connectionInfo;
}

sshConnection::~sshConnection() {
    stop();
}

void sshConnection::start(){
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return; // Need some way to deal with errors from threads
        // return 1;
    }
    printf("WSAStartup succeeded!\n");

    // resolve "host" into an actual IP address
    struct addrinfo hints{};
    struct addrinfo *result;
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    iResult = getaddrinfo(connectionInfo.hostName.c_str(), "22", &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d (%s)\n", iResult, gai_strerrorA(iResult));
        return;
        // return 1;
    }

    // From private field
    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == INVALID_SOCKET) {
        printWinsockError("socket");
        return;
        // return 1;
    }
    printf("Sock created!\n");

    iResult = connect(sock, result->ai_addr, result->ai_addrlen);
    if (iResult != 0) {
        printWinsockError("connect");
        return;
        // return 1;
    }

    // This could be a little finicky, so give time between code runs
    libssh2_init(0);
    session = libssh2_session_init(); // From the private field
    if (!session) {
        printf("libssh2_session_init failed\n");
        return;
        // return 1;
    }
    printf("Session Started!\n");

    int rc = libssh2_session_handshake(session, sock);
    if (rc != 0) {
        printf("handshake failed: %d\n", rc);
        return;
        // return 1;
    }
    printf("Session handshake succeeded!\n");

    // Use the sshConnectionInfo struct here
    rc = libssh2_userauth_password(session, connectionInfo.user.c_str(), connectionInfo.pass.c_str());
    if (rc != 0) {
        char* errmsg;
        int errlen;
        libssh2_session_last_error(session, &errmsg, &errlen, 0);
        printf("auth failed: %s\n", errmsg);
        return;
        // return 1;
    }

    channel = libssh2_channel_open_session(session); // From the private field
    if (!channel) {
        printf("channel_open_session failed\n");
        return;
        // return 1;
    }
    printf("Channel Opened!\n");

    // Pseudo channel for persistance
    rc = libssh2_channel_request_pty(channel, "xterm");
    if (rc != 0) {
        printf("request_pty failed: %d\n", rc);
        return;
        // return 1;
    }

    rc = libssh2_channel_shell(channel);
    if (rc != 0) {
        printf("channel_shell failed: %d\n", rc);
        return;
        // return 1;
    }
    running.store(true);
    libssh2_session_set_blocking(session, 0);
    printf("Interactive shell started!\n");

    // From the private reader
    reader = std::jthread(&sshConnection::readerFunc, this);

    // while (running.load()) {
    //     sendCommand("ls");
    //     sleep(20);
    //     sendCommand("exit");
    //     break;
    // }
    while (running.load()) {
        sleep(20);
    }
}

bool sshConnection::sendCommand(const std::string& command) {
    if (!isConnected()) {
        printf("Send command: not connected\n");
        return false;
    }
    std::string toSend = std::format("{}\n", command);
    size_t totalWritten = 0;
    ssize_t n;

    while (totalWritten < toSend.size()) {
        {
            // Lock and attempt to write to channel
            std::lock_guard<std::mutex> lock(sshMutex);
            n = libssh2_channel_write(channel,
                                      toSend.c_str() + totalWritten,
                                      toSend.size() - totalWritten
                                      );
        }
        if (n == LIBSSH2_ERROR_EAGAIN) {
            sleep(20);
            continue; // Retry
        }
        if (n < 0) {
            return false;
        }
        totalWritten += static_cast<size_t>(n);
    }
    return true;

}

void sshConnection::stop() {
    auto t0 = std::chrono::steady_clock::now();
    auto mark = [&](const char* label) {
        auto now = std::chrono::steady_clock::now();
        printf("[stop] %s at +%lldms\n", label,
               (long long)std::chrono::duration_cast<std::chrono::milliseconds>(now - t0).count());
    };

    if (isConnected()) {
        sendCommand("exit");
        mark("sent exit");
        int timeout = 2000, waited = 0;
        while (running.load() && timeout > waited) { sleep(20); waited += 20; }
        mark("wait loop done");
        running.store(false);
    }

    if (reader.joinable()) reader.join();
    mark("reader joined");

    if (channel) { libssh2_channel_close(channel); libssh2_channel_free(channel); channel = nullptr; }
    mark("channel closed/freed");

    if (session) { libssh2_session_disconnect(session, "done"); libssh2_session_free(session); session = nullptr; }
    mark("session disconnected/freed");

    if (sock != INVALID_SOCKET) { closesocket(sock); sock = INVALID_SOCKET; }
    WSACleanup();
    mark("socket/winsock cleaned");
}

// ### Private ###
void sshConnection::printWinsockError(const char* func) {
    int err = WSAGetLastError();
    char* msgBuf = nullptr;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&msgBuf, 0, nullptr);
    printf("%s failed: %d (%s)\n", func, err, msgBuf ? msgBuf : "unknown error");
    LocalFree(msgBuf);
}

void sshConnection::readerFunc(){
    // channel here is from the private field
    char buf[4096];
    while (running.load()){
        ssize_t n;
        {
            std::lock_guard<std::mutex> lock(sshMutex);
            n = libssh2_channel_read(channel, buf, sizeof(buf));
        } // releases lock after this

        if (n > 0) {
            fwrite(buf, 1, n, stdout);
            fflush(stdout);
            continue;
        }

        if (n == LIBSSH2_ERROR_EAGAIN) {
            sleep(20);
            continue;
        }

        std::lock_guard<std::mutex> lock(sshMutex);
        if (libssh2_channel_eof(channel)) {
            printf("[remote shell closed]\n");
            running.store(false);
            break;
        }
    }
}
