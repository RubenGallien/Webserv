#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "Conf.hpp"

class Client
{
    private:
        int                      _fd;
        std::string              _buffer;
        std::string               _stash;
        std::vector<HTTPRequest> _requests;
        std::vector<HTTPResponse> _responses;
        std::vector<Conf>        _confs;

    public:
        Client();
        Client(int fd);
        Client(const Client& other);

        Client& operator=(const Client& src);


        // getter
        int getFd();
        std::string&                getBuffer();
        HTTPRequest&                getRequest();
        std::vector<HTTPRequest>&   getAllRequest();
        std::vector<Conf>&          getConf();
        HTTPResponse                getResponse();
        HTTPResponse &               getResponseCgi();


        // setter
        void setConf(Conf & conf);
        void setBuffer(std::string data);
        void setResponse(HTTPResponse & response);
        void setStash();

        // requests
        int hasRequests();
        int requestsAllComplete();
        int atleastOneComplete();
        int lastRequestComplete();
        void startNewRequest();
        void addRequest(HTTPRequest &newRequest);

        int hasResponse();
        ~Client();

};

#endif
