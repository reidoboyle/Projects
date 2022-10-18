#include "pch.h"
#include "Packet.h"

char* Packet::getPkt()
{
	return pkt;
}

Packet::Packet(char* pt,int sz,int tp,int s)
{
	type = tp;
	size = sz;
	seq = s;
	memcpy(pkt, pt, MAX_PACKET_SIZE);
}

clock_t Packet::getTime()
{
	return txTime;
}

int Packet::getSize()
{
	return size;
}

void Packet::setTime()
{
	txTime = clock();
}

void Packet::incrementRTX()
{
	n_rtx++;
}

int Packet::getRTX()
{
	return n_rtx;
}

bool Packet::hasBeenRTX()
{
	return isRTX;
}

void Packet::setHasRTX()
{
	isRTX = true;
}
