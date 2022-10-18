#pragma once
#define MAX_PACKET_SIZE (1500-28)
#include <ctime>

class Packet
{
public:
	int type;	// 1=SYN,2 = data, 3 = FINE
	int size;	// bytes
	clock_t txTime;
	char pkt[MAX_PACKET_SIZE];
	int n_rtx = 0;
	bool isRTX = false;
	int seq;

	char* getPkt();
	Packet(char* pt,int sz,int tp,int s);
	clock_t getTime();
	int getSize();
	void setTime();
	void incrementRTX();
	int getRTX();
	bool hasBeenRTX();
	void setHasRTX();
};

