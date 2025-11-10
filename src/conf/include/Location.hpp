/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvicino <lvicino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 17:26:56 by lvicino           #+#    #+#             */
/*   Updated: 2025/11/04 17:27:05 by lvicino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <iostream>
# include <cstdlib>
# include <sstream>
# include <vector>
# include <map>

class	Location
{
	private:
		Location(void);

		std::string					_root;
		std::string					_index;
		std::string					_redirection;
		int							_status_code;
		bool						_autoindex;
		std::vector<std::string>	_cgi;
		std::string					_upload;
		std::vector<std::string>	_method;

		bool	_init_root;
		bool	_init_index;
		bool	_init_redirection;
		bool	_init_autoindex;
		bool	_init_cgi;
		bool	_init_upload;
		bool	_init_method;

		void	setRoot(std::string str);
		void	setIndex(std::string str);
		void	setRedirection(std::string str);
		void	setAutoIndex(std::string str);
		void	setCgi(std::string str);
		void	setUpload(std::string str);
		void	setMethod(std::string str);

	public:
		Location(const std::string &str);
		Location(const Location &other);

		~Location(void);

		Location	&operator=(const Location &other);

		std::string					getRoot(void) const;
		std::string					getIndex(void) const;
		std::string					getRedirection(void) const;
		int							getStatusCode(void) const;
		bool						getAutoIndex(void) const;
		std::vector<std::string>	getCgi(void) const;
		std::string					getUpload(void) const;
		std::vector<std::string>	getMethod(void) const;
};

std::ostream	&operator<<(std::ostream &os, const Location &loc);

#endif
