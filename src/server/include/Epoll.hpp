#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <sys/epoll.h>
#include <sys/socket.h>
#include <fstream>
#include <cstring>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <arpa/inet.h>
#include "ClientManager.hpp"
#include "Conf.hpp"
#include "Socket.hpp"
#include "Cgi.hpp"
#include "CgiManager.hpp"

#define MAX_EVENTS 30

class Epoll {

    public:
        Epoll(const int readSignalFd, std::vector<Conf> &confs,std::vector<Socket*> &sockets);
        ~Epoll();
        int wait();
        int actions(int nbs);
        int epollAccept(int target_fd);
        void handleClientEvent(int target_fd, int event);
        int getEpfd();
        int targetIsListenerFd(int target);

        Socket * findSocket(int fd);
    private:
        void                    handleRecvReturn(int status, int fd_client);
        void                    handleCgiRecvReturn(int fd_client, int pipe);
        int                     _readSignalFd;
        std::vector<Conf>&      _confs;
        std::vector<Socket*>&    _sockets;
        ClientManager           _clientManager;
        CgiManager              _cgiManager;
        int                     _epfd;
        int                     _acceptFd;
        struct epoll_event      _events[MAX_EVENTS];
};

#endif
