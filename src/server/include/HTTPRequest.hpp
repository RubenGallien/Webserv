#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>

struct HTTPRequest
{


    HTTPRequest();
    HTTPRequest(const HTTPRequest& src);
    HTTPRequest& operator=(const HTTPRequest& other);

    bool                                complete;
    bool                                complete_header;
    bool                                cgi;
    std::string                         method;
    std::string                         uri;
    std::string                         path_info;
    std::string                         query_string;
    std::string                         protocol;
    std::string                         start_line;
    std::map<std::string, std::string>  headers;
    std::string                         body;
    std::string                         location;

    std::string getValueHeader(std::string key);
    ~HTTPRequest();
};

#endif
