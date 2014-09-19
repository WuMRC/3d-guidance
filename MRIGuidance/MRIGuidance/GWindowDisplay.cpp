#include "GWindowDisplay.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;

GWindowDisplay::GWindowDisplay(string windowName, vector<cv::Mat> images, bool displayTrackbar):
	m_sWindowName(windowName),
	m_vImages(images),
	m_bTrackbarOn(displayTrackbar),
	m_nImageNumber(0)
{}

void GWindowDisplay::createWindow() {
	displayWindow();

	if (m_bTrackbarOn) {
		cv::createTrackbar("Image", m_sWindowName, &m_nImageNumber, m_vImages.size() - 1);
	}
}

void GWindowDisplay::displayWindow() {
	cv::imshow(m_sWindowName, m_vImages[m_nImageNumber]);
}

void GWindowDisplay::updateWindow(int x, int y, int image) {
	m_nImageNumber = image;

	cv::Mat imageToDisplay = m_vImages[m_nImageNumber].clone();

	cv::circle(imageToDisplay, cv::Point(x, y), 5, cv::Scalar(0, 255, 0), 1);

	cv::imshow(m_sWindowName, imageToDisplay);

	//displayWindow();
};

void GWindowDisplay::deleteWindow() {
	cv::destroyWindow(m_sWindowName);
}