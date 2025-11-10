#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

#include <iostream>
#include <map>

struct CgiResponse {

		std::string _status;
		std::string _location;
		std::string _content_type;
		std::string _body;
		std::map<std::string, std::string> _general_headers;
		bool _complete_header;
		bool _complete;
		std::string _buffer;

		CgiResponse();

		CgiResponse(const CgiResponse& other);

		CgiResponse &operator=(const CgiResponse &other);

		~CgiResponse();
};

#endif
