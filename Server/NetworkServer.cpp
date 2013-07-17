/* Author: Maximilian Fricke */

#include "NetworkServer.h"

NetworkServer::NetworkServer()
    : _maxId(0),
      _sendTimer(0) {
    _socket.Open(NET_PORT);
}

NetworkServer::~NetworkServer() {
    _socket.Close();
}
