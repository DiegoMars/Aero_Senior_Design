#ifndef H_SSH
#define H_SSH

#include "TerminalOutput.h"
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <libssh2.h>
#include <winsock2.h>

// Structures
struct sshConnectionInfo {
    std::string hostName;
    std::string user;
    std::string pass;
};

// class for ssh connection goes here
// Need to look at how to return a pipe of the terminal output
class sshConnection {
private:
    std::mutex sshMutex;
    std::atomic<bool> running{false};
    sshConnectionInfo connectionInfo;
    TerminalOutput* output = nullptr;

    SOCKET sock = INVALID_SOCKET;
    LIBSSH2_SESSION* session = nullptr;
    LIBSSH2_CHANNEL* channel = nullptr;
    std::jthread reader;

    void printWinsockError(const char* func);
    void readerFunc();
    void pipePrint(std::string where, std::string value);

public:
    sshConnection(sshConnectionInfo connectionInfo,
                  TerminalOutput* output);
    ~sshConnection();
    void start();  // Will start a socket, connect, starts the shell and reader function,
                            // then awaits commands
    bool sendCommand(const std::string& command);
    bool isConnected() const { return running.load(); }
    void stop();
};

#endif
