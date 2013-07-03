#ifndef NETWORK_SERVER_H_
#define NETWORK_SERVER_H_

#include <vector>

#include "../Network.h"
#include "../VirtualConnection.h"
#include "../defines.h"

#ifdef DEBUG
    #include "iostream"
#endif

class NetworkServer {
private:
    struct _Client{
        unsigned int id;
        Address address;
        Connection connection;
    };

    std::vector<_Client> _clients;
    unsigned int _maxId;

    Socket _socket;

    char _packetBuffer[NET_MAX_PACKET_SIZE];
    unsigned int _packetSize;
    char _bodyBuffer[NET_MAX_BODY_SIZE];
    unsigned int _bodySize;

public:
    NetworkServer();
    ~NetworkServer();

    template<class State>
    void update(float dt, State& state) {
        /*
        ** Send a packet to all clients
        */
        for (std::vector<_Client>::iterator itClient = _clients.begin(); itClient != _clients.end(); itClient++) {
            // Serialize
            state.serialize(_bodyBuffer, _bodySize, itClient->id);

            // Build header
            itClient->connection.BuildHeader(_bodyBuffer, _bodySize, _packetBuffer, _packetSize);

            // Send the packet
            _socket.Send(itClient->address, _packetBuffer, _packetSize);
        }

        /*
        ** Receive a packet from any client
        */
        _Client senderClient;
        _socket.Receive(senderClient.address, _packetBuffer, NET_MAX_PACKET_SIZE);

        // Check if client is already connected
        bool clientFound = false;
        for (std::vector<_Client>::iterator itClient = _clients.begin(); itClient != _clients.end(); itClient++) {
            if (itClient->address == senderClient.address) {
                // Client found
                clientFound = true;

                // Extract body
                itClient->connection.ExtractBody(_packetBuffer, _packetSize, _bodyBuffer, _bodySize);

                // Deserialize
                state.deserialize(_bodyBuffer, _bodySize, itClient->id);
            }
        }

        // New client?
        if (!clientFound) {
            senderClient.id = ++_maxId;

            // Extract body
            senderClient.connection.ExtractBody(_packetBuffer, _packetSize, _bodyBuffer, _bodySize);

            // Deserialize
            state.deserialize(_bodyBuffer, _bodySize, senderClient.id);
        }
    }
};

#endif
