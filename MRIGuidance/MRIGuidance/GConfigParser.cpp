#include "GConfigParser.h"
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace std;

GConfigParser::GConfigParser(string filepath):m_sConfigurationFilepath(filepath)
{}

void GConfigParser::parseConfiguration()
{
	// REQUIRES:
	//		m_sConfigurationFilePath has been defined to a valid XML filepath
	// EFFECTS:
	//		Parses through m_sConfigurationFilePath to set variables to be used for
	//		registration and tracking.

	cv::FileStorage fs;
	fs.open(m_sConfigurationFilepath, cv::FileStorage::READ); 

	if (!fs.isOpened()) {
		cout << "[Fatal error] Failed to open " << m_sConfigurationFilepath << endl;
		exit(1);
	}

	// Extract values from nodes of m_sConfigurationFilepath to define variables
	fs["ImagesLocation"] >> m_sImagesLocation;
	fs["NumberOfFiducials"] >> m_nNumberOfFiducials;
	fs["RegistrationMode"] >> m_nRegistrationMode;
	fs["Metric"] >> m_bMetric;
	fs["Rate"] >> m_nRate;

	fs.release();
}