#include "CGIUtils.hpp"
#include <sys/epoll.h>

int isCgi(std::string &uri, std::string &path_info, std::map<std::string, Location>::iterator &it, bool& cgi)
{
	// --> GET "/cgi-bin/upload.py/foo/bar?name=ruben&lang=fr"
	(void)path_info;

	std::vector<std::string> cgis = it->second.getCgi();

	if (!cgis.size())
		return (0);
	size_t len_cgis = cgis.size();
	size_t start = uri.size();
	struct stat sb;
	std::string tmp;

	while (start)
	{
		tmp  = uri.substr(0, start);
		if (stat(tmp.c_str(), &sb) == 0)
		{
			path_info = uri.substr(tmp.size(), uri.size() - tmp.size());
			if (S_ISDIR(sb.st_mode))
			{
				if (it->second.getIndex().size())
					tmp += "/" + it->second.getIndex();
				else
					return (0);
			}
			for (size_t i = 0; i < len_cgis; i++)
			{
				size_t ext = tmp.rfind(cgis[i]);
				if (ext != std::string::npos && (tmp.size() - ext) == cgis[i].size())
				{
					uri = tmp;
					if (access(uri.c_str(), X_OK) == -1)
						cgi = false;
					else
						cgi = true;
					return (1);
				}
			}
			return (0);
		}
		start--;
		if (start > 0)
			start = uri.rfind("/", start);
		else
			break;
	}
	return (0);
}