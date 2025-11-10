#include "Socket.hpp"

Socket::Socket()
{
    return;
}

Socket::Socket(Conf conf)
{
    this->_serverAddress = NULL;
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string host = conf.getHost();
    std::string port = conf.getPort();

    getaddrinfo(host.c_str(), port.c_str(), &hints, &this->_serverAddress);
    this->_host = host;
    this->_port = port;
}

void Socket::listen()
{
    if (::listen(this->_socketListenerFd, 1024) == -1)
        this->_socketListenerFd = -1;
}

Socket::Socket(const Socket& socket)
{
    *this = socket;
    return;
}

Socket& Socket::operator=(const Socket& socket)
{
    this->_host = socket._host;
    this->_port = socket._port;
    return *this;
}

void Socket::createAndBind()
{
    struct addrinfo *rp;
    for (rp = this->_serverAddress; rp != NULL; rp = rp->ai_next)
    {
        this->_socketListenerFd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (this->_socketListenerFd == -1)
            continue;

        int opt = 1;
        setsockopt(this->_socketListenerFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if (bind(this->_socketListenerFd, rp->ai_addr, rp->ai_addrlen) == 0)
        {
            struct sockaddr_in my_addr;
            char myIP[16];
            unsigned int myPort;
            bzero(&my_addr, sizeof(my_addr));
            socklen_t len = sizeof(my_addr);
            getsockname(this->_socketListenerFd, (struct sockaddr *) &my_addr, &len);
            inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
            myPort = ntohs(my_addr.sin_port);
            std::cout << "bind FD[" << this->_socketListenerFd << "]" << " on :  ";
            std::cout << myIP;
            std::cout << ":" << myPort << std::endl;
            break;
        }

        close(this->_socketListenerFd);
    }
}

int Socket::getSocketListenerFd()
{
    return this->_socketListenerFd;
}

std::string Socket::getHost()
{
    return this->_host;
}

std::string Socket::getPort()
{
    return this->_port;
}

Socket::~Socket()
{
    if (this->_serverAddress)
    {
        int sl = this->_socketListenerFd;
        if (sl > -1)
            close(sl);
        freeaddrinfo(this->_serverAddress);
    }
}
