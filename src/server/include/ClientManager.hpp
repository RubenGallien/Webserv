#ifndef CLIENTMANAGER_HPP
#define CLIENTMANAGER_HPP

#include <string>
#include <iostream>
#include <unistd.h>
#include <map>
#include <stdlib.h>
#include <sys/socket.h>

#include "Client.hpp"
#include "Socket.hpp"
#include "Client.hpp"
#include "HTTPRequestParser.hpp"
#include "HTTPRequestHandler.hpp"
#include "HTTPResponse.hpp"
#include "Cgi.hpp"
#include "CgiManager.hpp"

class ClientManager {
    public:
        ClientManager();
        ~ClientManager();

        void    addClient(int fd, Socket * socket,std::vector<Conf>* confs);
        int     receiveData(int fd, CgiManager& cgiManager);
        int     sendData(int fd);


        Client * getClient(int fd);
        void     deleteClient(int client_fd);

        int         getNbClient();

    private:
        std::map<int, Client *> _clients;
        void sendCatchError(int fd, std::string reason);
        int     prepareClientResponse(Client * client, CgiManager& cgiManager);

};

#endif
