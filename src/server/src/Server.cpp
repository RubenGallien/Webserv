#include "Server.hpp"


Server::Server(int readSignalFd, std::vector<Conf> confs) : _confs(confs)
{
	Socket * socket;
	for (size_t i = 0; i < confs.size(); i++)
	{
		socket = new Socket(confs[i]);
		socket->createAndBind();
		socket->listen();
		this->_sockets.push_back(socket);
	}
	this->_epoll = new Epoll(readSignalFd, this->_confs, this->_sockets);
}

/**
* Main loop
 */
bool Server::run()
{
	for (;;)
	{
		// wait an event -> do something when happen
		if (!this->_epoll->actions(this->_epoll->wait()))
			return false;
	}
	return true;
}

Server::~Server()
{
	for (size_t i = 0; i < this->_sockets.size(); i++)
	{
		delete this->_sockets[i];
	}
	int fd = this->_epoll->getEpfd();
	if (fd > -1)
		close(fd);
	delete this->_epoll;
}
