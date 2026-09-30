// How header files will be installed
// #include "connections/MAVLink.h"
#include "connections/ssh.h"
// #include "drivers/Payload.h"
// #include "drivers/Plane.h"

#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <libssh2.h>
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
    sshConnection sshThing(connectionInfo);
    jthread runningConnection(&sshConnection::start, &sshThing);
    runningConnection.join();
}
