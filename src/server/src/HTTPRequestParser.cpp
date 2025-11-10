#include "HTTPRequestParser.hpp"

HTTPRequestParser::HTTPRequestParser()
{
    return;
}

HTTPRequestParser::HTTPRequestParser(const HTTPRequestParser & src)
{
    *this = src;
    return;
}

HTTPRequestParser& HTTPRequestParser::operator=(const HTTPRequestParser& other)
{
    (void)other;
    return *this;
}


void addHeaderField(HTTPRequest& request, std::string& line)
{
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    size_t colon_pos = line.find(":");
    std::string lvalue(line.substr(0, colon_pos));
    std::string rvalue(line.substr(colon_pos + 1));
    std::transform(lvalue.begin(), lvalue.end(), lvalue.begin(), ::tolower);
	if (lvalue == "host")
	{
		size_t host = rvalue.find(":");
		if (host != std::string::npos)
			rvalue.erase(host);
	}
    request.headers[lvalue] = rvalue;
}

void addStartLine(HTTPRequest& request, std::string& line)
{
    std::istringstream ss(line);
    ss >> request.method >> request.uri >> request.protocol;
    if (ss.peek() != std::char_traits<char>::eof() \
    || request.protocol.empty())
    {
        throw(HTTPRequestParser::requestException());
    }

    if (request.method != "POST" && request.method != "GET" && request.method != "DELETE")
        throw(HTTPRequestParser::methodException());

    if (request.protocol != "HTTP/1.1")
        throw(HTTPRequestParser::versionException());
        // GET /cgi-bin/script.py/test/user1?name=ruben

    size_t querystring = request.uri.find("?");
    if (querystring != std::string::npos && querystring + 1 != std::string::npos)
    {
        request.query_string = request.uri.substr(querystring + 1);
        request.uri.erase(querystring);
    }
    request.start_line = request.method + " " + request.uri + " " + request.protocol;
}

int isChunkedComplete(std::string& buffer, HTTPRequest& request)
{
    size_t pos = 0;
    while (true)
    {
        size_t endline = buffer.find("\r\n", pos);
        if (endline == std::string::npos)
            break;

        std::string lenStr = buffer.substr(pos, endline - pos);

        size_t len;
        std::istringstream(lenStr) >> std::hex >> len;
        pos = endline + 2;

        endline = buffer.find("\r\n", pos);
        if (endline == std::string::npos)
            break;

        std::string str = buffer.substr(pos, endline - pos);
        if (str.size() != len)
            throw(HTTPRequestParser::requestException());
        buffer.erase(0, endline + 2);
        if (len == 0)
            return 1;
        request.body += str;
        pos = 0;
    }
    return 0;
}

int isBodyCorrespondLength(std::string& length, std::string& buffer)
{
    std::istringstream ss(length);
    size_t size;
    ss >> size;
    size_t buffersize = buffer.size();
    if (size == buffersize)
        return 1;
    return 0;
}

void HTTPRequestParser::checkIfComplete(HTTPRequest& request, std::string& buffer)
{
    std::map<std::string, std::string>::iterator it;
    it = request.headers.find("content-length");
    if (it != request.headers.end())
    {
        request.body += buffer;
        buffer.clear();
        if (isBodyCorrespondLength(it->second, request.body))
            request.complete = true;
        return;
    }
    it = request.headers.find("transfer-encoding");
    if (it != request.headers.end() && it->second == "chunked")
    {
        if (isChunkedComplete(buffer, request))
            request.complete = true;
        return;
    }
    request.complete = true;
}

void HTTPRequestParser::fillRequest(HTTPRequest& request, std::string& buffer)
{
    size_t findLine = buffer.find("\r\n");
    while (!request.complete_header && findLine != std::string::npos)
    {
        if (findLine == 0)
        {
            request.complete_header = true;
            buffer.erase(0, 2);
            break;
        }
        size_t colon_pos = buffer.find(":");
        std::string toAdd = buffer.substr(0, findLine);
        if (colon_pos == std::string::npos || colon_pos > findLine)
            addStartLine(request, toAdd);
        else
            addHeaderField(request, toAdd);
        buffer.erase(0, findLine+2);
        findLine = buffer.find("\r\n");
    }
    if (request.complete_header && !request.complete)
        HTTPRequestParser::checkIfComplete(request, buffer);
    return;
}

HTTPRequestParser::~HTTPRequestParser()
{
    return;
}
