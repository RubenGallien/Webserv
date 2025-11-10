/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvicino <lvicino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 15:28:28 by lvicino           #+#    #+#             */
/*   Updated: 2025/11/04 15:00:37 by lvicino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_HPP
# define CONF_HPP

# include "ErrorPath.hpp"
# include "Location.hpp"
#include <string>
# include <map>

class	Conf
{
	private:
		Conf(void);

		std::string						_host;	//localhost:90 localhost
		std::string						_port;	//90
		// std::string						_server_name;
		std::vector<std::string>		_server_name;
		std::vector<ErrorPath>			_error;
		int								_max_request_size;
		std::map<std::string, Location>	_location;

		bool	_init_listen;
		bool	_init_max_request_size;

		void	setHostPort(std::string str);
		void	setServerName(std::string str);
		void	setError(std::string str);
		void	setMaxRequestSize(std::string str);
		void	setLocation(std::string str);

	public:
		Conf(const std::string &conf);
		Conf(const Conf &other);

		~Conf(void);

		Conf	&operator=(const Conf &other);

		std::string								getHost(void) const;
		std::string								getPort(void) const;
		std::vector<std::string>				getServerName(void) const;
		std::vector<ErrorPath>					getError(void) const;
		int										getMaxRequestSize(void) const;
		std::map<std::string, Location>			&getLocation(void);
		const std::map<std::string, Location>	getLocation(void) const;

		bool	findError(int code, std::string *filename) const;
};

std::ostream	&operator<<(std::ostream &os, const Conf &conf);

#endif
