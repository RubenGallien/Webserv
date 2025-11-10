/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvicino <lvicino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 13:48:10 by lvicino           #+#    #+#             */
/*   Updated: 2025/10/22 13:17:47 by lvicino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONF_HPP
# define SERVERCONF_HPP

# include "Conf.hpp"

# include <cstring>
# include <fstream>
# include <sys/stat.h>

class	ServerConf
{
	private:
		ServerConf(void);

		std::vector<Conf>	_conf_list;

		std::string	getFileContent(char *file_name);
		size_t		findMatchingBrace(const std::string &str, size_t start);

	public:
		ServerConf(int argc, char **argv);
		ServerConf(const ServerConf &other);

		~ServerConf(void);

		ServerConf	&operator=(const ServerConf &other);

		std::vector<Conf>	getConfList(void) const;

		class	ArgException : public std::exception
		{
			const char	*what() const throw();
		};

		class	CantOpenFileException : public std::exception
		{
			std::string _error;

			const char	*what() const throw();

			public:
				virtual ~CantOpenFileException() throw() {}

				CantOpenFileException(const std::string &file_name) :
				_error(file_name + ": " + strerror(errno)) {}
		};
};

std::ostream	&operator<<(std::ostream &os, const ServerConf &serv);

#endif
