#include "HTTPResponse.hpp"

std::string HTTPResponse::getReason(int status, std::string& location, Conf *conf)
{
    static std::map<int, std::string> reasons;
    reasons[100] = "100 Continue";
    reasons[101] = "101 Switching Protocols";
    reasons[102] = "102 Processing";
    reasons[200] = "200 OK";
    reasons[201] = "201 Created";
    reasons[202] = "202 Accepted";
    reasons[204] = "204 No Content";
    reasons[206] = "206 Partial Content";
    reasons[300] = "300 Multiple Choices";
    reasons[301] = "301 Moved Permanently";
    reasons[302] = "302 Found";
    reasons[303] = "303 See Other";
    reasons[304] = "304 Not Modified";
    reasons[307] = "307 Temporary Redirect";
    reasons[308] = "308 Permanent Redirect";
    reasons[400] = "400 Bad Request";
    reasons[401] = "401 Unauthorized";
    reasons[402] = "402 Payment Required";
    reasons[403] = "403 Forbidden";
    reasons[404] = "404 Not Found";
    reasons[405] = "405 Method Not Allowed";
    reasons[406] = "406 Not Acceptable";
    reasons[408] = "408 Request Timeout";
    reasons[409] = "409 Conflict";
    reasons[410] = "410 Gone";
    reasons[411] = "411 Length Required";
    reasons[413] = "413 Payload Too Large";
    reasons[414] = "414 URI Too Long";
    reasons[415] = "415 Unsupported Media Type";
    reasons[416] = "416 Range Not Satisfiable";
    reasons[417] = "417 Expectation Failed";
    reasons[418] = "418 I'm a teapot";
    reasons[500] = "500 Internal Server Error";
    reasons[501] = "501 Not Implemented";
    reasons[502] = "502 Bad Gateway";
    reasons[503] = "503 Service Unavailable";
    reasons[504] = "504 Gateway Timeout";
    reasons[505] = "505 HTTP Version Not Supported";
    if (status >= 400)
        setErrorFile(status, conf);
    else if (status >= 300)
        setHeaders("Location", location);
    return reasons.find(status)->second;
}

HTTPResponse::HTTPResponse()
{
    return;
}


std::string getCurrentDateTime()
{
    std::time_t now = std::time(0);
    std::tm tstruct = *std::localtime(&now);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tstruct);

    return std::string(buffer);
}

HTTPResponse::HTTPResponse(int status, std::string& location)
{
	this->_reason = this->getReason(status, location, NULL); // la
    this->_start_line = "HTTP/1.1 " + this->_reason;
    this->_headers["Server"] = "webserv/1.0";
    return;
}

HTTPResponse::HTTPResponse(int status, std::string& location, Conf conf) // need loc
{
    this->_reason = this->getReason(status, location, &conf); // la
    this->_start_line = "HTTP/1.1 " + this->_reason;
    this->_headers["Server"] = "webserv/1.0";
    return;
}

HTTPResponse::HTTPResponse(const HTTPResponse& handler)
{
    *this = handler;
}

void HTTPResponse::setHeaders(std::string key, std::string value)
{

    _headers[key] = value;
}

void HTTPResponse::setBody(std::string& body)
{
    this->_body = body;
    std::ostringstream oss;
    oss << this->_body.size();
    this->_headers["Content-length"] = oss.str();
}


std::string HTTPResponse::serialize()
{
    std::string resp;

    this->_headers["Date"] = getCurrentDateTime();
    resp = this->_start_line + "\r\n";
    for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
    {
        resp += it->first;
        resp += ": ";
        resp += it->second;
        resp += "\r\n";
    }
    resp += "\r\n";
    resp += this->_body;
    return resp;
}

void HTTPResponse::setErrorFile(int code, Conf *conf)
{
    std::string contenu, ligne, filename;
    std::ostringstream oss;
	std::string content;
    oss << code;

	if (!conf || (!conf->findError(code, &filename) || \
	(conf->findError(code, &filename) && access(filename.c_str(), F_OK | R_OK) == -1)))
	{
		filename = "default/default/index.html";
		std::ifstream in(filename.c_str());

		while (std::getline(in, ligne))
		{
			if (ligne.find("const codeString =") != std::string::npos)
				ligne = "const codeString = \"" + oss.str() + "\";";
			contenu += ligne + "\n";
		}
		in.close();

		std::ofstream out(filename.c_str());
		out << contenu;

	}
	int fd = open(filename.c_str(), O_RDWR);

	if (fd > 0)
	{
		char buffer[4096];
		ssize_t n;

		while ((n = read(fd, buffer, sizeof(buffer))) > 0)
			content.append(buffer, n);
		close(fd);
		this->setContentType(".html");
		this->setBody(content);
	}
	else
	{
		std::stringstream str;

		str << code;
		std::string codeStr = str.str();
		this->setBody(codeStr);
	}
}

HTTPResponse& HTTPResponse::operator=(const HTTPResponse& handler)
{
    this->_start_line = handler._start_line;
    this->_body = handler._body;
    this->_headers = handler._headers;
    this->_reason = handler._reason;
    return *this;
}
void HTTPResponse::setConnection(std::string cnx)
{
    this->_headers["Connection"] = cnx;
}

std::string HTTPResponse::getConnection()
{
    std::map<std::string, std::string>::iterator it = this->_headers.find("Connection");
    if (it != this->_headers.end())
        return it->second;
    return "";
}
void HTTPResponse::setContentType(std::string ext)
{
    static std::map<std::string, std::string> contentType;
    contentType[".js"] = "application/javascript";
    contentType[".html"] = "text/html";
    contentType[".pdf"] = "application/pdf";
    contentType[".css"] = "text/css";
    contentType[".jpeg"] = "image/jpeg";
    contentType[".jpg"] = "image/jpeg";
    contentType[".png"] = "image/png";
    contentType[".gif"] = "image/gif";
    contentType[".mp3"] = "audio/mpeg";
    contentType[".mp4"] = "video/mp4";
    if (contentType.find(ext) == contentType.end())
        this->_headers["Content-Type"] = "text/html";
    else
        this->_headers["Content-Type"] = contentType.find(ext)->second;
}

HTTPResponse::~HTTPResponse()
{
    return;
}

