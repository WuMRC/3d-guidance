#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <dirent.h>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <math.h>
#include "ViewManager.h"
#include <iostream>

extern "C" void exit( int);

// Forward declaration of error handler
//
// A very simple error handler is provided at the end of this file
// It simply takes error codes and converts them to message strings
// then outputs them to the console.
//
void errorHandler(int error);

// An output buffer
// 10000 records * 8 possible units * 4 sensors per unit *128 bytes of string storage
char output[10000][8*4][128] = { { { 0,}, }, };

using namespace std;

/////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////

int main()
{
	// Initialize graphics windows
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

	// Initialize necesarry variables for tracker API
	CSystem	PCIBird;
	CSensor *pSensor;
	CXmtr *pXmtr;
	int errorCode;
	int i;
	int sensorID;
	short id;
	int records = 20000;
	double rate = 255.0f;
	const int INCH_2_MM = 25.4;

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

	// Get tracker system configuration
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

	// Get sensor configuration
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

	// Get transmitter configuration
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

	// Set the measurement rate
	errorCode = SetSystemParameter(MEASUREMENT_RATE, &rate, sizeof(rate));
	if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);

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

	// Collect as many records as specified in the records var
	printf("Collecting %d data records at %3.2fhz...\n", records, rate*3.0);
	double xCurr, yCurr, zCurr;
	double xOrigin, yOrigin, zOrigin;
	int xDiff, yDiff, zDiff;
	char uselessInput;
	bool continueReading = true;
	i = 0;

	while (continueReading)//for(i=0;i<records;i++)
	{
		// Set previous values to be last current ones
		if (i == 0) {
			cout << "\nType 's' and press <ENTER> to set origin" << endl;
			cin >> uselessInput;
		}

		errorCode = GetSynchronousRecord(ALL_SENSORS, pRecord, sizeof(record[0]) * PCIBird.m_config.numberSensors);
		if(errorCode!=BIRD_ERROR_SUCCESS) 
		{
			errorHandler(errorCode);
		}

		xCurr = record[0].x * INCH_2_MM;
		yCurr = record[0].y * INCH_2_MM;
		zCurr = record[0].z * INCH_2_MM;

		if (i == 0) {
			xOrigin = xCurr;
			yOrigin = yCurr;
			zOrigin = zCurr;
		}

		cout << xCurr - xOrigin << '\t' << yCurr - yOrigin << '\t' << zCurr - zOrigin << endl;

		xDiff = floor(xCurr - xOrigin);
		yDiff = floor(yCurr - yOrigin);
		zDiff = floor(zCurr - zOrigin);
		
		if (i == 0) {
			axialView.setOriginImage(zDiff);
			coronalView.setOriginImage(xDiff);
			sagittalView.setOriginImage(yDiff);
		}


		axialView.resetImage(axialView.updateImage(-zDiff));
		axialView.resetCircle(axialView.updateCircleX(yDiff),coronalView.updateCircleY(-xDiff));

		coronalView.resetImage(coronalView.updateImage(-xDiff));
		coronalView.resetCircle(coronalView.updateCircleX(-yDiff),coronalView.updateCircleY(zDiff));

		sagittalView.resetImage(sagittalView.updateImage(yDiff));
		sagittalView.resetCircle(sagittalView.updateCircleX(-xDiff),sagittalView.updateCircleY(zDiff));

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
