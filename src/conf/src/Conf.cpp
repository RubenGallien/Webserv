/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvicino <lvicino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 15:28:30 by lvicino           #+#    #+#             */
/*   Updated: 2025/11/05 17:17:15 by lvicino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Conf.hpp"

// Constructor

Conf::Conf(void)
{
	return ;
}

Conf::Conf(const std::string &conf) :
_max_request_size(0) // needs to recieve the content of a server block
{
	std::map<std::string, void (Conf::*)(std::string)>	directive;

	directive["listen"] = &Conf::setHostPort;
	directive["server_name"] = &Conf::setServerName;
	directive["error_page"] = &Conf::setError;
	directive["client_max_body_size"] = &Conf::setMaxRequestSize;
	directive["location"] = &Conf::setLocation;

	size_t	size = conf.size();
	size_t	j = 0;

	_init_listen = false;
	_init_max_request_size = false;

	for (size_t i = 0; i < size; )
	{
		bool	directive_found = false;

		for (std::map<std::string, void (Conf::*)(std::string)>::iterator it = directive.begin(); it != directive.end(); it++)
		{
			while (i < size && std::isspace(conf[i]))
				i++;
			if (i >= size)
				break ;
			j = 0;
			if (conf.find(it->first, i) == i && std::isspace(conf[i + it->first.size()]))
			{
				directive_found = true;
				i += it->first.size();
				while (i < size && std::isspace(conf[i]))
					i++;
				if (it->first != "location")
				{
					if (i >= size)
						throw std::runtime_error("Error: Expected value after '" + it->first + "'");
					while (i + j < size && conf[i + j] != ';')
					{
						if (conf[i + j] == '{')
							throw std::runtime_error("Error: Unexpected '{' after '" + it->first + '\'');
						j++;
					}
					if (i + j == size)
						throw std::runtime_error("Error: expected ';' after '" + it->first + '\'');
				}
				else
				{
					if (i >= size)
						throw std::runtime_error("Error: Expected path after 'location'");
					while (i + j < size && conf[i + j] != '{')
					{
						if (conf[i + j] == ';')
							throw std::runtime_error("Error: Unexpected ';' after '" + it->first + '\'');
						j++;
					}
					if (i + j == size)
						throw std::runtime_error("Error: Expected '{' after 'location'");
					j = conf.find('}', i) - i + 1;
					if (conf[i + j - 1] != '}')
						throw std::runtime_error("Error: Expected '}' after 'location' block");
				}
				(this->*it->second)(conf.substr(i, j));
				i += j + (it->first != "location");
				break ;
			}
		}
		if (i < size && !directive_found)
		{
			j = 0;
			while (i + j < size && !std::isspace(conf[i + j]) && conf[i + j] != ';')
				j++;
			throw std::runtime_error("Unknown configuration directive: '" + conf.substr(i, j) + "'");
		}
	}
	if (!_init_listen)
		throw std::runtime_error("Error: [emerg] \"listen\" directive expected");
	return ;
}

Conf::Conf(const Conf &other)
{
	*this = other;
	return ;
}

Conf::~Conf(void)
{
	return ;
}

// Operator

Conf &Conf::operator=(const Conf &other)
{
	_host = other._host;
	_port = other._port;
	_server_name = other._server_name;
	_error = other._error;
	_max_request_size = other._max_request_size;
	_location = other._location;

	_init_listen = other._init_listen;
	_init_max_request_size = other._init_max_request_size;
	return (*this);
}

std::ostream	&operator<<(std::ostream &os, const Conf &conf)
{
	std::vector<ErrorPath>			er = conf.getError();
	std::map<std::string, Location>	loc = conf.getLocation();
	std::vector<std::string>		server_name = conf.getServerName();

	for (size_t i = 0; i < server_name.size(); i++)
		std::cout << "server name = " << server_name[i] << '$' << std::endl;

	for (size_t i = 0; i < er.size(); i++)
		std::cout << er[i];
	std::cout << "max request size = " << conf.getMaxRequestSize() << '$' << std::endl;
	for (std::map<std::string, Location>::iterator	loc_it = loc.begin(); loc_it != loc.end(); loc_it++)
		std::cout << "Location: " << loc_it->first + '$' + '\n' << loc_it->second;
	return (os);
}


// Fonction

bool	Conf::findError(int code, std::string *filename) const
{
	for (size_t i = 0; i < _error.size(); i++)
	{
		std::vector<int> vec = _error[i].getErrorId();
		for (size_t j = 0; j < vec.size(); j++)
		{
			if (code == vec[j])
			{
				*filename = _error[i].getPath();
				return (1);
			}
		}
	}
	return (0);
}

// Getter

std::string	Conf::getHost(void) const
{
	return (_host);
}

std::string	Conf::getPort(void) const
{
	return (_port);
}

std::vector<std::string>	Conf::getServerName(void) const
{
	return (_server_name);
}

std::vector<ErrorPath>	Conf::getError(void) const
{
	return (_error);
}

int	Conf::getMaxRequestSize(void) const
{
	return (_max_request_size);
}

std::map<std::string, Location>	&Conf::getLocation(void)
{
	return (_location);
}

const std::map<std::string, Location>	Conf::getLocation(void) const
{
	return (_location);
}


// Setter

void	Conf::setHostPort(std::string str)
{
	std::istringstream	iss(str);
	std::string			word;
	size_t				i;

	if (_init_listen)
		throw std::runtime_error("Error: [emerg] \"listen\" directive is duplicate");
	if (iss.peek() == -1)
		throw std::runtime_error("Error: [emerg] invalid number of arguments in \"listen\" directive");
	iss >> word;
	if (iss.peek() != -1)
		throw std::runtime_error("Error: [emerg] invalid number of arguments in \"listen\" directive");
	i = word.find(':');
	if (i == std::string::npos)
		throw std::invalid_argument("Error: Invalid host:port format: missing colon");
	_host = word.substr(0, i);
	_port = word.substr(i + 1);
	_init_listen = true;
	return ;
}

void	Conf::setServerName(std::string str)
{
	// if (!_server_name.empty())
	// 	throw std::runtime_error("Error: [emerg] \"server_name\" directive is duplicate");
	// _server_name = str;

	std::istringstream	iss(str);
	std::string			word;

	if (iss.peek() == -1 && !_server_name.size())
		throw std::runtime_error("Error: [emerg] invalid number of arguments in \"server_name\" directive");
	while (iss >> word)
		_server_name.push_back(word);
	return ;
}

void	Conf::setError(std::string str)
{
	_error.push_back(ErrorPath(str));
	return ;
}

void	Conf::setMaxRequestSize(std::string str)
{
	std::stringstream ss(str);

	if (_init_max_request_size)
		throw std::runtime_error("Error: [emerg] \"client_max_body_size\" directive is duplicate");
	ss >> _max_request_size;
	if (ss.fail() || !ss.eof())
		throw std::invalid_argument("Error: [emerg] \"client_max_body_size\" Invalid number format");
	else if (_max_request_size < 0)
		throw std::runtime_error("Error: [emerg] \"client_max_body_size\" directive invalid value");
	_init_max_request_size = true;
	return ;
}

void	Conf::setLocation(std::string str) // might need to be fixed
{
	size_t		i = 0;
	size_t		ln = 0;
	std::string	path, block;

	while (i < str.size() && !std::isspace(str[i]))
		i++;
	path = str.substr(0, i);
	if (path[0] != '/')
		path = '/' + path;
	if (_location.find(path) != _location.end())
		throw std::runtime_error("Error: [emerg] duplicate location \"" + path + "\"");
	i = str.find('{') + 1;
	ln = str.find('}') - i;
	block =  str.substr(i, ln);
	_location.insert(std::pair<std::string, Location>(path, block));
	return ;
}
