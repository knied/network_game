#ifndef VIRTUALCONNECTION_H
#define VIRTUALCONNECTION_H

#include <vector>

#ifdef DEBUG
    #include <iostream>
    #include <iomanip>
#endif

#include "Network.h"
#include "defines.h"

class Connection
{
private:
    // Header stuff
    unsigned short _seqNo;
    unsigned short _ackNo;
    unsigned int _prevAcks;

    bool IsRecent(unsigned int newSeqNo);

public:
    Connection();
    bool BuildHeader(const char* body, const unsigned int bodySize, char* packet, int& packetSize);
    bool ExtractBody(const char* packet, const unsigned int packetSize, char* body, int& bodySize);
};

#endif // VIRTUALCONNECTION_H
