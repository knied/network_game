#ifndef NETWORK_SERVER_H_
#define NETWORK_SERVER_H_

#include <vector>

#include "../Network.h"
#include "../VirtualConnection.h"
#include "../defines.h"

//#ifdef DEBUG //TODO: Uncomment
    #include "iostream"
//#endif

class NetworkServer {
private:
    struct _Client{
        unsigned int id;
        Address address;
        Connection connection;
    };

    Socket _socket;
    std::vector<_Client> _clients;
    unsigned int _maxId;
    float _sendTimer;

public:
    NetworkServer();
    ~NetworkServer();

    template<class State>
    void update(float dt, State& state) {
        unsigned char rcvPacket[State::MAX_DESERIALIZE_SIZE + NET_HEADER_SIZE];
        unsigned char rcvState[State::MAX_DESERIALIZE_SIZE];
        unsigned char sndPacket[State::MAX_SERIALIZE_SIZE + NET_HEADER_SIZE];
        unsigned char sndState[State::MAX_SERIALIZE_SIZE];

        unsigned int rcvPacketSize;
        unsigned int rcvStateSize;
        unsigned int sndPacketSize;
        unsigned int sndStateSize;

        // Update timer
        _sendTimer += dt;

        /*
        ** Send a packet to all clients
        */

        if (_sendTimer > 0.1f) {
            _sendTimer = 0;

            for (std::vector<_Client>::iterator itClient = _clients.begin(); itClient != _clients.end(); itClient++) {
                // Serialize
                state.serialize(sndState, sndStateSize, itClient->id);

                // Build header
                itClient->connection.BuildHeader(sndState, sndStateSize, sndPacket, sndPacketSize);

                std::cout << "sndPacketSize = " << sndPacketSize << std::endl;

                // Send the packet
                _socket.Send(itClient->address, sndPacket, sndPacketSize);
            }
        }

        /*
        ** Receive a packet from any client
        */

        Address sender;
        rcvPacketSize = _socket.Receive(sender, rcvPacket, State::MAX_SERIALIZE_SIZE + NET_HEADER_SIZE);

        if (rcvPacketSize > 0) {
            // Check if client is already connected
            bool clientFound = false;
            for (std::vector<_Client>::iterator itClient = _clients.begin(); itClient != _clients.end(); itClient++) {
                if (itClient->address == sender) {
                    // Client found
                    clientFound = true;

                    // Extract body
                    itClient->connection.ExtractBody(rcvPacket, rcvPacketSize, rcvState, rcvStateSize);

                    // Deserialize
                    state.deserialize(rcvState, rcvStateSize, itClient->id);
                }
            }

            // New client?
            if (!clientFound) {
#ifdef DEBUG
                std::cout << "New Client!" << std::endl;
#endif
                _Client newClient;
                newClient.address = sender;
                newClient.id = ++_maxId;
                _clients.push_back(newClient);

                // Extract body
                newClient.connection.ExtractBody(rcvPacket, rcvPacketSize, rcvState, rcvStateSize);

                // Deserialize
                state.deserialize(rcvState, rcvStateSize, newClient.id);
            }
        }
    }
};

#endif
