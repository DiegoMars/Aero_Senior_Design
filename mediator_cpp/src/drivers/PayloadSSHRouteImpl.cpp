#include "PayloadSshRoute.h"
#include "../connections/ssh.h"
#include <thread>

PayloadSshRoute::PayloadSshRoute(sshConnectionInfo connectionInfo, TerminalOutput* output) : connection(connectionInfo, output) {
    // Nothing here, the header already handles initialization
}

// The destructor of the connection already handles this.
// Not needed but nice to have
PayloadSshRoute::~PayloadSshRoute() { 
    // disconnect();
}

void PayloadSshRoute::connect() {
    connectionThread = std::jthread(&sshConnection::start, &connection);
}

void PayloadSshRoute::disconnect() {
    connection.stop();
    if (connectionThread.joinable()) {
        connectionThread.join();
    }
}

bool PayloadSshRoute::isConnected() const {
    return connection.isConnected();
}

void PayloadSshRoute::testing() {
    connection.sendCommand("ls");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    connection.stop();
}

void PayloadSshRoute::startAligning() {
    // Need to implement
    return;
}

void PayloadSshRoute::stopAligning() {
    // Need to implement
    return;
}
