#include "CgiResponse.hpp"

CgiResponse::CgiResponse()
{
	_complete_header = false;
	_complete = false;
	return;
}

CgiResponse::CgiResponse(const CgiResponse& other)
{
	*this = other;
	return;
}

CgiResponse &CgiResponse::operator=(const CgiResponse &other)
{
	_status = other._status;
	_location = other._location;
	_content_type = other._content_type;
	_body = other._body;
	_complete_header = other._complete_header;
	_complete = other._complete;
	_general_headers = other._general_headers;
	_buffer = other._buffer;
	return *this;
}

CgiResponse::~CgiResponse()
{
	return;
}
