#include "Signal.hpp"

int g_writePipeFd;

Signal::Signal()
{
	pipe(this->pipeFd);
	this->readPipeFd = this->pipeFd[0];
	this->writePipeFd = this->pipeFd[1];
	init_signal(this->writePipeFd);
}


void signal_handler(int signal)
{
	if (signal == 2)
	{
		write(g_writePipeFd, "2", 1);
	}
}

void Signal::init_signal(int writeFd)
{
	g_writePipeFd = writeFd;
	std::signal(SIGINT, signal_handler);
}

int Signal::getReadFd()
{
	return this->readPipeFd;
}

Signal::~Signal()
{
	close(this->writePipeFd);
	close(this->readPipeFd);
}