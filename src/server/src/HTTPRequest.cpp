#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
{
    this->complete = false;
    this->complete_header = false;
    this->cgi = false;
    return;
}

HTTPRequest::HTTPRequest(const HTTPRequest& src)
{
    *this = src;
    return;
}

HTTPRequest& HTTPRequest::operator=(const HTTPRequest& other)
{
    this->complete = other.complete;
    this->start_line = other.start_line;
    this->method = other.method;
    this->path_info = other.path_info;
    this->query_string = other.query_string;
    this->complete_header = other.complete_header;
    this->complete = other.complete;
    this->headers = other.headers;
    this->body = other.body;
    this->uri = other.uri;
    this->protocol = other.protocol;
    this->cgi = other.cgi;
    this->location = other.location;
    return *this;
}

std::string HTTPRequest::getValueHeader(std::string key)
{
    std::map<std::string, std::string>::iterator it = headers.find(key);
    if (it != headers.end())
        return it->second;
    else
        return "";
}

HTTPRequest::~HTTPRequest()
{
    return;
}
