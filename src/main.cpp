#include "Server.hpp"
#include "Signal.hpp"
#include "ServerConf.hpp"

int main(int argc, char * argv[])
{
	ServerConf	* conf = NULL;
	try
	{
		conf = new ServerConf(argc, argv);
		Signal signal;
		Server *server = new Server(signal.getReadFd(), conf->getConfList());

		server->run();
		delete server;
		delete conf;

	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}
