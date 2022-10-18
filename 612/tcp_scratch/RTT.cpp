#include "pch.h"
#include "RTT.h"

// default constructor
RTT::RTT()
{
	alpha = 0.125;
	beta = 0.25;
	sampleRTT = 1.0;
	estimatedRTT = 1.0;
	devRTT = 0.0;
	RTO = 1.0;
}
// custom constructor
RTT::RTT(double rtt)
{
	alpha = 0.125;
	beta = 0.25;
	sampleRTT = 1.0;
	estimatedRTT = 1.0;
	devRTT = 0.0;
	RTO = 1.0;

}

void RTT::setSampleRTT(double rtt)
{
	sampleRTT = rtt;
	estimatedRTT = ((1 - alpha) * estimatedRTT) + (alpha * sampleRTT);
	devRTT = ((1 - beta) * devRTT) + (beta * abs(sampleRTT - estimatedRTT));
	RTO = estimatedRTT + max(4 * devRTT, 0.010);
}
double RTT::getRTO()
{
	return RTO;
}
double RTT::getEstimatedRTT()
{
	return estimatedRTT;
}
void RTT::setEstimatedRTT(double rtt)
{
	estimatedRTT = rtt;
}
void RTT::doubleRTO()
{
	RTO = RTO * 2;
}
/*


void RTT::setDevRTT(double rtt)
{
	devRTT = (1 - beta) * devRTT + beta * abs(sampleRTT - estimatedRTT);
}

double RTT::getSampleRTT()
{
	return sampleRTT;
}

double RTT::getEstimatedRTT()
{
	return estimatedRTT;
}

double RTT::getDevRTT()
{
	return devRTT;
}

double RTT::getRTO()
{
	return estimatedRTT+ max(4*devRTT, 0.010);
}
*/
