#include "NetworkClient.h"

NetworkClient::NetworkClient()
    : _serverAddress(NET_SERVER_IP, NET_PORT),
      _sendTimer(0),
      _disconnectTimer(0),
      _isConnected(false) {
    // Open socket on any free port
    _socket.Open(0);
}

NetworkClient::~NetworkClient() {
    _socket.Close();
}

void NetworkClient::ConnectTo(Address server) {
    _serverAddress = server;
    _isConnected = true;
    _disconnectTimer = 0;
}
