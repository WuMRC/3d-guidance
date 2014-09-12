#include "GConfigParser.h"
#include "GImagesPreprocessor.h"
#include "GRegistrator.h"
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

	GRegistrator *registrator = new GRegistrator(parser->getNumberOfFiducials(), parser->getRegistrationMode(), "Axial", preprocessor->getImages());
	registrator->displayImageWindow();
	registrator->registerFiducials();
	//vector<cv::Mat> coronal;
	//preprocessor->sliceView(preprocessor->getImages(), coronal,"row");
	//vector<cv::Mat> sagittal; 
	//preprocessor->sliceView(preprocessor->getImages(), sagittal,"col");

	//cv::imshow("Axial", preprocessor->getImages()[0]);
	//cv::imshow("Coronal", coronal[0]);
	//cv::imshow("Sagittal", sagittal[0]);

	//int axialSlider = 0;
	//cv::createTrackbar("Image", "Axial", &axialSlider, preprocessor->getImages().size() - 1);

	//int coronalSlider = 0;
	//cv::createTrackbar("Image", "Coronal", &coronalSlider, coronal.size() - 1);

	//int sagittalSlider = 0;
	//cv::createTrackbar("Image", "Sagittal", &sagittalSlider, sagittal.size() - 1);

	//cv::Mat test = cv::Mat::zeros(3,3, CV_64FC1);

	//while(true) {
	//	cv::imshow("Axial", preprocessor->getImages()[axialSlider]);
	//	cv::imshow("Coronal", coronal[coronalSlider]);
	//	cv::imshow("Sagittal", sagittal[sagittalSlider]);

	//	//cv::setMouseCallback("Axial", onMouse, 0);


	//	int key = cv::waitKey(50);
	//	if (key == 27) {
	//		break;
	//	}
	//}

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