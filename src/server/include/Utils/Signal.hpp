#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <csignal>
#include <iostream>
#include <csignal>
#include <unistd.h>

class Signal {
	
	private:
		int pipeFd[2];
		int writePipeFd;
		int readPipeFd;

	public:
		Signal();
		~Signal();
		void init_signal(int writeFd);
		int getReadFd();
	
};

#endif