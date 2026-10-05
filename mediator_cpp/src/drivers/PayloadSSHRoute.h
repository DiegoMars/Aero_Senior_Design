#ifndef H_PAYLOAD_SSH_ROUTE
#define H_PAYLOAD_SSH_ROUTE

#include "Payload.h"
#include "../connections/ssh.h"
#include <thread>

class PayloadSshRoute : public PayloadDriver {
private:
    sshConnection connection;
    std::jthread connectionThread;

public:
    explicit PayloadSshRoute(sshConnectionInfo connectionInfo,
                             TerminalOutput* output);
    ~PayloadSshRoute() override;

    void connect() override;
    void disconnect() override;
    bool isConnected() const override;

    void testing() override;
    void startAligning() override;
    void stopAligning() override;
};

// ### Usage Example ####
// #include "connections/ssh.h"
// #include "drivers/PayloadSSHRoute.h"
//
// #include <thread>
// using namespace std;
//
// static const char *raspHostName = "example";
// static const char *username = "example";
// static const char *pass = "example";
//
// int main() {
//     sshConnectionInfo connectionInfo = {
//         raspHostName,
//         username,
//         pass
//     };
//     PayloadSshRoute route(connectionInfo);
//     route.connect();
//
//     this_thread::sleep_for(chrono::seconds(3));
//     if (route.isConnected()) {
//         route.testing();
//     }
// }

#endif
