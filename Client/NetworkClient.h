#ifndef NETWORK_CLIENT_H_
#define NETWORK_CLIENT_H_

#include "../VirtualConnection.h"
#include "../Network.h"
#include "../defines.h"

#include <math.h>

// Timeouts and Intervals in s
#define CON_TIMEOUT  10.0f
#define SND_INTERVAL 0.1f

class NetworkClient {
private:    
    Connection _connection;
    Socket _socket;
    Address _serverAddress;

    float _sendTimer;
    float _disconnectTimer;

    bool _isConnected;

public:
    NetworkClient();
    ~NetworkClient();
    bool IsConnected() const { return _isConnected; }
    void ConnectTo(Address address);

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
        ** Receive packet
        **   TODO: empty queue
        */

        Address sender;
        if (_isConnected) {
            _disconnectTimer += dt;
            if (_disconnectTimer > CON_TIMEOUT) {
                _isConnected = false;
                state.disconnect();
            }

            while ((rcvPacketSize = _socket.Receive(sender, rcvPacket, State::MAX_DESERIALIZE_SIZE + NET_HEADER_SIZE)) > 0) {
                // Client is connected
                _disconnectTimer = 0;

                // Extract the body
                if (!(_connection.ExtractBody(rcvPacket, rcvPacketSize, rcvState, rcvStateSize))) {
    #ifdef DEBUG
                    std::cout << "Error extracting body from received packet." << std::endl;
    #endif
                }

                // Deserialize it
                state.deserialize(rcvState, rcvStateSize);
            }
        }

        /*
        ** Send packet
        */

        if ((_sendTimer > SND_INTERVAL) && (_isConnected)) {
            _sendTimer = fmod(_sendTimer, SND_INTERVAL);

            // Serialize
            state.serialize(sndState, sndStateSize);

            // Add the header
            if (_connection.BuildHeader(sndState, sndStateSize, sndPacket, sndPacketSize)) {
                // Send the packet
                _socket.Send(_serverAddress, sndPacket, sndPacketSize);
            }
        }
    }
};

#endif
