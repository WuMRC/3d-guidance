#ifndef GCONFIGPARSER
#define GCONFIGPARSER

#include <string>

class GConfigParser
{
public:
	// Classic Constructor
	GConfigParser();

	// Custom Constructor to set file path of XML configuration file
	GConfigParser(std::string filepath);

	// Public Member Functions
	// Parses through XML configuration file to set variables for tracking
	void parseConfiguration();
	std::string getImagesLocation() const {return m_sImagesLocation;};
	int getNumberOfFiducials() const {return m_nNumberOfFiducials;};
	int getRegistrationMode() const {return m_nRegistrationMode;};
	bool getMetricMode() const {return m_bMetric;};
	int getRate() const {return m_nRate;};

	// Classic Destructor
	~GConfigParser(void){};
private:
	// File path to XML configuration file from which values will be read
	std::string m_sConfigurationFilepath;

	// Folder path containing original images
	std::string m_sImagesLocation;

	// Number of fiducial markers to be registered
	int m_nNumberOfFiducials;

	// Mode used for registering fiducial markers
	int m_nRegistrationMode;

	// Magnetic Tracker Values
	bool m_bMetric;
	int m_nRate;
};

#endif