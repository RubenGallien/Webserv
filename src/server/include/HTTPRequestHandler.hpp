#ifndef HTTPREQUESTHANDLER_HPP
#define HTTPREQUESTHANDLER_HPP

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "Conf.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "CGIUtils.hpp"
#include <dirent.h>
#include <fstream>
#include <stdio.h>
#include <Upload.hpp>

class HTTPRequestHandler
{

    public:
        HTTPRequestHandler(HTTPRequest& req);
        HTTPRequestHandler(const HTTPRequestHandler& handler);

        HTTPRequestHandler& operator=(const HTTPRequestHandler& handler);

        HTTPRequest & getRequest();

        HTTPResponse    handle(std::vector<Conf> & confs);


        ~HTTPRequestHandler();

    private:
        struct stat      _sb;
        std::string _root;
        HTTPRequest &_req;
        HTTPResponse    _resp;
		size_t _confNum;
        std::map<std::string, Location>::iterator       getCorrectLocation(std::vector<Conf> & confs);
        void findContentType();
        void findConnection();
        int methodAllowed(std::string current, std::vector<std::string> allowed);
		bool checkRedirection(Location & loc, Conf &confs);
        bool payloadTooLarge(Conf & conf);

        void getPath(std::map<std::string, Location>::iterator &it);
        int staticRequestCheck(std::map<std::string, Location>::iterator &it);

        // method
        int checkGet(std::string index, bool autoindex);
        int checkDelete();
        int checkPost(std::string upload, std::string content_type);

        void getFileBody(const char *file);
        void getAutoIndexBody();
};

#endif
