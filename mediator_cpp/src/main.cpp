// How header files will be installed
// #include "connections/MAVLink.h"
#include "connections/ssh.h"
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
    PayloadSshRoute route(connectionInfo);
    route.connect();

    this_thread::sleep_for(chrono::seconds(3));
    if (route.isConnected()) {
        route.testing();
    }
}
