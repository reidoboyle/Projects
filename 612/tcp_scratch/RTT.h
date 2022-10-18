#pragma once

#ifndef RTT_HPP
#define RTT_HPP
class RTT
{
private:
	double devRTT;
	double sampleRTT;
	double estimatedRTT;
	double beta;
	double alpha;
	double RTO;

public:
	RTT();
	RTT(double rtt);
	void setSampleRTT(double rtt);
	double getRTO();
	double getEstimatedRTT();
	void setEstimatedRTT(double rtt);
	void doubleRTO();
	/*void setDevRTT(double rtt);
	double getSampleRTT();

	double getDevRTT();
	double getRTO();*/
};
#endif
