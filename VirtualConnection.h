/* Author: Maximilian Fricke */

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
    bool BuildHeader(const unsigned char* body, const unsigned int bodySize, unsigned char* packet, unsigned int& packetSize);
    bool ExtractBody(const unsigned char* packet, const unsigned int packetSize, unsigned char* body, unsigned int& bodySize);
};

#endif // VIRTUALCONNECTION_H
