#include "GRegistrator.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace std;

GRegistrator::GRegistrator(int numberOfFiducials, int registrationMode, string windowName, vector<cv::Mat> images):
	m_nNumberOfFiducials(numberOfFiducials),
	m_nRegistrationMode(registrationMode),
	m_sWindowName(windowName),
	m_vImages(images)
{
	m_PhysicalMat = cv::Mat(3, 1, CV_64FC1);
	m_PixelMat = m_PhysicalMat;
}

void onMousePixel_(int e, int x, int y, int d, void *ptr)
{
	if (e != CV_EVENT_LBUTTONDOWN) {
		return;
	}

	cv::Mat *coords = (cv::Mat *)ptr;
	cout << x << ' ' << y << endl;
	coords->at<double>(0,0) = x;
	coords->at<double>(1,0) = y;
};

void GRegistrator::pushPhysicalCoord()
{
	if (m_nRegistrationMode == 1) {
		cv::Mat tempCol = cv::Mat(3, 1, CV_64F);

		cout << "Enter x coordinate: ";
		cin >> tempCol.at<double>(0,0);

		cout << "Enter y coordinate: ";
		cin >> tempCol.at<double>(1,0);

		cout << "Enter z coordinate: ";
		cin >> tempCol.at<double>(2,0);

		cv::hconcat(m_PhysicalMat, tempCol, m_PhysicalMat);
	}
}

void GRegistrator::displayImageWindow()
{
	m_nImageIndex = 0;
	cv::imshow(m_sWindowName, m_vImages[m_nImageIndex]);
	cv::createTrackbar("Image", m_sWindowName, &m_nImageIndex, m_vImages.size() - 1);
}

void printMat(cv::Mat m)
{
	for (size_t i = 0; i < m.rows; i++) {
		for (size_t j = 0; j < m.cols; j++) {
			cout << m.at<double>(i,j) << ' ';
		}
		cout << endl;
	}
};

void GRegistrator::registerFiducials()
{
	for (size_t i = 0; i < m_nNumberOfFiducials; i++) {
		cv::Mat tempCol(3, 1, CV_64FC1);
		tempCol.at<double>(0,0) = -1;
		
		pushPhysicalCoord();

		while(tempCol.at<double>(0,0) == -1) {
			cv::imshow(m_sWindowName, m_vImages[m_nImageIndex]);
			cv::setMouseCallback(m_sWindowName, onMousePixel_, &tempCol);

			int key = cv::waitKey(50);
			if (key == 27)
				break;
		}

		tempCol.at<double>(2,0) = m_nImageIndex;
		cv::hconcat(m_PixelMat, tempCol, m_PixelMat);
	}

	m_PhysicalMat = m_PhysicalMat.colRange(1, m_PhysicalMat.cols);
	m_PixelMat = m_PixelMat.colRange(1, m_PixelMat.cols);

	cout << "Physical: " << endl;
	printMat(m_PhysicalMat);
	cout << "Pixel: " << endl;
	printMat(m_PixelMat);
}