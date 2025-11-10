#include "Cgi.hpp"

// Constructor

Cgi::Cgi(HTTPRequest req, int client_fd, HTTPResponse &response) :
_resp(response)
{
	_start = -1;
	_pid = 0;
	_return_status = -1;
	_cgi_in = -1;
	_cgi_out = -1;
	_client_fd = client_fd;
	_path = req.uri;
	_body = req.body;

	_redirect_status = "REDIRECT_STATUS=200";
	_script_filename = "SCRIPT_FILENAME=" + req.uri;
	_auth_type = "AUTH_TYPE=" + req.getValueHeader("auth-type");
	_content_length = "CONTENT_LENGTH=" + req.headers["content-length"];
	_content_type = "CONTENT_TYPE=" + req.headers["content-type"];
	_gateway_interface = "GATEWAY_INTERFACE="  + req.getValueHeader("gateway-interface");
	_path_info = "PATH_INFO=" + req.path_info;
	_path_translated = "PATH_TRANSLATED=" + req.getValueHeader("path-translated");
	_query_string = "QUERY_STRING=" + req.query_string;
	_remote_addr = "REMOTE_ADDR=" + req.getValueHeader("remote-addr");
	_remote_host = "REMOTE_HOST=" + req.getValueHeader("remote-host");
	_remote_ident = "REMOTE_IDENT=" + req.getValueHeader("remote-ident");
	_remote_user = "REMOTE_USER=" + req.getValueHeader("remote-user");
	_request_method = "REQUEST_METHOD=" + req.method;
	_script_name = "SCRIPT_NAME=" + req.uri;
	_server_name = "SERVER_NAME=" + req.getValueHeader("server-name");
	_server_port = "SERVER_PORT=" + req.getValueHeader("server-port");
	_server_protocol = "SERVER_PROTOCOL=" + req.getValueHeader("server-protocol");
	_server_software = "SERVER_SOFTWARE=" + req.getValueHeader("server-software");
	_cookies = "HTTP_COOKIE=" + req.headers["cookie"];
	return ;
}


Cgi::~Cgi(void)
{
	if (_pid)
		kill(_pid, SIGTERM);
	return ;
}


// Getter
int Cgi::getFdIn()
{
	return this->_cgi_in;
}

int Cgi::getFdOut()
{
	return this->_cgi_out;
}


pid_t	Cgi::getPid(void)
{
	return (_pid);
}

char	**Cgi::getEnvp(void)
{
	char **envp;

	envp = new char*[21];
	if (envp == NULL)
		return (envp);
	envp[0] = strdup(_auth_type.c_str());
	envp[1] = strdup(_content_length.c_str());
	envp[2] = strdup(_content_type.c_str());
	envp[3] = strdup(_gateway_interface.c_str());
	envp[4] = strdup(_path_info.c_str());
	envp[5] = strdup(_path_translated.c_str());
	envp[6] = strdup(_query_string.c_str());
	envp[7] = strdup(_remote_addr.c_str());
	envp[8] = strdup(_remote_host.c_str());
	envp[9] = strdup(_remote_ident.c_str());
	envp[10] = strdup(_remote_user.c_str());
	envp[11] = strdup(_request_method.c_str());
	envp[12] = strdup(_script_name.c_str());
	envp[13] = strdup(_server_name.c_str());
	envp[14] = strdup(_server_port.c_str());
	envp[15] = strdup(_server_protocol.c_str());
	envp[16] = strdup(_server_software.c_str());
	envp[17] = strdup(_redirect_status.c_str());
	envp[18] = strdup(_script_filename.c_str());
	envp[19] = strdup(_cookies.c_str());
	envp[20] = NULL;
	return (envp);
}

char	**Cgi::getArgv(void)
{
	char **argv;

	size_t pos = _path.rfind(".");
	std::string ext = _path.substr(pos);

	argv = new char*[3];
	if (argv == NULL)
		return (argv);

	if (ext == ".py")
		argv[0] = strdup("/usr/bin/python3");
	else if (ext == ".php")
		argv[0] = strdup("/usr/bin/php-cgi");

	argv[1] = strdup(_path.c_str());
	argv[2] = NULL;
	return (argv);
}

CgiResponse	&Cgi::getCgiResponse()
{
	return _cgiResponse;
}

std::string	Cgi::getBody()
{
	return _body;
}

int	Cgi::getFdClient()
{
	return _client_fd;
}

// Setter

void	Cgi::setCgiResponse(CgiResponse &cgiResponse)
{
	this->_cgiResponse = cgiResponse;
	return ;
}

void	Cgi::setHttpResponse(HTTPResponse& response)
{
	this->_resp = response;
	return ;
}

void	Cgi::setPipe(int *fd1, int *fd2)
{
	if (!_pid)
	{
		close(fd1[1]);
		close(fd2[0]);
		dup2(fd1[0], 0); // stdin
		close(fd1[0]);
		dup2(fd2[1], 1); // stdout
		close(fd2[1]);
	}
	else
	{
		_cgi_in = fd1[1];
		close(fd1[0]);
		_cgi_out = fd2[0];
		close(fd2[1]);
	}
	return ;
}


// Function

void	Cgi::freeTab(char **tab, size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		if (tab[i])
			free(tab[i]);
	}
	if (tab)
		delete[] tab;
	return ;
}

bool	Cgi::execCgi(void)
{
	if (_start < 0)
	{
		int	fd1[2];
		int	fd2[2];

		if (pipe(fd1))
		{
			exit(1);
		}
		if (pipe(fd2))
		{
			close(fd1[0]);
			close(fd1[1]);
			exit(1);
		}

		fcntl(fd1[0], F_SETFD, FD_CLOEXEC);
		fcntl(fd1[1], F_SETFD, FD_CLOEXEC);
		fcntl(fd2[0], F_SETFD, FD_CLOEXEC);
		fcntl(fd2[1], F_SETFD, FD_CLOEXEC);

		_start = time(0);
		_pid = fork();
		setPipe(fd1, fd2);
		if (!_pid)
		{
			char	**envp = getEnvp();
			char	**argv = getArgv();

			execve(argv[0], argv, envp);
			freeTab(envp, 21);
			freeTab(argv, 3);
			exit(1);
		}
		return (1);
	}
	return (0);
}

int	Cgi::healtCheck(void)
{
	pid_t	result;
	int		status;

	result = waitpid(_pid, &status, WNOHANG);
	if (!result)
	{
		time_t current_time = time(0);
		if (current_time - _start > TIMEOUT)
		{
			kill(_pid, SIGTERM);
			return (-1);
		}
		else
			return (1);
	}
	else if (result > 0)
		_return_status = WEXITSTATUS(status);
	else
		return (1);
	return (0);
}
