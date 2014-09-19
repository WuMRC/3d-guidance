#include "GConfigParser.h"
#include "GImagesPreprocessor.h"
#include "GRegistrator.h"
#include "GMagneticTracker.h"
#include "GLiveTracking.h"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

void onMouse(int event, int x, int y, int, void* param);

int main()
{
	GConfigParser *parser = new GConfigParser("ConfigurationData.xml");
	parser->parseConfiguration();

	GImagesPreprocessor *preprocessor = new GImagesPreprocessor(parser->getImagesLocation());
	preprocessor->loadAllImages();

	GRegistrator *registrator;
	
	if (parser->getRegistrationMode() == 1) {
		registrator = new GRegistrator(parser->getNumberOfFiducials(), parser->getRegistrationMode(), "Axial", preprocessor->getImages());
	}
	else if (parser->getRegistrationMode() == 2) {
		GMagneticTracker *tracker = new GMagneticTracker(parser->getMetricMode(), parser->getRate());
		registrator = new GRegistrator(parser->getNumberOfFiducials(), parser->getRegistrationMode(), "Axial", preprocessor->getImages(), tracker);
	}

	registrator->displayImageWindow();
	registrator->registerFiducials();
	cv::Mat T;
	registrator->computeTransformation(T);

	GLiveTracking *liveTracker = new GLiveTracking(preprocessor->getImages(), T);
	liveTracker->sliceAllViews();
	liveTracker->displayWindows();
	liveTracker->track();

	delete liveTracker;
	delete parser;
	delete preprocessor;
	return 0;
}

void onMouse(int event, int x, int y, int, void* param) {

	if (event != CV_EVENT_LBUTTONDOWN) {
		return;
	}

	cv::Point pt = cv::Point(x,y);
	cout << pt.x << ' ' << pt.y << ' ' << endl;
}