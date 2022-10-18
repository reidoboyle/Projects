#pragma once
#include "pch.h"
class Router
{
public:
	int seq;
	std::string host;
	std::string ip;
	double time;
	bool failed = false;
	bool noDNS = false;
	bool echo = false;
	u_long intIP;
	int attempt;
	clock_t start_c;
	bool recieved = false;
	double timeout = 50;

	Router(int s);

	void handleResponse();
	void Print();
};

