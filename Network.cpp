#include "Network.h"

Address::Address()
    : _port(0)
{
    memset(&_address, 0, sizeof(_address));
}

Address::Address(std::string address, unsigned short port)
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
    _address = *(res->ai_addr);
}

Address::Address(sockaddr& address, unsigned short port)
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

    // DELETE: print all possible sockets
    /*
    std::cout << "Possible sockets:" << std::endl << std::endl;
    char IPstring[128];
    for (addrinfo* p = srvInfo; p != NULL; p = p->ai_next) {
        inet_ntop(p->ai_family, p->ai_addr, IPstring, 128);
        if (p->ai_family == AF_INET) {
            std::cout << "IPv4" << std::endl;
            std::cout << "Address: " << IPstring << std::endl;
            std::cout << "Port: " << ntohs(((sockaddr_in*) p->ai_addr)->sin_port) << std::endl;
        } else if (p->ai_family == AF_INET6) {
            std::cout << "IPv6" << std::endl;
            std::cout << "Address: " << IPstring << std::endl;
            std::cout << "Port: " << ntohs(((sockaddr_in6*) p->ai_addr)->sin6_port) << std::endl;
        } else {
            std::cout << "Unknown address family" << std::endl;
        }
        std::cout << std::endl;
    }
    */

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
        newSock.address = *p->ai_addr;
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

    for (std::list<_Sock>::iterator itSocket = _socketDescs.begin(); itSocket != _socketDescs.end(); itSocket++) {
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

bool Socket::Send(const Address& destination, const void *data, int size) {
    // Find socket with matching IP version
    std::list<_Sock>::iterator itSocket;
    for (itSocket = _socketDescs.begin(); itSocket != _socketDescs.end(); itSocket++) {
        // Does the IP version match?
        if (itSocket->address.sa_family == destination.Family()) {
            // Send the data
            int sentBytes = 0;
            while (sentBytes != size) {
                int retVal = sendto(itSocket->descriptor, data, size, 0, destination.Sockaddr(), destination.Size());
                if (retVal <= 0) {
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
    printf("Socket::Send(): no matching socket found\n");
    return false;
}

/*
** Receive <size> Bytes from the first socket with data in queue
*/

int Socket::Receive(Address& sender, void *data, int size) {
    // Any sockets open?
    if (_socketDescs.empty()) {
        printf("Socket::Receive(): no open sockets\n");
        return -1;
    }

    sockaddr senderSockaddr;
    socklen_t senderSocklen;
    for (std::list<_Sock>::iterator itSocket = _socketDescs.begin();
         itSocket != _socketDescs.end();
         itSocket++) {
        int retVal = recvfrom(itSocket->descriptor, data, size, 0, &senderSockaddr, &senderSocklen);
        if (retVal <= 0) {
            perror("recvfrom()");
            return -1;
        } else {
            sender.setSockaddr(senderSockaddr);
            return retVal;
        }
    }

    return -1;
}

/* DELETE */
void testNetwork() {
    std::string msg = "Hello World!";
    char receiveBuf[BUFFER_SIZE];
    Address addressSelf4("127.0.0.1", PORT), sender;
    Address addressSelf6("::1", PORT);
    Socket socketSelf;
    socketSelf.Open(PORT);
    std::cout << "Trying to send using IPv4" << std::endl;
    socketSelf.Send(addressSelf4, msg.c_str(), msg.size());
    std::cout << "Send() returned" << std::endl;
    std::cout << "Trying to send using IPv6" << std::endl;
    socketSelf.Send(addressSelf6, msg.c_str(), msg.size());
    std::cout << "Send() returned" << std::endl;
    sleep(1);
    int retVal;
    do {
        std::cout << "Trying to receive" << std::endl;
        retVal = socketSelf.Receive(sender, receiveBuf, BUFFER_SIZE);
        if (retVal > 0) {
            std::cout << "Received " << retVal << " Byte via ";
            if (sender.Family() == AF_INET) std::cout << "IPv4.";
            else std::cout << "IPv6";
            std::cout << std::endl;
        }
    } while (retVal > 0);
}
