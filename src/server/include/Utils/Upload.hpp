#ifndef UPLOAD_HPP
#define UPLOAD_HPP

#include <iostream>
#include <string>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

int classicUpload(std::string upload, std::string &body, std::string &location);
int multipartUpload(std::string upload, std::string &body, std::string &location, std::string& content_type);

#endif
