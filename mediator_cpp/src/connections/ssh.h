#ifndef H_SSH
#define H_SSH

#include <string>
#include <mutex>
#include <atomic>
#include <libssh2.h>

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
    std::atomic<bool> running{true}; // for the channel
    std::atomic<bool> connected{false}; // for the shell
    sshConnectionInfo connectionInfo;
    LIBSSH2_SESSION* session = nullptr;
    LIBSSH2_CHANNEL* channel = nullptr;

    void printWinsockError(const char* func);
    void readerThreadFunc();

public:
    sshConnection(sshConnectionInfo connectionInfo);
    ~sshConnection();
    void threadFunction();  // Will start a socket, connect, starts the shell and reader function,
                            // then awats commands
                            // Will eventually be renamed to "start()"
    bool sendCommand(const std::string& command);
    bool isConnected() const { return connected.load(); }
    void stop();
};

#endif
