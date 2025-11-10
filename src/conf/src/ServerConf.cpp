/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvicino <lvicino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 14:44:11 by lvicino           #+#    #+#             */
/*   Updated: 2025/09/05 17:20:02 by lvicino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConf.hpp"


// Constructor

ServerConf::ServerConf(void)
{
	return ;
}

ServerConf::ServerConf(int argc, char **argv)
{
	if (argc != 2)
		throw ArgException();

	std::string	content = getFileContent(argv[1]);
	size_t		size = content.size();
	size_t		ln;

	for (size_t i = 0; i < size; i++)
	{
		if (content.find("server", i) == i)
		{
			i += 6;
			while (i < size && std::isspace(content[i]))
				i++;
			if (i < size && content[i++] == '{')
			{
				ln = findMatchingBrace(content, i) - i;
				_conf_list.push_back(Conf(content.substr(i, ln)));
				i += ln;
			}
			else
				throw std::runtime_error("Error: Expected '{' after 'server'");
		}
		else if (!std::isspace(content[i]))
			throw std::runtime_error("Error: Invalid character or missing 'server'");
	}
	if (_conf_list.empty())
		throw std::runtime_error("Error: Empty configuration file");
	return ;
}

ServerConf::ServerConf(const ServerConf &other)
{
	*this = other;
	return ;
}

ServerConf::~ServerConf(void)
{
	return ;
}


// Operator

ServerConf	&ServerConf::operator=(const ServerConf &other)
{
	_conf_list = other._conf_list;
	return (*this);
}

std::ostream	&operator<<(std::ostream &os, const ServerConf &serv)
{
	std::vector<Conf>	conf = serv.getConfList();

	for (size_t i = 0; i < conf.size(); i++)
		std::cout << "Server[" << i << "]\n" << conf[i] << std::endl;
	return (os);
}


// Fonction

std::string	ServerConf::getFileContent(char *file_name)
{
	std::ifstream	conf_file(file_name, std::ifstream::in);
	std::string		buffer, content;
	size_t			pos;
	struct stat		sb;

	if (!stat(file_name, &sb))
	{
		if (S_ISDIR(sb.st_mode))
			throw std::runtime_error("Error: \"" + std::string(file_name) + "\" is a directory");
	}
	else if (!conf_file)
		throw CantOpenFileException(file_name);
	while (getline(conf_file, buffer))
	{
		pos = buffer.find('#');
		if (pos == std::string::npos)
			pos = buffer.length();
		while (pos > 0 && std::isspace(buffer[pos - 1]))
			--pos;
		if (pos != std::string::npos)
			buffer.erase(pos);
		if (!buffer.empty())
			content += buffer + "\n";
	}
	return (content);
}

size_t	ServerConf::findMatchingBrace(const std::string &str, size_t i)
{
	size_t	size = str.size();
	size_t	n = 1;

	while (n && i++ < size)
	{
		if (str[i] == '{')
			n++;
		else if (str[i] == '}')
			n--;
	}
	if (!n && i < size && str[i] == '}')
		return (i);
	else
		throw std::runtime_error("Error: Unbalanced or missing closing brace");
}

std::vector<Conf>	ServerConf::getConfList(void) const
{
	return (_conf_list);
}


// Exception

const char	*ServerConf::ArgException::what() const throw()
{
	return("Error: webserv needs one argument");
}

const char	*ServerConf::CantOpenFileException::what() const throw()
{
	return(_error.c_str());
}
