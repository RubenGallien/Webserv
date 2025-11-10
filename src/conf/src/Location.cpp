/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvicino <lvicino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 17:30:53 by lvicino           #+#    #+#             */
/*   Updated: 2025/11/06 12:50:54 by lvicino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"


// Constructor

Location::Location(void)
{
	return ;
}

Location::Location(const std::string &str) :
_status_code(0), _autoindex(false) //needs to recieve content of lacation block
{
	std::map<std::string, void (Location::*)(std::string)>	directive;

	directive["root"] = &Location::setRoot;
	directive["index"] = &Location::setIndex;
	directive["return"] = &Location::setRedirection;
	directive["autoindex"] = &Location::setAutoIndex;
	directive["cgi"] = &Location::setCgi;
	directive["upload"] = &Location::setUpload;
	directive["method"] = &Location::setMethod;

	size_t	size = str.size();
	size_t	j = 0;

	_init_root = false;
	_init_index = false;
	_init_redirection = false;
	_init_autoindex = false;
	_init_cgi = false;
	_init_upload = false;
	_init_method = false;

	for (size_t i = 0; i < size; )
	{
		bool	directive_found = false;

		for (std::map<std::string, void (Location::*)(std::string)>::iterator it = directive.begin(); it != directive.end(); it++)
		{
			while (i < size && std::isspace(str[i]))
				i++;
			if (i >= size)
				break;
			j = 0;
			if (str.find(it->first, i) == i && std::isspace(str[i + it->first.size()]))
			{
				directive_found = true;
				i += it->first.size();
				while (i < size && std::isspace(str[i]))
					i++;
				if (i >= size)
					throw std::runtime_error("Error: Expected value after '" + it->first + "'");
				while (i + j < size && str[i + j] != ';')
				{
					if (str[i + j] == '{')
						throw std::runtime_error("Error: Unexpected '{' after '" + it->first + '\'');
					j++;
				}
				if (i + j == size)
					throw std::runtime_error("Error: expected ';' after '" + it->first + '\'');
				(this->*it->second)(str.substr(i, j));
				i += j + 1;
				break;
			}
		}
		if (i < size && !directive_found)
		{
			j = 0;
			while (i + j < size && !std::isspace(str[i + j]) && str[i + j] != ';')
				j++;
			throw std::runtime_error("Unknown configuration directive: '" + str.substr(i, j) + "'");
		}
	}
	return ;
}

Location::Location(const Location &other)
{
	*this = other;
	return ;
}

Location::~Location(void)
{
	return ;
}


// Operator

Location	&Location::operator=(const Location &other)
{
	_root = other._root;
	_index = other._index;
	_redirection = other._redirection;
	_status_code = other._status_code;
	_autoindex = other._autoindex;
	_cgi = other._cgi;
	_upload = other._upload;
	_method = other._method;

	_init_root = other._init_root;
	_init_index = other._init_index;
	_init_redirection = other._init_redirection;
	_init_autoindex = other._init_autoindex;
	_init_cgi = other._init_cgi;
	_init_upload = other._init_upload;
	_init_method = other._init_method;
	return (*this);
}

std::ostream	&operator<<(std::ostream &os, const Location &loc)
{
	std::vector<std::string>	cgi = loc.getCgi();
	std::vector<std::string>	method = loc.getMethod();


	if (loc.getAutoIndex())
		std::cout << "Auto index = on" << std::endl;
	else
		std::cout << "Auto index = off" << std::endl;

	for (size_t i = 0; i < cgi.size(); i++)
		std::cout << "cgi = " << cgi[i] << '$' << std::endl;

	std::cout << "Upload = " << loc.getUpload() << "$" << std::endl;

	for (size_t i = 0; i < method.size(); i++)
		std::cout << "method = " << method[i] << '$' << std::endl;

	return (os);
}


// Fonction

// Getter

std::string	Location::getRoot(void) const
{
	return (_root);
}

std::string	Location::getIndex(void) const
{
	return (_index);
}

// std::vector<std::string>	Location::getIndex(void) const
// {
// 	return (_index);
// }

std::string	Location::getRedirection(void) const
{
	return (_redirection);
}

int	Location::getStatusCode(void) const
{
	return (_status_code);
}

bool		Location::getAutoIndex(void) const
{
	return (_autoindex);
}

std::vector<std::string>	Location::getCgi(void) const
{
	return (_cgi);
}

std::string	Location::getUpload(void) const
{
	return (_upload);
}

std::vector<std::string>	Location::getMethod(void) const
{
	return (_method);
}


// Setter

void	Location::setRoot(std::string str)
{
	std::istringstream	iss(str);
	std::string			word;

	if (_init_root)
		throw std::runtime_error("Error: [emerg] \"root\" directive is duplicate");
	iss >> word;
	if (iss.peek() != -1)
		throw std::runtime_error("Error: [emerg] invalid number of arguments in \"root\" directive");
	_root = word;
	_init_root = true;
	return ;
}

void	Location::setIndex(std::string str)
{
	std::istringstream	iss(str);
	std::string			word;

	if (_init_index)
		throw std::runtime_error("Error: [emerg] \"index\" directive is duplicate");
	iss >> word;
	if (iss.peek() != -1)
		throw std::runtime_error("Error: [emerg] invalid number of arguments in \"index\" directive");
	_index = word;
	_init_index = true;
	return ;
}

void	Location::setRedirection(std::string str)
{
	std::istringstream	iss(str);
	std::string			word;
	char				*ptr;
	int					n;
	std::vector<int>	vec;

	if (_init_redirection)
		throw std::runtime_error("Error: [emerg] \"return\" directive is duplicate");
	if (iss.peek() == -1)
		throw std::runtime_error("Error: [emerg] invalid number of arguments in \"return\" directive");
	vec.push_back(100);
	vec.push_back(101);
	vec.push_back(200);
	vec.push_back(201);
	vec.push_back(202);
	vec.push_back(203);
	vec.push_back(204);
	vec.push_back(205);
	vec.push_back(300);
	vec.push_back(301);
	vec.push_back(302);
	vec.push_back(303);
	vec.push_back(305);
	vec.push_back(306);
	vec.push_back(307);
	vec.push_back(400);
	vec.push_back(402);
	vec.push_back(403);
	vec.push_back(404);
	vec.push_back(405);
	vec.push_back(406);
	vec.push_back(408);
	vec.push_back(409);
	vec.push_back(410);
	vec.push_back(411);
	vec.push_back(413);
	vec.push_back(414);
	vec.push_back(415);
	vec.push_back(417);
	vec.push_back(426);
	vec.push_back(500);
	vec.push_back(501);
	vec.push_back(502);
	vec.push_back(503);
	vec.push_back(504);
	vec.push_back(505);
	iss >> word;
	n = std::strtol(word.c_str(), &ptr, 10);
	if (iss.peek() != -1)
	{
		if (*ptr || 100 > n || n > 505)
			throw std::runtime_error("Error: [emerg] invalid return code \"" + word + "\"");
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (n == vec[i])
				break ;
			else if (i + 1 == vec.size())
				throw std::runtime_error("Error: [emerg] invalid return code \"" + word + "\"");
		}
		_status_code = n;
		iss >> word;
		_redirection = word;
		if (_redirection.size() && _redirection[0] == '"')
		{

			if (str[str.size() - 1] != '"')
				throw std::runtime_error("Error: [emerg] bad quotes in \"return\" directive");
			while (_redirection[_redirection.size() - 1] != '"')
			{
				iss >> word;
				_redirection += " ";
				_redirection += word;
			}
			if (_redirection[0] && _redirection[_redirection.size() - 1])
			{
				_redirection.erase(0, 1);
				_redirection.erase(_redirection.size() - 1, 1);
			}
		}
		if (iss.peek() != -1)
			throw std::runtime_error("Error: [emerg] invalid number of arguments in \"return\" directive");
	}
	else
	{
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (!*ptr && (n == 301 || n == 302 || n == 303 || n == 307))
				throw std::runtime_error("Error: [emerg] invalid return code \"" + word + "\"");
			else if (n == vec[i])
			{
				_status_code = n;
				break ;
			}
			else if (i + 1 == vec.size())
			{
				_status_code = 302;
				_redirection = word;
			}
		}
	}
	if (_redirection.find("\"") != std::string::npos)
	{
		int	n = 0;
		for (size_t i = 0; i < _redirection.size(); i++)
		{
			if (_redirection[i] == '\"' && (i && i != _redirection.size() - 1))
				throw std::runtime_error("Error: [emerg] bad quotes in \"return\" directive");
			else if (_redirection[i] == '\"')
				n++;
		}
		if (n != 2)
			throw std::runtime_error("Error: [emerg] bad quotes in \"return\" directive");
	}
	_init_redirection = true;
	return ;
}

void	Location::setAutoIndex(std::string str)
{
	std::istringstream	iss(str);
	std::string			word;

	if (_init_autoindex)
		throw std::runtime_error("Error: [emerg] \"autoindex\" directive is duplicate");
	if (iss.peek() == -1)
		throw std::runtime_error("Error: [emerg] invalid number of arguments in \"autoindex\" directive");
	iss >> word;
	if (iss.peek() != -1)
		throw std::runtime_error("Error: [emerg] invalid number of arguments in \"autoindex\" directive");
	if (word == "on")
		_autoindex = true;
	else if (word != "off")
		throw std::runtime_error("Error: [emerg] invalid value \"" + str + "\" in \"autoindex\" directive, it must be \"on\" or \"off\"");
	_init_autoindex = true;
	return ;
}

void	Location::setCgi(std::string str)
{
	std::istringstream	iss(str);
	std::string			word;

	if (_init_cgi)
		throw std::runtime_error("Error: [emerg] \"cgi\" directive is duplicate");
	if (iss.peek() == -1)
		throw std::runtime_error("Error: [emerg] invalid number of arguments in \"cgi\" directive");
	while (iss >> word)
	{
		if (word == ".php" || word == ".py")
			_cgi.push_back(word);
		else
			throw std::runtime_error("Error: [emerg] invalid value \"" + word + "\" in \"cgi\" directive, it must be \".php\" or \".py\"");
	}
	_init_cgi = true;
	return ;
}

void	Location::setUpload(std::string str)
{
	std::istringstream	iss(str);
	std::string			word;

	if (_init_upload)
		throw std::runtime_error("Error: [emerg] \"upload\" directive is duplicate");
	if (iss.peek() == -1)
		throw std::runtime_error("Error: [emerg] invalid number of arguments in \"upload\" directive");
	iss >> word;
	if (iss.peek() != -1)
		throw std::runtime_error("Error: [emerg] invalid number of arguments in \"upload\" directive");
	_upload = word;
	_init_upload = true;
	return ;
}

void	Location::setMethod(std::string str)
{
	std::istringstream	iss(str);
	std::string			word;

	if (_init_method)
		throw std::runtime_error("Error: [emerg] \"method\" directive is duplicate");
	if (iss.peek() == -1)
		throw std::runtime_error("Error: [emerg] invalid number of arguments in \"method\" directive");
	while (iss >> word)
	{
		if (word == "GET" || word == "POST" || word == "DELETE")
			_method.push_back(word);
		else
			throw std::runtime_error("Error: [emerg] invalid value \"" + word + "\" in \"method\" directive, it must be \"GET\", \"POST\" or \"DELETE\"");
	}
	_init_method = true;
	return ;
}
