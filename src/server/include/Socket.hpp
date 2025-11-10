#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include "Conf.hpp"

class Socket {
    
    public:
        Socket(Conf conf);
        Socket();
        Socket(const Socket& socket);

        Socket& operator=(const Socket& socket);

        ~Socket();
        void createAndBind();
        void listen();
        int getSocketListenerFd();
        std::string getHost();
        std::string getPort();
    private:
        std::string     _host;
        std::string     _port;
        int             _socketListenerFd;
        struct addrinfo *_serverAddress;

};

#endif