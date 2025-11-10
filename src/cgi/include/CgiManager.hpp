#ifndef CGIMANAGER_HPP
#define CGIMANAGER_HPP

#include <vector>
#include <sys/epoll.h>
#include <sys/socket.h>

#include "Cgi.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

class CgiManager
{
    public:
        CgiManager();

        CgiManager(const CgiManager &other);

        CgiManager& operator=(const CgiManager &other);

        ~CgiManager();

        void checkCgiToRun(int& epfd);
		void checkCgiToDie();
        void addCgi(HTTPRequest& req,  int client_fd, HTTPResponse& resp);
        int isCgiFd(uint32_t event, int fd);
        void deleteCgi();
        void getSize();

        int sendData(int fd);
        int receiveData(int fd);

    private:
        int fill_headers(CgiResponse &response);
        int fill_body(CgiResponse &response);
        CgiResponse getCgiResponseFromCgi(int fd);
        int getCgiFdClient(int fd_cgi, HTTPResponse& resp);
        void setCgiResponseToCgi(int fd, CgiResponse &response);
        int updateResponse(CgiResponse& response, int fd_cgi);
        std::vector<Cgi *> _cgis;
};


#endif
