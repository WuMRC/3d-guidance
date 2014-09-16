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

GRegistrator::GRegistrator(int numberOfFiducials, int registrationMode, std::string windowName, std::vector<cv::Mat> images, GMagneticTracker *tracker):
	m_nNumberOfFiducials(numberOfFiducials),
	m_nRegistrationMode(registrationMode),
	m_sWindowName(windowName),
	m_vImages(images),
	m_pTracker(tracker)
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
	else {
		cv::hconcat(m_PhysicalMat, m_pTracker->stabilityReading(5), m_PhysicalMat);
	}
}

void GRegistrator::displayImageWindow()
{
	m_nImageIndex = 0;
	cv::imshow(m_sWindowName, m_vImages[m_nImageIndex]);
	cv::createTrackbar("Image", m_sWindowName, &m_nImageIndex, m_vImages.size() - 1);
}

void printMat(const cv::Mat &m)
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
}

void GRegistrator::computeTransformation(cv::Mat &T) {

	cout << "Physical Matrix: " << endl;
	printMat(m_PhysicalMat);
	cout << "Pixel Matrix: " << endl;
	printMat(m_PixelMat);

	cv::Mat pixelMeanVec, physicalMeanVec;

	// Find mean values for each row of each matrix
	cv::reduce(m_PhysicalMat, physicalMeanVec, 1, CV_REDUCE_AVG);
	cv::reduce(m_PixelMat, pixelMeanVec, 1, CV_REDUCE_AVG);

	cout << "physicalMeanVec: " << endl;
	printMat(physicalMeanVec);
	cout << "pixelMeanVec: " << endl;
	printMat(pixelMeanVec);

	cv::Mat nOnes = cv::Mat::ones(1, m_nNumberOfFiducials, CV_64FC1);
	cout << "nOnes: " << endl;
	printMat(nOnes);

	// Compute demeaned matrix for physical and pixel coordinates
	cv::Mat demeanedPhysical = m_PhysicalMat - physicalMeanVec * nOnes;
	cv::Mat demeanedPixel = m_PixelMat - pixelMeanVec * nOnes;

	cout << "demeanedPhysical: " << endl;
	printMat(demeanedPhysical);
	cout << "demeanedPixel: " << endl;
	printMat(demeanedPixel);

	cv::Mat demeanedPixelT;
	cv::transpose(demeanedPixel, demeanedPixelT);

	cv::Mat H = demeanedPhysical * demeanedPixelT;
	cout << "H: " << endl;
	printMat(H);

	// Compute singular value decomposition
	cv::Mat S, U, Vt, R, t;
	cv::SVD::compute(H, S, U, Vt);
	//R = U * Vt;
	cv::transpose(U, U);
	cv::transpose(Vt, Vt);
	R = Vt * U;
	t = pixelMeanVec - R * physicalMeanVec;

	cout << "R: " << endl;
	printMat(R);
	cout << "t: " << endl;
	printMat(t);

	cv::Mat homogeneousRow = cv::Mat::zeros(1, 4, CV_64FC1);
	homogeneousRow.at<double>(0,3) = 1;
	
	cv::hconcat(R, t, T);
	cv::vconcat(T, homogeneousRow, T);

	cout << "T: " << endl;
	printMat(T);

	cv::Mat testPhysicalVec = cv::Mat::ones(4, 1, CV_64FC1);
	testPhysicalVec.at<double>(1,0) = 2;
	testPhysicalVec.at<double>(2,0) = 3;

	cv::Mat resultantPixelVec = T * testPhysicalVec;
	cout << "resultantPixelVec" << endl;
	printMat(resultantPixelVec);
}