#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include "Epoll.hpp"
#include "ClientManager.hpp"
#include "ServerConf.hpp"
#include <errno.h>
#include <unistd.h>

class Server {

	public:
		Server(int readSignalFd, std::vector<Conf> confs);
		~Server();

		bool run();

	private:
		std::vector<Conf>		_confs;
		std::vector<Socket *> _sockets;
		Epoll*					_epoll;
};

#endif
