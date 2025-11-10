#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>
# include <map>
# include <ctime>
# include <iostream>
# include <stdlib.h>
# include <sstream>
# include <fcntl.h>
# include <unistd.h>
# include <fstream>

# include "Conf.hpp"

class HTTPResponse
{

    public:
        HTTPResponse();
		HTTPResponse(int status, std::string& location);
        HTTPResponse(int status, std::string& location, Conf conf);

        HTTPResponse(const HTTPResponse& handler);

        HTTPResponse& operator=(const HTTPResponse& handler);

        std::string serialize();

        std::string getReason(int status, std::string& location, Conf *conf);
        std::string getConnection();

        void setConnection(std::string cnx);
        void setContentType(std::string ext);
        void setBody(std::string &body);
        void setErrorFile(int code, Conf *conf);
        void setHeaders(std::string key, std::string value);

        ~HTTPResponse();
    private:
        std::string                                     _start_line;
        std::map<std::string, std::string>              _headers;
        std::string                                     _body;
        std::string                                     _reason;

};

#endif
