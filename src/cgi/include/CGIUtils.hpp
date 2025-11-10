#ifndef CGIUTILS_HPP
#define CGIUTILS_HPP

#include <string>
#include "Location.hpp"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <Cgi.hpp>

int isCgi(std::string &uri, std::string &path_info, std::map<std::string, Location>::iterator &it, bool& cgi);

#endif


