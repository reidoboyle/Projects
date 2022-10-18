#include "pch.h"
#include "Router.h"

Router::Router(int s)
{
	seq = s+1;
	attempt = 1;
}

void Router::handleResponse()
{
	time = (double(start_c - clock()) / CLOCKS_PER_SEC) * 1000;
}

void Router::Print()
{
	std::cout << seq << "\t" << host << " (" << ip << ") ";
	printf("%.3f ms (%d)\n", time, attempt);
}
