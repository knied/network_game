#include "VirtualConnection.h"

Connection::Connection()
    : _seqNo(0x0),
      _ackNo(0x0),
      _prevAcks(0xFFFFFFFF) {
    std::cout << "Connection constructor called!" << std::endl; // DELETE
}

bool Connection::IsRecent(unsigned int newSeqNo) {
    // No wrap around OR wrap around and out of order
    if (newSeqNo > _ackNo) {
        return ((newSeqNo - _ackNo) < NET_MAX_SEQ_DISTANCE/2);
    // Wrap around OR no wrap around and out of order
    } else {
        return ((_ackNo - newSeqNo) > NET_MAX_SEQ_DISTANCE/2);
    }

    return ((newSeqNo > _ackNo) || ((_ackNo - newSeqNo) > NET_MAX_SEQ_DISTANCE/2));
}

bool Connection::BuildHeader(const unsigned char* body, const unsigned int bodySize, unsigned char* packet, unsigned int& packetSize) {
    unsigned char *tmpPacketPtr = packet;

    // Verfiy the packet size
    packetSize = bodySize + NET_HEADER_SIZE;
    /*
    if ((packetSize = (bodySize + NET_HEADER_SIZE)) > NET_MAX_PACKET_SIZE) {
#ifdef DEBUG
        std::cout << "The packet size is too large (" << packetSize << ") bytes" << std::endl;
#endif
        return false;
    }
    */

    // Insert the protocol ID
    *((unsigned int*) tmpPacketPtr) = NET_PROTOCOL_ID;
    tmpPacketPtr += 4;

    // Pre-increment and insert the SEQ number
    *((unsigned short*) tmpPacketPtr) = ++_seqNo;
    tmpPacketPtr += 2;

    // Insert the ACK number
    *((unsigned short*) tmpPacketPtr) = _ackNo;
    tmpPacketPtr += 2;

    // Insert previous ACKs
    *((unsigned int*) tmpPacketPtr) = _prevAcks;
    tmpPacketPtr += 4;

    // Insert the body
    for (unsigned int it = 0; it < bodySize; it++) {
        tmpPacketPtr[it] = body[it];
    }

    return true;
}

bool Connection::ExtractBody(const unsigned char* packet, const unsigned int packetSize, unsigned char* body, unsigned int& bodySize) {
    /*
    ** Packet size
    */

    // Verify
    /*
    if (packetSize > NET_MAX_PACKET_SIZE) {
#ifdef DEBUG
        std::cout << "The received packet is too large." << std::endl;
#endif
        return false;
    }
    */

    // Size of received body
    bodySize = packetSize - NET_HEADER_SIZE;
#ifdef DEBUG
    std::cout << "Received a packet of " << bodySize << " bytes." << std::endl;
#endif

    /*
    ** Check for protocol ID
    */

    unsigned int receivedProtocolID = *((unsigned int*) packet);
    packet += 4;
    // Does it match?
    if (receivedProtocolID != NET_PROTOCOL_ID) {
#ifdef DEBUG
        std::cout << "The received protocol ID does not match." << std::endl;
#endif
        return false;
    }

    /*
    ** Extract and process SEQ number
    ** TODO: Check for duplicated packets
    */

    unsigned int distance; // SEQ distance
    unsigned short recvdSeqNo = *((unsigned short*) packet);
#ifdef DEBUG
    std::cout << "Received SEQ Number: " << recvdSeqNo;
#endif
    packet += 2;

    // Packet is recent
    if (IsRecent(recvdSeqNo)) {
        // Calculate SEQ distance
        if (recvdSeqNo > _ackNo) {
            distance = recvdSeqNo - _ackNo;
        } else {
            distance = NET_MAX_SEQ_DISTANCE - (_ackNo - recvdSeqNo);
        }
#ifdef DEBUG
        std::cout << " (recent)" << std::endl
                  << "Current ACK Number: " << _ackNo << std::endl
                  << (distance - 1) << " packets were skipped." << std::endl;
#endif
        // Adjust previous Acks
        _prevAcks <<= distance;
        _prevAcks |= (0x1 << (distance - 1));
        // Set ACK to most recent
        _ackNo = recvdSeqNo;

    // Packet is not recent
    } else {
        // Calculate SEQ distance
        if (recvdSeqNo > _ackNo) {
            distance = NET_MAX_SEQ_DISTANCE - (recvdSeqNo - _ackNo);
        } else {
            distance = (_ackNo - recvdSeqNo);
        }
#ifdef DEBUG
        std::cout << " (not recent)" << std::endl;
#endif
        // Received a packet out of order
        _prevAcks |= (0x1 << (distance - 1));
    }

    /*
    ** Extract and process ACK number and previous ACKs bitfield
    */

    unsigned short recvdAckNo = *((unsigned short*) packet);
    packet += 2;
    unsigned int recvdPrevAcks = *((unsigned int*) packet);
    packet += 4;
#ifdef DEBUG
    std::cout << "Received ACK number: " << recvdAckNo << std::endl
              << "Received prev ACKS: " << std::hex << recvdPrevAcks << std::endl;
#endif
    // TODO: flag missing packets to be resend

    /*
    ** Extract the body
    */

    for (unsigned int it = 0; it < bodySize; it++ ) body[it] = packet[it];

    return true;
}
