#include "Upload.hpp"

int multipartUpload(std::string upload, std::string &body, std::string &location, std::string & content_type)
{
	(void)location;
	struct stat sb;
	if (stat(upload.c_str(), &sb) == -1)
		return (404);
	if (S_ISREG(sb.st_mode))
		return (405);

	size_t boundStart = content_type.find("boundary=");
	if (boundStart == std::string::npos)
		return 400;
	boundStart += 10;
	size_t boundEnd = content_type.find("\"", boundStart);
	std::string boundary = content_type.substr(boundStart, boundEnd - boundStart);
	std::string file;
	size_t filenameStart = body.find("filename=\"");
	size_t filenameEnd = 0;
	if (filenameStart != std::string::npos)
	{
		filenameStart += 10;
		filenameEnd = body.find("\"", filenameStart);
		file = body.substr(filenameStart, filenameEnd - filenameStart);
	}
	size_t contentStart = body.find("\r\n\r\n", filenameEnd);
	contentStart += 4;
	size_t contentEnd = body.find(boundary, contentStart);

	std::string fileContent = body.substr(contentStart, contentEnd - contentStart);
	std::string upload_path = upload + "/";
	upload_path += file;
	std::ofstream outfile(upload_path.c_str());
	outfile.write(fileContent.c_str(), fileContent.size());
	outfile.close();
	return 204;
}

int classicUpload(std::string upload, std::string &body, std::string &location)
{
	(void)location;
	struct stat sb;
	if (stat(upload.c_str(), &sb) == -1)
		return (404);
	if (S_ISREG(sb.st_mode))
		return (405);

	struct dirent *file;
	DIR *directory;
	directory = opendir(upload.c_str());
	std::string upload_path = upload + "/";
	int number = 0;
	std::string final_number;
	while ((file = readdir(directory)) != NULL)
	{
		std::string tmp(file->d_name);
		size_t up = tmp.find("upload_");
		if (up != std::string::npos)
		{
			size_t ext = tmp.find(".txt");
			std::string number_tmp;
			if (ext != std::string::npos && ext > up)
			{
				for (size_t i = 7; i < ext; i++)
					number_tmp.push_back(tmp[i]);
				int current;
				std::istringstream(number_tmp) >> current;
				if (current > number)
				{
					final_number = number_tmp;
					number = current;
				}
			}
		}
	}
	closedir(directory);
	number++;
	std::stringstream ss;
	ss << number;
	upload_path += "upload_";
	upload_path += ss.str();
	upload_path += ".txt";
	std::ofstream outfile(upload_path.c_str());

	if (outfile.is_open())
	{
		outfile << body << std::endl;
		outfile.close();
	}
	else
		return (403);
	return 204;
}

