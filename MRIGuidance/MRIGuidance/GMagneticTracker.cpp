#include "GMagneticTracker.h"
#include "StdAfx.h"

extern "C" void exit( int);

using namespace std;

GMagneticTracker::GMagneticTracker(bool metric, double rate): m_bMetric(metric), m_fRate(rate)
{
	int errorCode;
	int i;
	int sensorID;
	short id;
	int records = 20000;

	//////////////////////////////////////////////////////////////

	// Initialize the PCIBIRD driver and DLL
	//
	// It is always necessary to first initialize the PCIBird "system". By
	// "system" we mean the set of PCIBird cards installed in the PC. All cards
	// will be initialized by a single call to InitializeBIRDSystem(). This
	// call will first invoke a hardware reset of each board. If at any time 
	// during operation of the system an unrecoverable error occurs then the 
	// first course of action should be to attempt to Recall InitializeBIRDSystem()
	// if this doesn't restore normal operating conditions there is probably a
	// permanent failure - contact tech support.
	// A call to InitializeBIRDSystem() does not return any information.
	//
	printf("Initializing ATC3DG system...\n");
	errorCode = InitializeBIRDSystem();
	if(errorCode!=BIRD_ERROR_SUCCESS) 
	{
		errorHandler(errorCode);
		exit(1);
	}

	//////////////////////////////////////////////////////////////
	// GET SYSTEM, SENSOR, AND TRANSMITTER CONFIGURATION
	//-----------------------------------------------------------
	// Pull info from the ATC3DG system, transmitter, and sensor.
	// The values obtained give information on anything from measurement
	// rate to number of sensors attached to the system.
	//-----------------------------------------------------------
	//
	// Get System Configuration
	//
	// In order to get information about the system we have to make a call to
	// GetBIRDSystemConfiguration(). This call will fill a fixed size structure
	// containing amongst other things the number of boards detected and the
	// number of sensors and transmitters the system can support (Note: This
	// does not mean that all sensors and transmitters that can be supported
	// are physically attached)
	//
	errorCode = GetBIRDSystemConfiguration(&m_System.m_config);
	if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);

	// Get Sensor Configuration
	//
	// Having determined how many sensors can be supported we can dynamically
	// allocate storage for the information about each sensor.
	// This information is acquired through a call to GetSensorConfiguration()
	// This call will fill a fixed size structure containing amongst other things
	// a status which indicates whether a physical sensor is attached to this
	// sensor port or not.
	//
	m_pSensor = new CSensor[m_System.m_config.numberSensors];
	for(i=0;i<m_System.m_config.numberSensors;i++)
	{
		errorCode = GetSensorConfiguration(i, &(m_pSensor+i)->m_config);
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
	}

	// Get Transmitter Configuration
	//
	// The call to GetTransmitterConfiguration() performs a similar task to the 
	// GetSensorConfiguration() call. It also returns a status in the filled
	// structure which indicates whether a transmitter is attached to this
	// port or not. In a single transmitter system it is only necessary to 
	// find where that transmitter is in order to turn it on and use it.
	//
	m_pXmtr = new CXmtr[m_System.m_config.numberTransmitters];
	for(i=0;i<m_System.m_config.numberTransmitters;i++)
	{
		errorCode = GetTransmitterConfiguration(i, &(m_pXmtr+i)->m_config);
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
	}
	//////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////
	// SET SYSTEM PARAMETERS
	//-----------------------------------------------------------
	// Unlike setting parameters for the transmitter and 
	// sensors individually, setting system parameters changes the 
	// state of the variable for the entire system
	//-----------------------------------------------------------

	// Set system measurement rate
	errorCode = SetSystemParameter(MEASUREMENT_RATE, &rate, sizeof(rate));
	if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);

	// Set system measurement type
	errorCode = SetSystemParameter(METRIC, &metric, sizeof(metric));
	if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);

	//////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////
	// TRACKING PREPARATION
	//-----------------------------------------------------------
	// This block contains final set up for continuous tracking
	//-----------------------------------------------------------

	// Search for the first attached transmitter and turn it on
	for(id=0;id<m_System.m_config.numberTransmitters;id++)
	{
		if((m_pXmtr+id)->m_config.attached)
		{
			// Transmitter selection is a system function.
			// Using the SELECT_TRANSMITTER parameter we send the id of the
			// transmitter that we want to run with the SetSystemParameter() call
			errorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id));
			if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
			break;
		}
	}

	// Collect data from all birds
	// Loop through all sensors and get a data record if the sensor is attached.
	// Print result to screen
	// Note: The default data format is DOUBLE_POSITION_ANGLES. We can use this
	// format without first setting it.
	DOUBLE_POSITION_ANGLES_TIME_Q_RECORD record[8*4], *pRecord = record;

	// Set the data format type for each attached sensor.
	for(i=0;i<m_System.m_config.numberSensors;i++)
	{
		DATA_FORMAT_TYPE type = DOUBLE_POSITION_ANGLES_TIME_Q;
		errorCode = SetSensorParameter(i,DATA_FORMAT,&type,sizeof(type));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
	}
}

void GMagneticTracker::errorHandler(int error)
{
	char			buffer[1024];
	char			*pBuffer = &buffer[0];
	int				numberBytes;

	while(error!=BIRD_ERROR_SUCCESS)
	{
		error = GetErrorText(error, pBuffer, sizeof(buffer), SIMPLE_MESSAGE);
		numberBytes = strlen(buffer);
		buffer[numberBytes] = '\n';		// append a newline to buffer
		printf("%s", buffer);
	}
}

cv::Mat GMagneticTracker::stabilityReading(int numSeconds) {

	cout << "Press a number/letter and press ENTER to begin reading: ";
	char input;
	cin >> input;

	clock_t wait = 10;
	clock_t goal = wait + clock();

	int numReadings = numSeconds * 100;
	double x = 0, y = 0, z = 0, quality = 0;
	DOUBLE_POSITION_ANGLES_TIME_Q_RECORD record[8*4], *pRecord = record;

	cout << "Finding stabilized position";
	for (int i = 0; i < numReadings; i++) {
		if ( i % 100 == 0) cout << '.';

		// Delay 10ms between collecting data and wait until time delay expires
		while(goal>clock());
		// Set up time delay for next loop
		goal = wait + clock();

		int errorCode = GetSynchronousRecord(ALL_SENSORS, pRecord, sizeof(record[0]) * m_System.m_config.numberSensors);
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);

		x += record[0].x;
		y += record[0].y;
		z += record[0].z;
		//quality += (double) record[0].quality;
	}

	cv::Mat averageMat(3, 1, CV_64FC1);
	averageMat.at<double>(0,0) = x / numReadings;
	averageMat.at<double>(1,0) = y / numReadings;
	averageMat.at<double>(2,0) = z / numReadings;

	return averageMat;
}

GMagneticTracker::~GMagneticTracker(void) {
	// Turn off the transmitter before exiting
	// We turn off the transmitter by "selecting" a transmitter with an id of "-1"
	short id = -1;
	int errorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id));
	if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);

	//  Free memory allocations before exiting
	delete[] m_pSensor;
	delete[] m_pXmtr;
}