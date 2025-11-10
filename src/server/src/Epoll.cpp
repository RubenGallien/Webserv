#include "Epoll.hpp"

Epoll::Epoll(int readSignalFd, std::vector<Conf> &confs, std::vector<Socket*> &sockets) :
_readSignalFd(readSignalFd),
_confs(confs),
_sockets(sockets),
_clientManager(),
_cgiManager()
{
    (void)_confs;
    this->_acceptFd = -1;
    this->_epfd = epoll_create1(0);
    if (this->_epfd == -1)
        std::cout << strerror(errno) << std::endl;

    struct epoll_event ev;
    for (size_t i = 0; i < this->_sockets.size(); i++)
    {
        int fd = this->_sockets[i]->getSocketListenerFd();
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        epoll_ctl(this->_epfd, EPOLL_CTL_ADD, fd, &ev);
    }

    ev.data.fd = readSignalFd;
    epoll_ctl(this->_epfd, EPOLL_CTL_ADD, readSignalFd, &ev);
}

int Epoll::wait()
{
    int nbs;

	this->_cgiManager.checkCgiToDie();
    nbs = epoll_wait(this->_epfd, this->_events, MAX_EVENTS, 1000);

    return nbs;
}

void Epoll::handleCgiRecvReturn(int fd_client, int pipe)
{
    (void)fd_client;
    struct epoll_event ev;
    if (fd_client > 0)
    {
        _cgiManager.getSize();
        std::cout << "je suis ici avec fd_client = " << fd_client << " et pipe = " << pipe << std::endl;
        ev.data.fd = fd_client;
        ev.events = EPOLLOUT;
        close(pipe);
        _cgiManager.deleteCgi();
        _cgiManager.getSize();
        epoll_ctl(this->_epfd, EPOLL_CTL_DEL, pipe, 0);
        epoll_ctl(this->_epfd, EPOLL_CTL_MOD, fd_client, &ev);
    }
}

void Epoll::handleRecvReturn(int status, int fd_client)
{
    (void)fd_client;
    struct epoll_event ev;
    switch (status)
    {
        case -1:
        {
            ev.data.fd = fd_client;
            epoll_ctl(this->_epfd, EPOLL_CTL_DEL, fd_client, 0);
            close(fd_client);
            this->_clientManager.deleteClient(fd_client);
            break;
        }
        // add epollout, stay epollin
        case 1:
        {
            ev.data.fd = fd_client;
            ev.events = EPOLLIN | EPOLLOUT;
            epoll_ctl(this->_epfd, EPOLL_CTL_MOD, fd_client, &ev);
            break;
        }
        case 2:
            ev.data.fd = fd_client;
            ev.events = EPOLLIN;
            epoll_ctl(this->_epfd, EPOLL_CTL_MOD, fd_client, &ev);
            break;
        default:
            break;
    }
}

void Epoll::handleClientEvent(int target_fd, int event)
{
    switch (event)
    {
        case EPOLLIN:
        {
            if (_cgiManager.isCgiFd(event, target_fd))
                handleCgiRecvReturn(_cgiManager.receiveData(target_fd), target_fd);
            else
                handleRecvReturn(_clientManager.receiveData(target_fd, _cgiManager), target_fd);
            break;
        }
        case EPOLLOUT:
            if (_cgiManager.isCgiFd(event, target_fd))
                handleRecvReturn(_cgiManager.sendData(target_fd), target_fd);
            else
                handleRecvReturn(_clientManager.sendData(target_fd), target_fd);
            break;
        case EPOLLOUT | EPOLLIN:
            handleRecvReturn(_clientManager.sendData(target_fd), target_fd);
            break;
        case EPOLLHUP:
        {
            int test = _cgiManager.isCgiFd(event, target_fd);
            if (test == 1)
                handleCgiRecvReturn(_cgiManager.receiveData(target_fd), target_fd);
            else if (test == 2)
                handleCgiRecvReturn(_cgiManager.sendData(target_fd), target_fd);
            break;
        }
        case EPOLLIN | EPOLLHUP:
        {
            int test = _cgiManager.isCgiFd(event, target_fd);
            if (test == 1)
                handleCgiRecvReturn(_cgiManager.receiveData(target_fd), target_fd);
            else if (test == 2)
                handleCgiRecvReturn(_cgiManager.sendData(target_fd), target_fd);
            break;
        }
        default:
            break;
    }
}

int Epoll::epollAccept(int target_fd)
{
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_size = sizeof(peer_addr);

    int acceptFd = accept(target_fd, (struct sockaddr*)&peer_addr, &peer_addr_size);
    if (acceptFd == -1) {
        std::cout << strerror(errno) << std::endl;
        return -1;
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = acceptFd;
    epoll_ctl(this->_epfd, EPOLL_CTL_ADD, acceptFd, &ev);
    this->_clientManager.addClient(acceptFd, this->findSocket(target_fd), &this->_confs);
    return (0);
}

int Epoll::actions(int nbs)
{
    for (int i = 0; i < nbs ; i++)
    {
        int target_fd = this->_events[i].data.fd;
        if (target_fd == this->_readSignalFd)
            return(0);
        else if (this->targetIsListenerFd(target_fd))
        {
            if (this->epollAccept(target_fd) == -1)
                continue;
        }
        else
            this->handleClientEvent(target_fd, this->_events[i].events);
        std::cout << "==== check_cgi ====" << std::endl;
        _cgiManager.checkCgiToRun(this->_epfd);
    }
    return (1);
}


Socket * Epoll::findSocket(int fd)
{
    for (std::vector<Socket*>::iterator it = this->_sockets.begin() ; it != this->_sockets.end(); ++it)
    {
        if ((*it)->getSocketListenerFd() == fd)
            return *it;
    }
    return NULL;
}

int Epoll::getEpfd()
{
    return this->_epfd;
}

int Epoll::targetIsListenerFd(int target)
{
    for (size_t i = 0; i < this->_sockets.size(); i++)
    {
        if (target == this->_sockets[i]->getSocketListenerFd())
            return (1);
    }
    return (0);
}

Epoll::~Epoll()
{
    return ;
}
