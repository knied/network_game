#ifndef NETWORK_SERVER_H_
#define NETWORK_SERVER_H_

#include <vector>

#include "../Network.h"
#include "../VirtualConnection.h"
#include "../defines.h"

#include <math.h>

//#ifdef DEBUG
    #include <iostream>
//#endif

// Timeout and send interval in seconds
#define CON_TIMEOUT  10.0f
#define SND_INTERVAL 0.1f

class NetworkServer {
private:
    struct _Client{
        unsigned int id;
        Address address;
        Connection connection;
        float disconnectTimer;
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

        // Update timers
        _sendTimer += dt;

        /*
        ** Send packets to clients
        */

        for (std::vector<_Client>::iterator itClient = _clients.begin(); itClient != _clients.end(); itClient++) {
            // Update disconnection timer
            if ((itClient->disconnectTimer += dt) > CON_TIMEOUT) {
                // Client disconnected!
                state.disconnect(itClient->id);

                // Remove it from the list
                _clients.erase(itClient);

                continue;
            }

            // Send
            if (_sendTimer > SND_INTERVAL) {
                // Serialize
                state.serialize(sndState, sndStateSize, itClient->id);

                // Build header
                itClient->connection.BuildHeader(sndState, sndStateSize, sndPacket, sndPacketSize);

                // Send the packet
                _socket.Send(itClient->address, sndPacket, sndPacketSize);
            }
        }
        // Update send timer
        _sendTimer = fmod(_sendTimer, SND_INTERVAL);

        /*
        ** Receive all queued packets
        */

        Address sender;
        while ((rcvPacketSize = (_socket.Receive(sender, rcvPacket, State::MAX_SERIALIZE_SIZE + NET_HEADER_SIZE))) > 0) {
            // Check if client is already connected
            bool clientFound = false;
            for (std::vector<_Client>::iterator itClient = _clients.begin(); itClient != _clients.end(); itClient++) {
                if (itClient->address == sender) {
                    // Client found
                    clientFound = true;

                    // Reset disconnection timer
                    itClient->disconnectTimer = 0;

                    // Extract body
                    itClient->connection.ExtractBody(rcvPacket, rcvPacketSize, rcvState, rcvStateSize);

                    // Deserialize
                    state.deserialize(rcvState, rcvStateSize, itClient->id);

                    break;
                }
            }

            // New client?
            if (!clientFound) {
//#ifdef DEBUG
                std::cout << "New Client!" << std::endl;
//#endif
                _Client newClient;
                newClient.address = sender;
                newClient.id = ++_maxId;
                newClient.disconnectTimer = 0;
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
