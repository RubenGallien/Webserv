#include "HTTPRequestHandler.hpp"

HTTPRequestHandler::HTTPRequestHandler(HTTPRequest& req) : _req(req)
{
	_confNum = 0;
	return;
}

HTTPRequestHandler::HTTPRequestHandler(const HTTPRequestHandler& handler) : _req(handler._req)
{
	_confNum = 0;
	return;
}

bool HTTPRequestHandler::payloadTooLarge(Conf & conf)
{
	size_t max_size = conf.getMaxRequestSize();
	if (max_size < _req.body.size() && max_size != 0)
		return 0;
	else
		return 1;
}

bool HTTPRequestHandler::checkRedirection(Location & loc, Conf &confs)
{
	int status = loc.getStatusCode();
	if (!status)
		return false;
	std::string  red = loc.getRedirection();
	this->_resp = HTTPResponse(status, _req.location, confs);
	if (status > 300 && status < 308)
	{
		this->_resp.setHeaders("Location", red);
		if (this->_req.body.size())
			this->_resp.setBody(this->_req.body);
	}
	else if (red.size())
		this->_resp.setBody(red);
	return true;
}

void HTTPRequestHandler::findContentType()
{
	std::string resources = this->_req.uri;
	size_t findExt = resources.rfind('.');
	std::string ext;
	if (findExt != std::string::npos)
		ext = resources.substr(findExt);
	this->_resp.setContentType(ext);
}
void HTTPRequestHandler::findConnection()
{
	std::map<std::string, std::string>::iterator it = this->_req.headers.find("connection");
	if (it != this->_req.headers.end())
		this->_resp.setConnection(it->second);
	else
		this->_resp.setConnection("keep-alive");
}

void	HTTPRequestHandler::getFileBody(const char * file)
{
	int fd = open(file, O_RDONLY);

	std::string content;
	char buffer[4096];
	ssize_t n;

	while ((n = read(fd, buffer, sizeof(buffer))) > 0)
		content.append(buffer, n);
	close(fd);
	this->_resp = HTTPResponse(200, _req.location);
	this->_resp.setBody(content);
}

int HTTPRequestHandler::methodAllowed(std::string current, std::vector<std::string> allowed)
{
	if (_req.uri.find("/default") != std::string::npos)
		return (1);
	else if (!allowed.size())
		return (0);
	if (allowed.size() && std::find(allowed.begin(), allowed.end(), current) != allowed.end())
		return (1);
	else
		return (0);
}

void	HTTPRequestHandler::getAutoIndexBody()
{
	DIR *directory;
	std::string body =	"<!DOCTYPE html>\r\n"
						"<html lang = \"fr\" >\r\n"
						"<head>\r\n"
						"<title> Liste de fichiers</title>\r\n"
						"</head>\r\n"
						"<body>\r\n"
						"<h1> Liste de fichiers disponibles</h1>\r\n"
						"<ul>\r\n"
						;
	struct dirent *file;
	std::string root = _root;

	directory = opendir(root.c_str());

	std::string list = "<li><a href = \"";
	while ((file = readdir(directory)) != NULL)
	{

		body += list + file->d_name + "\"";
		body += ">";
		body += file->d_name;
		body += "</a></ li>\r\n";
	}
	closedir(directory);
	body += "</ ul>\r\n</body> </html>\r\n<meta charset = \"UTF-8\">\r\n";
	this->_resp = HTTPResponse(200, _req.location);
	this->_resp.setBody(body);
	return;
}

void	HTTPRequestHandler::getPath(std::map<std::string, Location>::iterator &it)
{
	_root = it->second.getRoot();

	if (it->first == "/" && this->_req.uri != "/")
		_root += this->_req.uri;
	else if (_root.size() && it->first.size() < this->_req.uri.size())
		_root += this->_req.uri.substr(it->first.size(), this->_req.uri.size());

}

int HTTPRequestHandler::checkDelete()
{
	struct stat sb;
	if (stat(_root.c_str(), &sb) == -1)
		return (404);
	if (S_ISDIR(sb.st_mode))
		return (403);

	if (remove(_root.c_str()) == 0)
	{
		this->_resp = this->_resp = HTTPResponse(204, _req.location);
		return (204);
	}
	else
		return (403);
	return 1;
}

int HTTPRequestHandler::checkPost(std::string upload, std::string content_type)
{
	int code = 200;
	if (!upload.size())
		return (403);
	if (content_type == "application/x-www-form-urlencoded" || content_type == "text/plain")
		code =  classicUpload(upload, _req.body, _req.location);
	else
		code =  multipartUpload(upload, _req.body, _req.location, content_type);
	if (code <= 205)
		this->_resp = this->_resp = HTTPResponse(code, _req.location);
	return code;
}

int HTTPRequestHandler::checkGet(std::string index, bool autoindex)
{
	struct stat sb;
	if (stat(_root.c_str(), &sb) == -1)
		return (404);
	if (access(_root.c_str(), R_OK) == -1)
		return (403);
	if (S_ISREG(sb.st_mode))
		getFileBody(_root.c_str());
	else if (S_ISDIR(sb.st_mode))
	{
		if (index.size())
		{
			_root += "/";
			_root += index;
			if (stat(_root.c_str(), &sb) == -1)
				return (404);
			if (access(_root.c_str(), R_OK) == -1)
				return (403);
			if (S_ISREG(sb.st_mode))
				getFileBody(_root.c_str());
			else
			{
				if (!autoindex)
					return (403);
				else
					getAutoIndexBody();
			}
		}
		else
		{
			if (!autoindex)
				return (403);
			else
				getAutoIndexBody();
		}
	}
	return (200);
}

int HTTPRequestHandler::staticRequestCheck(std::map<std::string, Location>:: iterator &it)
{
	int ret = 200;
	std::string index = it->second.getIndex();
	std::string method = _req.method;
	if (method == "GET")
		ret = checkGet(index, it->second.getAutoIndex());
	else if (method == "POST")
		ret = checkPost(it->second.getUpload(), _req.getValueHeader("content-type"));
	else if (method == "DELETE")
		ret = checkDelete();
	else
		ret = 400;
	return ret;
}

std::map<std::string, Location>::iterator HTTPRequestHandler::getCorrectLocation(std::vector<Conf> & confs)
{
	// si pas de host -> error
	std::map<std::string, std::string>::iterator itName = this->_req.headers.find("host");
	if (itName == _req.headers.end())
		return confs.begin()->getLocation().end();
	// on cherche les location d'un bloc serveur associe au host:<quelquechose>
	std::map<std::string, Location> * loc = NULL;
	size_t nb_conf = 0;

	for (std::vector<Conf>::iterator it = confs.begin(); it != confs.end(); it++)
	{
		std::vector<std::string> vecServerName = it->getServerName();
		for (size_t i = 0; i < vecServerName.size(); i++)
		{
			if (vecServerName[i] == itName->second)
			{
				_confNum = nb_conf;
				loc = &it->getLocation();
				break;
			}
		}
		nb_conf++;
	}
	if (!loc)
		loc = &confs.begin()->getLocation();
	std::string tmp = "";
	for (std::map<std::string, Location>::iterator it = loc->begin(); it != loc->end(); it++)
	{
		if (_req.uri.find(it->first) != std::string::npos)
		{
			if (it->first.size() > tmp.size())
				tmp  = it->first;
		}
	}
	std::map<std::string, Location>::iterator itLoc =  loc->find(tmp);
	return itLoc;
}

HTTPResponse HTTPRequestHandler::handle(std::vector<Conf> & confs)
{
	int ret = 0;
	std::map<std::string, Location>::iterator it = this->getCorrectLocation(confs);
	if (it == confs.begin()->getLocation().end())
		return HTTPResponse(404, _req.location, confs[_confNum]);
	if (!payloadTooLarge(confs[_confNum]))
		return HTTPResponse(413, _req.location, confs[_confNum]);
	if (checkRedirection(it->second, confs[_confNum]))
		return (this->_resp);
	if (!methodAllowed(this->_req.method, it->second.getMethod()))
		return HTTPResponse(405, _req.location, confs[_confNum]);
	getPath(it);
	if (isCgi(this->_root, this->_req.path_info, it, this->_req.cgi))
	{
		this->_req.uri = _root;
		if (!this->_req.cgi)
			ret = 403;
		else
			ret = 200;
		return (HTTPResponse(ret, _req.location, confs[_confNum]));
	}
	ret = staticRequestCheck(it);
	if (ret > 205)
		return (HTTPResponse(ret, _req.location, confs[_confNum]));
	this->findContentType();
	this->findConnection();
	return this->_resp;
}

HTTPRequestHandler& HTTPRequestHandler::operator=(const HTTPRequestHandler& handler)
{
	this->_req = handler._req;
	this->_resp = handler._resp;
	this->_root = handler._root;
	this->_sb = handler._sb;
	this->_confNum = handler._confNum;
	return *this;
}

HTTPRequest & HTTPRequestHandler::getRequest()
 {
	return this->_req;
 }

HTTPRequestHandler::~HTTPRequestHandler()
{
	return;
}
