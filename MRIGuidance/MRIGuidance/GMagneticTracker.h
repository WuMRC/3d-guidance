#ifndef GMAGNETICTRACKER
#define GMAGNETICTRACKER

#include "ATC3DG.h"
#include <opencv2/core/core.hpp>

class CSystem
{
public:
	SYSTEM_CONFIGURATION	m_config;
}; 

class CSensor
{
public:
	SENSOR_CONFIGURATION	m_config;
};

class CXmtr
{
public:
	TRANSMITTER_CONFIGURATION	m_config;
};

class GMagneticTracker{
public:
	GMagneticTracker();
	GMagneticTracker(bool metricMode, double rate);

	cv::Mat stabilityReading(int numSeconds);

	~GMagneticTracker();

private:
	bool m_bMetric;
	double m_fRate;
	CSystem m_System;
	CSensor *m_pSensor;
	CXmtr *m_pXmtr;

	void errorHandler(int error);
};

#endif