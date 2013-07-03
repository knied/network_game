#ifndef NETWORK_CLIENT_H_
#define NETWORK_CLIENT_H_

#include "../VirtualConnection.h"
#include "../Network.h"
#include "../defines.h"

class NetworkClient {
private:
    Connection _connection;
    Socket _socket;
    Address _serverAddress;

public:
    NetworkClient();
    ~NetworkClient();
    bool IsConnected() { /*TODO*/ return true; }

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

        /*
        ** Receive packet
        */
        Address sender;
        rcvPacketSize = _socket.Receive(sender, rcvPacket, State::MAX_SERIALIZE_SIZE + NET_HEADER_SIZE);

        // Packet received?
        if (rcvPacketSize > 0) {
            // Extract the body
            if (!(_connection.ExtractBody(rcvPacket, rcvPacketSize, rcvState, rcvStateSize))) {
#ifdef DEBUG
                std::cout << "Error extracting body from received packet." << std::endl;
#endif
            }

            // Deserialize it
            state.deserialize(rcvState, rcvStateSize);
        }

        /*
        ** Send packet
        */

        // Serialize
        state.serialize(sndState, sndStateSize);

        // Add the header
        if (_connection.BuildHeader(sndState, sndStateSize, sndPacket, sndPacketSize)) {
            // Send the packet
            _socket.Send(_serverAddress, sndPacket, sndPacketSize);
        }
    }
};

#endif
