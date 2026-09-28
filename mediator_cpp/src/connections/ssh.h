#ifndef H_SSH
#define H_SSH

#include <string>
#include <mutex>
#include <atomic>
#include <libssh2.h>
using namespace std;

// Structures
struct sshConnectionInfo {
    string hostName;
    string user;
    string pass;
};

// class for ssh connection goes here
// Need to look at how to return a pipe of the terminal output
class sshConnection {
private:
    mutex sshMutex;
    atomic<bool> running{true};
    sshConnectionInfo connectionInfo;
    void printWinsockError(const char* func);
    void readerThreadFunc(LIBSSH2_CHANNEL* channel);
public:
    sshConnection(sshConnectionInfo connectionInfo);
    void threadFunction();
};

#endif
