#include "Network.h"

Address::Address()
    : _port(0)
{
    memset(&_address, 0, sizeof(_address));
}

Address::Address(const std::string address, const unsigned short port)
    : _port(port)
{
    // Cast port number into C string
    std::stringstream portSStream;
    portSStream << port;
    const char* portStr = portSStream.str().c_str();

    // Get possible sockaddrs
    addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    getaddrinfo(address.c_str(), portStr, &hints, &res);

    // Pick the first valid sockaddr
    memset(&_address, 0, sizeof(_address));
    if (res->ai_addr != 0) {
        _address = *((sockaddr_storage*) res->ai_addr);
    }
}

Address::Address(const sockaddr_storage& address, const unsigned short port)
    : _address(address),
      _port(port) {
}

Socket::Socket() {
    _socketDescs.clear();
}

Socket::~Socket() {
    Close();
}

/*
** Open both an IPv4 and IPv6 non-blocking UDP socket for the given port
*/

bool Socket::Open(const unsigned short port) {
#if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup(MAKEWORD(2,2), &WsaData) == NO_ERROR;
#endif

    // Cast port number into C string
    std::stringstream portSStream;
    portSStream << port;
    const char* portStr = portSStream.str().c_str();

    // Fill address hints
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    // Get possible sockaddrs
    addrinfo* srvInfo;
    int retVal = getaddrinfo(NULL, portStr, &hints, &srvInfo);
    if (retVal != 0) {
        printf("getaddrinfo(): %s\n", gai_strerror(retVal));
        freeaddrinfo(srvInfo);
        return false;
    }

    // Print all possible sockets
#ifdef DEBUG
    std::cout << "-------Opening sockets:-------";
    char IPString[128], SrvString[128];
    for (addrinfo* p = srvInfo; p != NULL; p = p->ai_next) {
        getnameinfo(p->ai_addr, p->ai_addrlen, IPString, 128, SrvString, 128, 0);
        std::cout << std::endl;
        if (p->ai_family == AF_INET) {
            std::cout << "IPv4" << std::endl;
            std::cout << "Address: " << IPString << std::endl;
            std::cout << "Port: " << ntohs(((sockaddr_in*) p->ai_addr)->sin_port) << std::endl;
        } else if (p->ai_family == AF_INET6) {
            std::cout << "IPv6" << std::endl;
            std::cout << "Address: " << IPString << std::endl;
            std::cout << "Port: " << ntohs(((sockaddr_in6*) p->ai_addr)->sin6_port) << std::endl;
        } else {
            std::cout << "Unknown address family" << std::endl;
        }
    }
#endif

    _Sock newSock;
    for (addrinfo* p = srvInfo; p != NULL; p = p->ai_next) {
        // Create the socket
        newSock.descriptor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (newSock.descriptor == -1) {
            perror("socket()");
            freeaddrinfo(srvInfo);
            return false;
        }

        // Bind
        retVal = bind(newSock.descriptor, p->ai_addr, p->ai_addrlen);
        if (retVal == -1) {
            perror("bind()");
            freeaddrinfo(srvInfo);
            return false;
        }

        // Make it non-blocking
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        int nonBlocking = 1;
        retVal = fcntl(newSock.descriptor, F_SETFL, O_NONBLOCK, nonBlocking);
        if (retVal == -1) {
            perror("fcntl()");
            freeaddrinfo(srvInfo);
            return false;
        }
#elif PLATFORM == PLATFORM_WINDOWS
        DWORD nonBlocking = 1;
        retval = ioctlsocket(newSock.descriptor, FIONBIO, &nonBlocking);
        if (retVal != 0) {
            printf("ioctlsocket(): Error %d\n", WSAGetLastError());
            freeaddrinfo(srvInfo);
            return false;
        }
#endif

        // Add the new socket to the list
        newSock.address = *((sockaddr_storage*) p->ai_addr);
        _socketDescs.push_back(newSock);
    }

    freeaddrinfo(srvInfo);
    return true;
}

/*
** Close all sockets
*/

void Socket::Close() {
#if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
#endif

    for (std::vector<_Sock>::iterator itSocket = _socketDescs.begin(); itSocket != _socketDescs.end(); itSocket++) {
#if PLATFORM == PLATFORM_WINDOWS
        closesocket(itSocket->descriptor);
#else
        int retVal = close(itSocket->descriptor);
        if (retVal == -1) {
            perror("close()");
        }
#endif
    }
}

/*
** Send data to the destination
** For IPv4/6 addresses the IPv4/6 socket is used respectively
*/

bool Socket::Send(const Address& destination, const void *data, const int size) {
    // Find socket with matching IP version
    std::vector<_Sock>::iterator itSocket;
    for (itSocket = _socketDescs.begin(); itSocket != _socketDescs.end(); itSocket++) {
        // Does the IP version match?
        if (itSocket->address.ss_family == destination.Family()) {
#ifdef DEBUG
            char IPString[128], SrvString[128];
            getnameinfo(destination.Sockaddr(), destination.Size(), IPString, 128, SrvString, 128, 0);

            std::cout << "-----------Sending:-----------" << std::endl;
            std::cout << "Using socket " << itSocket->descriptor
                      << " (IPv" << (itSocket->address.ss_family == AF_INET ? "4" : "6") << ")" << std::endl
                      << "Destination: " << IPString << " (IPv" << (itSocket->address.ss_family == AF_INET ? "4" : "6")
                      << ")" << std::endl;
#endif

            // Send the data
            int sentBytes = 0;
            while (sentBytes != size) {
                int retVal = sendto(itSocket->descriptor, data, size, 0, destination.Sockaddr(), destination.Size());
                if (retVal <= 0) {
#ifdef DEBUG
                    std::cout << "Socket::Send() failed." << std::endl;
#endif
                    perror("sendto()");
                    return false;
                } else {
                    sentBytes += retVal;
                }
            }

            return true;
         }
    }

    // We did not find a valid socket
#ifdef DEBUG
    std::cout << "Socket::Send(): no matching socket found" << std::endl;
#endif
    return false;
}

/*
** Receive <size> Bytes from the first socket with data in queue
*/

unsigned int Socket::Receive(Address& sender, void *data, int size) {
    // Any sockets open?
    if (_socketDescs.empty()) {
#ifdef DEBUG
        std::cout << "Socket::Receive(): no open sockets" << std::endl;
#endif
        return -1;
    }

    sockaddr_storage senderSockaddr;
    memset(&senderSockaddr, 0, sizeof(senderSockaddr));
    socklen_t senderSocklen = sizeof(senderSockaddr);
    for (std::vector<_Sock>::iterator itSocket = _socketDescs.begin();
         itSocket != _socketDescs.end();
         itSocket++) {
        // Try to receive from first socket
        int retVal = recvfrom(itSocket->descriptor, data, size, 0, (sockaddr*) &senderSockaddr, &senderSocklen);
        if (retVal <= 0) {
            // No data; continue with next socket
            continue;
        } else {
            // Data received
#ifdef DEBUG
            char IPString[128], SrvString[128];
            getnameinfo((sockaddr*) &senderSockaddr, senderSocklen, IPString, 128, SrvString, 128, 0);
            std::cout << "----------Receiving:----------" << std::endl;
            std::cout << "From socket " << itSocket->descriptor
                      << " (IPv" << (itSocket->address.ss_family == AF_INET ? "4" : "6") << ")" << std::endl
                      << "Source: " << IPString << " (IPv" << (senderSockaddr.ss_family == AF_INET ? "4" : "6") << ")" << std::endl;
#endif

            sender.setSockaddr(senderSockaddr);
            return retVal;
        }
    }

    // No sockets have data to be read
#ifdef DEBUG
    std::cout << "Socket::Receive(): no sockets have data to be read" << std::endl;
#endif
    return -1;
}

bool Address::operator ==(const Address& address) const {
    return ((Sockaddr() == address.Sockaddr()) && (Port() == address.Port()));
}

/* DELETE */
/*
void testNetwork() {
    std::string msg = "Hello World!";
    char receiveBuf[NET_MAX_BODY_SIZE];
    Address addressSelf4("127.0.0.1", NET_PORT), sender;
    Address addressSelf6("::1", NET_PORT);
    Socket socketSelf;
    socketSelf.Open(NET_PORT);
    socketSelf.Send(addressSelf4, msg.c_str(), msg.size());
    sleep(1);
    socketSelf.Receive(sender, receiveBuf, NET_MAX_BODY_SIZE);
    socketSelf.Send(addressSelf6, msg.c_str(), msg.size());
    sleep(1);
    socketSelf.Receive(sender, receiveBuf, NET_MAX_BODY_SIZE);
}
*/
