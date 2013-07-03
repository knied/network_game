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

    char _packetBuffer[NET_MAX_PACKET_SIZE];
    unsigned int _packetSize;
    char _bodyBuffer[NET_MAX_BODY_SIZE];
    unsigned int _bodySize;

public:
    NetworkClient();
    ~NetworkClient();
    bool IsConnected() { /*TODO*/ return true; }

    template<class State>
    void update(float dt, State& state) {
        /*
        ** Receive packet
        */

        Address sender;
        int numRecvdBytes = _socket.Receive(sender, _packetBuffer, NET_MAX_PACKET_SIZE);

        // Packet received?
        if (numRecvdBytes > 0) {
            // Extract the body
            if (!(_connection.ExtractBody(_packetBuffer, numRecvdBytes, _bodyBuffer, _bodySize))) {
#ifdef DEBUG
                std::cout << "Error extracting body from received packet." << std::endl;
#endif
            }

            // Deserialize it
            state.deserialize(_bodyBuffer, _bodySize);
        }

        /*
        ** Send packet
        */

        // Serialize
        state.serialize(_bodyBuffer, _bodySize);

        // Add the header
        if (!(_connection.BuildHeader(_bodyBuffer, _bodySize, _packetBuffer, _packetSize))) {
            // Send the packet
            _socket.Send(_serverAddress, _packetBuffer, _packetSize);
        }
    }
};

#endif
