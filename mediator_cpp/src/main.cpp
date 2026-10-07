// How header files will be installed
// #include "connections/MAVLink.h"
#include "connections/ssh.h"
#include "connections/TerminalOutput.h"
// #include "drivers/Payload.h"
#include "drivers/PayloadSSHRoute.h"
// #include "drivers/Plane.h"

#include <thread>
using namespace std;

// Change this to whatever you are using, will eventually be able to change through the GUI
// static const char *raspIP = "10.109.70.209";
static const char *raspHostName = "ieeeRasp";
static const char *username = "user_ieee";
static const char *pass = "pass_ieee8333";

int main() {
    sshConnectionInfo connectionInfo = {
        raspHostName,
        username,
        pass
    };
    TerminalOutput output;
    PayloadSshRoute route(connectionInfo, &output);
    route.connect();

    this_thread::sleep_for(chrono::seconds(3));
    if (route.isConnected()) {
        route.testing();
    }

    for (int i = 0; i < 10; ++i) {
        this_thread::sleep_for(chrono::milliseconds(500));
        for (auto& line : output.drain()) {
            printf("[%s] %s\n", line.source.c_str(), line.text.c_str());
        }
    }
}
