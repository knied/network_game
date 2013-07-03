#include "NetworkClient.h"

NetworkClient::NetworkClient()
    : _serverAddress(NET_SERVER_IP, NET_PORT),
      _sendTimer(0) {
    // Open socket on any free port
    _socket.Open(0);
}

NetworkClient::~NetworkClient() {
    _socket.Close();
}
