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
#include <list>
#include <iostream> // DELETE
#include "defines.h" // DELETE

/*
** Addresses
*/

class Address {
private:
    sockaddr _address;
    const unsigned short _port;

public:
    Address();
    Address(std::string address, unsigned short port);
    Address(sockaddr &address, unsigned short port);
    sa_family_t Family() const { return _address.sa_family; }
    const sockaddr* Sockaddr() const { return &_address; }
    socklen_t Size() const {
        if (_address.sa_family == AF_INET ) return sizeof(sockaddr_in);
        else return sizeof(sockaddr_in6);
    }
    void setSockaddr(sockaddr address) { _address = address; }
};

/*
** Non-blocking UDP sockets for both IPv4/6
*/

class Socket {
private:
    struct _Sock {
        int descriptor;
        sockaddr address;
    };

    std::list<_Sock> _socketDescs;

public:
    Socket();
    ~Socket();
    bool Open(const unsigned short port);
    void Close();
    bool Send(const Address& destination, const void* data, int size);
    int  Receive(Address& sender, void* data, int size);
};

/* DELETE */
void testNetwork();

#endif // NETWORK_H
