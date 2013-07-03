//
//  Network.h
//  Platform and IP version agnostic networking
//

#ifndef NETWORK_H
#define NETWORK_H

// Platform detection
#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
    #define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM PLATFORM_MAC
#else
    #define PLATFORM PLATFORM_UNIX
#endif

// Network stuff
#if PLATFORM == PLATFORM_WINDOWS
    #pragma comment(lib, "wsock32.lib")
    #include <winsock2.h>
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    #include <sys/socket.h>
    #include <unistd.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <netdb.h>
#endif

#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>

#include "defines.h"

#ifdef DEBUG
    #include <iostream>
    #include <arpa/inet.h>
#endif

/*
** Addresses
*/

class Address {
private:
    sockaddr_storage _address;
    unsigned short _port;

public:
    Address();
    Address(const std::string address, const unsigned short port);
    Address(const sockaddr_storage& address, const unsigned short port);
    sa_family_t Family() const { return _address.ss_family; }
    unsigned short Port() const { return _port; }
    const sockaddr* Sockaddr() const { return (sockaddr*) &_address; }
    socklen_t Size() const {
        return _address.ss_len;
    }
    void setSockaddr(const sockaddr_storage& address) { _address = address; }

    bool operator ==(const Address& address) const;
};

/*
** Non-blocking UDP sockets for both IPv4/6
*/

class Socket {
private:
    struct _Sock {
        int descriptor;
        sockaddr_storage address;
    };

    std::vector<_Sock> _socketDescs; // TODO replace with vector

public:
    Socket();
    ~Socket();
    bool Open(const unsigned short port);
    void Close();
    bool Send(const Address& destination, const void* data, const int size);
    unsigned int  Receive(Address& sender, void* data, int size);
};

/* DELETE */
// void testNetwork();

#endif // NETWORK_H
