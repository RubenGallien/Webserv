#include "CgiManager.hpp"

CgiManager::CgiManager()
{

}

CgiManager::CgiManager(const CgiManager &other)
{
    *this = other;
}

void CgiManager::getSize()
{
	std::cout << _cgis.size() << std::endl;
}

void CgiManager::addCgi(HTTPRequest& req,  int client_fd, HTTPResponse& resp)
{
	Cgi *cgi = new Cgi(req, client_fd, resp);
	_cgis.push_back(cgi);
}

CgiManager& CgiManager::operator=(const CgiManager &other)
{
    _cgis = other._cgis;
	return *this;
}

CgiManager::~CgiManager()
{
	std::cout << "==== Cgi Manager destructeur ====" << std::endl;
	for (size_t i = 0; i < _cgis.size(); i++)
		delete _cgis[i];
}

void CgiManager::checkCgiToDie()
{
	for (size_t i = 0; i < _cgis.size(); i++)
	{
		if (this->_cgis[i]->healtCheck() == -1)
		{
			int cgi = this->_cgis[i]->getFdOut();
			CgiResponse response = getCgiResponseFromCgi(cgi);
			response._complete = true;
			response._status = "504";
			setCgiResponseToCgi(cgi, response);
		}
	}
}

void CgiManager::deleteCgi()
{
	delete _cgis.front();
	_cgis.erase(_cgis.begin());
}

void CgiManager::checkCgiToRun(int& epfd)
{
    for (size_t i = 0; i < this->_cgis.size(); i++)
    {
        if (this->_cgis[i]->execCgi())
        {
            struct epoll_event ev1, ev2;
			ev1.events = EPOLLIN;   // lire ce que le CGI écrit
			ev2.events = EPOLLOUT;  // écrire ce que le CGI doit lire

			int fd_read  = this->_cgis[i]->getFdOut(); // lecture (fd2[0])
			int fd_write = this->_cgis[i]->getFdIn();  // écriture (fd1[1])

			ev1.data.fd = fd_read;
			ev2.data.fd = fd_write;

			epoll_ctl(epfd, EPOLL_CTL_ADD, fd_read, &ev1);
			epoll_ctl(epfd, EPOLL_CTL_ADD, fd_write, &ev2);
		}
	}
}

int CgiManager::isCgiFd(uint32_t event, int fd)
{
	switch (event)
	{
		case EPOLLIN:
		{
			for (size_t x = 0; x < _cgis.size(); x++)
			{
				if (fd == _cgis[x]->getFdOut())
					return 1;
			}
			break;
		}
		case EPOLLHUP:
		{
			for (size_t x = 0; x < _cgis.size(); x++)
			{
				if (fd == _cgis[x]->getFdOut())
					return 1;
				else if (fd == _cgis[x]->getFdIn())
					return 2;
			}
			break;
		}
		case EPOLLHUP | EPOLLIN:
		{
			for (size_t x = 0; x < _cgis.size(); x++)
			{
				if (fd == _cgis[x]->getFdOut())
					return 1;
				else if (fd == _cgis[x]->getFdIn())
					return 2;
			}
			break;
		}
		case EPOLLOUT:
		{
			for (size_t x = 0; x < _cgis.size(); x++)
			{
				if (fd == _cgis[x]->getFdIn())
					return 1;
			}
			break;
		}
		default:
			break;
	}
	return 0;
}

void CgiManager::setCgiResponseToCgi(int fd, CgiResponse& response)
{
	for (size_t i = 0; i < _cgis.size(); i++)
	{
		if (_cgis[i]->getFdOut() == fd)
		{
			_cgis[i]->setCgiResponse(response);
			break;
		}
	}
}

CgiResponse CgiManager::getCgiResponseFromCgi(int fd)
{
	CgiResponse newResponse;
	for (size_t i = 0; i < _cgis.size(); i++)
	{
		if (_cgis[i]->getFdOut() == fd)
		{
			newResponse = _cgis[i]->getCgiResponse();
			break;
		}
	}
	return newResponse;
}

int CgiManager::getCgiFdClient(int fd_cgi, HTTPResponse& resp)
{
	int fd = 0;
	for (size_t i = 0; i < _cgis.size(); i++)
	{
		if (_cgis[i]->getFdOut() == fd_cgi)
		{
			fd = _cgis[i]->getFdClient();
			_cgis[i]->setHttpResponse(resp);
			break;
		}
	}
	return fd;
}

int validCgiResponse(CgiResponse& response)
{
	if (!response._complete_header || !response._content_type.size())
		return (0);
	if (response._general_headers.count("content-length"))
	{
		std::istringstream ss(response._general_headers["content-length"]);
		size_t size;
		ss >> size;
		if (size != response._body.size())
			return (0);
	}
	return (1);
}

int CgiManager::updateResponse(CgiResponse& response, int fd_cgi)
{
	int status;
	if (response._status.size() && response._status != "504" && !validCgiResponse(response))
		response._status = "500";
	if (response._status.size())
		std::istringstream(response._status) >> status;
	else
		status = 200;
	HTTPResponse resp(status, response._location);
	resp.setHeaders("Content-Type", response._content_type);
	for (std::map<std::string, std::string>::iterator it = response._general_headers.begin(); it != response._general_headers.end(); it++)
		resp.setHeaders(it->first, it->second);
	if (status < 500)
	{
		resp.setBody(response._body);
	}
	else if (status > 499)
		resp.setHeaders("Connection", "close");
	int fd_client = getCgiFdClient(fd_cgi, resp);
	std::cout << " = = = Client FD : " << fd_client << " = = =" << std::endl;
	return fd_client;
}

int CgiManager::fill_headers(CgiResponse &response)
{
	size_t endline;
	while (true)
	{
		endline = response._buffer.find("\n");
		if (endline == std::string::npos)
			return 0;
		else if (!endline)
		{
			response._buffer.erase(0,1);
			response._complete_header = true;
			return 0;
		}
		std::string line = response._buffer.substr(0, endline);
		line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
		size_t colon_pos = line.find(":");
		std::string lvalue(line.substr(0, colon_pos));
    	std::string rvalue(line.substr(colon_pos + 1));
		std::transform(lvalue.begin(), lvalue.end(), lvalue.begin(), ::tolower);
		if (lvalue == "status")
			response._status = rvalue;
		else if (lvalue == "content-type")
			response._content_type = rvalue;
		else if (lvalue == "location")
			response._location = rvalue;
		else
			response._general_headers[lvalue] = rvalue;
		response._buffer.erase(0, endline + 1);
	}
	return (1);
}

int CgiManager::fill_body(CgiResponse &response)
{
	std::istringstream ss;
	response._body += response._buffer;
	if (response._general_headers.count("content-length"))
	{
		std::istringstream ss(response._general_headers["content-length"]);
		size_t size;
		ss >> size;
		if (size == response._body.size())
			response._complete = true;
	}
	response._buffer.clear();
	return (response._complete);
}

int CgiManager::receiveData(int fd)
{
	std::cout << " ==== RECEIVE DATA FROM " << fd << " ====" << std::endl;
	char buffer[1024];
	CgiResponse response = getCgiResponseFromCgi(fd);
	ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
	int	it = 1;
	buffer[bytes] = 0;
	if (bytes < 1)
	{
		response._complete = true;
		it = 0;
	}
	else
		response._buffer += buffer;
	while (it)
	{
		if (!response._complete_header)
			it = fill_headers(response);

		if (!response._complete)
			it = fill_body(response);
	}
	setCgiResponseToCgi(fd, response);
	if (response._complete)
	{
		std::cout << " = = = FD : " << fd << "      = = =" << std::endl;
		it = updateResponse(response, fd);
		return (it);
	}
	return (it);
}

int CgiManager::sendData(int fd)
{

	std::string body;

	for (size_t i = 0; i < _cgis.size(); i++)
	{
		if (fd == _cgis[i]->getFdIn())
			body = _cgis[i]->getBody();
	}

	if (body.size())
		write(fd, body.c_str(), body.size());
	return -1;
}

