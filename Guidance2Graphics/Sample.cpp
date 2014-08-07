#include "stdafx.h"

extern "C" void exit( int);

using namespace std;

void errorHandler(int error);
Vector3 averageForTime(int numSeconds, CSystem system);


int main()
{
	//////////////////////////////////////////////////////////////
	// INITIALIZE GRAPHICS WINDOWS
	//////////////////////////////////////////////////////////////
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	int DESKTOP_HEIGHT = desktop.height;
	int DESKTOP_WIDTH = desktop.width;

	ViewManager axialView("..\\Monkey_Skull_axial", "slice", ".png", DESKTOP_WIDTH / 3, DESKTOP_HEIGHT / 3, 75, 1, 129, 196);
	ViewManager coronalView("..\\Monkey_Skull_coronal_cut_flipped", "slice", ".png", DESKTOP_WIDTH / 3, DESKTOP_HEIGHT / 3, 196, 196/75, 129, 75);
	ViewManager sagittalView("..\\Monkey_Skull_sagittal_cut_flipped", "slice", ".png", DESKTOP_WIDTH / 3, DESKTOP_HEIGHT / 3, 129, 129/75, 196, 75);
	/*ViewManager coronalView("..\\Monkey_Skull_coronal_flipped", "slice", ".png", DESKTOP_WIDTH / 3, DESKTOP_HEIGHT / 3, 256, 195/75);
	ViewManager sagittalView("..\\Monkey_Skull_sagittal_flipped", "slice", ".png", DESKTOP_WIDTH / 3, DESKTOP_HEIGHT / 3, 256, 125/75);*/

	sf::RenderWindow axialWindow(sf::VideoMode(axialView.getWindowWidth(),axialView.getWindowHeight()), "Axial View");
	sf::RenderWindow coronalWindow(sf::VideoMode(coronalView.getWindowWidth(),coronalView.getWindowHeight()), "Coronal View");
	sf::RenderWindow sagittalWindow(sf::VideoMode(sagittalView.getWindowWidth(),sagittalView.getWindowHeight()), "Sagittal View");

	axialWindow.setPosition(sf::Vector2i(DESKTOP_WIDTH / 2, 0));
	coronalWindow.setPosition(sf::Vector2i(0, 0));
	sagittalWindow.setPosition(sf::Vector2i(0, DESKTOP_HEIGHT / 2));
	
	// Draw images to windows for first time
	axialWindow.clear();
	axialWindow.draw(axialView.getImageSprite());
	axialWindow.draw(axialView.getCircle());
	axialWindow.display();

	coronalWindow.clear();
	coronalWindow.draw(coronalView.getImageSprite());
	coronalWindow.draw(coronalView.getCircle());
	coronalWindow.display();

	sagittalWindow.clear();
	sagittalWindow.draw(sagittalView.getImageSprite());
	sagittalWindow.draw(sagittalView.getCircle());
	sagittalWindow.display();

	//////////////////////////////////////////////////////////////
	// INITIALIZE VARIABLES FOR ATC3DG SYSTEM
	//-----------------------------------------------------------
	// Set variables which will be used for the entire system.
	//-----------------------------------------------------------
	CSystem	PCIBird;
	CSensor *pSensor;
	CXmtr *pXmtr;
	int errorCode;
	int i;
	int sensorID;
	short id;
	int records = 20000;
	const int CALIBRATION_TIME = 5;
	double rate = 75;
	BOOL metric = true;
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
	errorCode = GetBIRDSystemConfiguration(&PCIBird.m_config);
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
	pSensor = new CSensor[PCIBird.m_config.numberSensors];
	for(i=0;i<PCIBird.m_config.numberSensors;i++)
	{
		errorCode = GetSensorConfiguration(i, &(pSensor+i)->m_config);
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
	pXmtr = new CXmtr[PCIBird.m_config.numberTransmitters];
	for(i=0;i<PCIBird.m_config.numberTransmitters;i++)
	{
		errorCode = GetTransmitterConfiguration(i, &(pXmtr+i)->m_config);
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
	for(id=0;id<PCIBird.m_config.numberTransmitters;id++)
	{
		if((pXmtr+id)->m_config.attached)
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
	for(i=0;i<PCIBird.m_config.numberSensors;i++)
	{
		DATA_FORMAT_TYPE type = DOUBLE_POSITION_ANGLES_TIME_Q;
		errorCode = SetSensorParameter(i,DATA_FORMAT,&type,sizeof(type));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
	}
	//////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////
	// READ AND USE TRACKER VALUES
	//-----------------------------------------------------------
	// Read tracker values from sensor and dislay information to
	// graphical window
	//-----------------------------------------------------------
	cout << "Collecting data records at " << rate * 3.0 <<  "hz" << endl;
	Vector3 origin, current, difference;
	char uselessInput;
	bool continueReading = true;
	i = 0;

	while (continueReading)//for(i=0;i<records;i++)
	{
		// Prompt physical selection of origin
		if (i == 0) {
			cout << "\nType 's' and press <ENTER> to set origin" << endl;
			cin >> uselessInput;
			origin = averageForTime(CALIBRATION_TIME, PCIBird);
			current = origin;
		}
		else {
			// Obtain readings for all connected birds
			errorCode = GetSynchronousRecord(ALL_SENSORS, pRecord, sizeof(record[0]) * PCIBird.m_config.numberSensors);
			if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);

			// Set current coordinates of sensor
			current.set(record[0].x, record[0].y, record[0].z);
		}

		cout << current.at(0) - origin.at(0) << '\t' << current.at(1) - origin.at(1) << '\t' << current.at(2) - origin.at(2) << endl;

		// Find the difference between the current coordinates and the origin
		difference = current - origin;

		// Round points to the nearest integer
		difference.set(floor(difference.at(0) + .5), floor(difference.at(0) + .5), floor(difference.at(0) + .5));

		// Set origin images for each window
		if (i == 0) {
			axialView.setOriginImage(difference.at(2));
			coronalView.setOriginImage(difference.at(0));
			sagittalView.setOriginImage(difference.at(1));
		}

		// Update all windows and their contained circles
		axialView.resetImage(axialView.updateImage(-difference.at(2)));
		axialView.resetCircle(axialView.updateCircleX(difference.at(1)),coronalView.updateCircleY(-difference.at(0)));

		coronalView.resetImage(coronalView.updateImage(-difference.at(0)));
		coronalView.resetCircle(coronalView.updateCircleX(-difference.at(1)),coronalView.updateCircleY(difference.at(2)));

		sagittalView.resetImage(sagittalView.updateImage(difference.at(1)));
		sagittalView.resetCircle(sagittalView.updateCircleX(-difference.at(0)),sagittalView.updateCircleY(difference.at(2)));

		axialWindow.clear();
		axialWindow.draw(axialView.getImageSprite());
		axialWindow.draw(axialView.getCircle());
		axialWindow.display();

		coronalWindow.clear();
		coronalWindow.draw(coronalView.getImageSprite());
		coronalWindow.draw(coronalView.getCircle());
		coronalWindow.display();

		sagittalWindow.clear();
		sagittalWindow.draw(sagittalView.getImageSprite());
		sagittalWindow.draw(sagittalView.getCircle());
		sagittalWindow.display();

		// Prompt user to continue reading each (int)records number or readings
		if (i % records == 0 && i != 0 ) {
			cout << "\nPress 'c' to continue and 'q' to quit" << endl;
			cin >> uselessInput;

			if (uselessInput != 'c') {
				continueReading = false;
			}
		}

		i++;
	}

	// Turn off the transmitter before exiting
	// We turn off the transmitter by "selecting" a transmitter with an id of "-1"
	id = -1;
	errorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id));
	if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);

	//  Free memory allocations before exiting
	delete[] pSensor;
	delete[] pXmtr;

	return 0;
}

//	ERROR HANDLER
//
// This is a simplified error handler.
// This error handler takes the error code and passes it to the GetErrorText()
// procedure along with a buffer to place an error message string.
// This error message string can then be output to a user display device
// like the console
// Specific error codes should be parsed depending on the application.

void errorHandler(int error)
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

Vector3 averageForTime(int numSeconds, CSystem system )
{
	clock_t wait = 10;
	clock_t goal = wait + clock();

	int numReadings = numSeconds * 100;
	double x = 0, y = 0, z = 0;
	DOUBLE_POSITION_ANGLES_TIME_Q_RECORD record[8*4], *pRecord = record;

	cout << "Finding stabilized position";
	for (int i = 0; i < numReadings; i++) {
		if ( i % 100 == 0) cout << '.';

		// Delay 10ms between collecting data and wait until time delay expires
		while(goal>clock());
		// Set up time delay for next loop
		goal = wait + clock();

		int errorCode = GetSynchronousRecord(ALL_SENSORS, pRecord, sizeof(record[0]) * system.m_config.numberSensors);
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);

		x += record[0].x;
		y += record[0].y;
		z += record[0].z;
	}

	Vector3 averageReadings(x / numReadings, y / numReadings, z / numReadings);

	cout << "AVERAGE:" << endl;
	cout << averageReadings.at(0) << ' ' << averageReadings.at(1) << ' ' << averageReadings.at(2) << endl;

	return averageReadings;
}
