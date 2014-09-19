#include "GLiveTracking.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;

GLiveTracking::GLiveTracking(vector<cv::Mat> axialImages, cv::Mat T):
	m_vAxialImages(axialImages),
	m_T(T),
	m_pAxialWindow(new GWindowDisplay("Axial", axialImages, true))
{
	m_pCoronalWindow = NULL;
	m_pSagittalWindow = NULL;
};

GLiveTracking::GLiveTracking(vector<cv::Mat> axialImages, cv::Mat T, GMagneticTracker *tracker):
	m_vAxialImages(axialImages),
	m_T(T),
	m_pAxialWindow(new GWindowDisplay("Axial", axialImages, false)),
	m_pTracker(tracker)
{
	m_pCoronalWindow = NULL;
	m_pSagittalWindow = NULL;
};

void GLiveTracking::sliceView(vector<cv::Mat> originalImages, vector<cv::Mat> &slicedImages, string type) {
	// REQUIRES:
	//		type is either "row" or "col"
	//		slicedImages is originally empty (if it is not, it will retain all previous data)
	// MODIFIES:
	//		Populates slicedImages with image matrices
	// EFFECTS:
	//		Creates vector of cv::Mat containing sliced view of the original images
	//		If the user inputs "row", the original images will be sliced horizontally
	//		If the user inputs "col", the original images will be sliced vertically

	if (type == "row") {
		for (size_t i = 0; i < originalImages[0].rows; i++) {
			cv::Mat tempMat = cv::Mat(0, originalImages[1].cols, CV_64FC1);
			for (size_t j = 0; j < originalImages.size(); j++) {
				tempMat.push_back(originalImages[j].row(i));
			}
			slicedImages.push_back(tempMat);
		}
	}
	else if (type == "col"){
		for (size_t i = 0; i < originalImages[0].cols; i++) {
			cv::Mat tempMat = cv::Mat(0, originalImages[1].rows, CV_64FC1);
			for (size_t j = 0; j < originalImages.size(); j++) {
				cv::Mat transposedTemp;
				cv::transpose(originalImages[j], transposedTemp);
				tempMat.push_back(transposedTemp.row(i));
			}
			slicedImages.push_back(tempMat);
		}
	}
	else {
		cout << "Input type must be either row or col" << endl;
		exit(2);
	}
}

void GLiveTracking::sliceAllViews() {

	vector<cv::Mat> coronalImages, sagittalImages;

	sliceView(m_vAxialImages, coronalImages, "row");
	sliceView(m_vAxialImages, sagittalImages, "col");

	m_pCoronalWindow = new GWindowDisplay("Coronal", coronalImages, false);
	m_pSagittalWindow = new GWindowDisplay("Sagittal", sagittalImages, false);
};

void GLiveTracking::displayWindows() {
	m_pAxialWindow->createWindow();
	m_pCoronalWindow->createWindow();
	m_pSagittalWindow->createWindow();

	// take this out
	cv::waitKey(0);
};

void onMouse_(int e, int x, int y, int d, void *ptr) {
	if (e != CV_EVENT_LBUTTONDOWN) {
		return;
	}

	cv::Mat *coords = (cv::Mat *)ptr;
	cout << x << ' ' << y << endl;
	coords->at<double>(0,0) = x;
	coords->at<double>(1,0) = y;
};

void GLiveTracking::track() {
	int x, y, imageNumber;
	cv::Mat tempVals(3, 1, CV_64FC1);
	tempVals.at<double>(0,0) = -1;

	while (true) {
		cv::setMouseCallback(m_pAxialWindow->getWindowName(), onMouse_, &tempVals);

		if (tempVals.at<double>(0,0) != -1) {
			x = tempVals.at<double>(0,0);
			y = tempVals.at<double>(1,0);
			imageNumber = m_pAxialWindow->getImageNumber();

			m_pAxialWindow->updateWindow(x, y, imageNumber);
			m_pCoronalWindow->updateWindow(x, imageNumber, y);
			m_pSagittalWindow->updateWindow(y, imageNumber, x);
		}

		tempVals.at<double>(0,0) = -1;

		int key = cv::waitKey(50);
		if (key == 27)
			break;
	}

}