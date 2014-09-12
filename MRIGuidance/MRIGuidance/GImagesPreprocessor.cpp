#include "GImagesPreprocessor.h"
#include <boost/filesystem.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

GImagesPreprocessor::GImagesPreprocessor(string filePath):m_sImagesFolder(filePath)
{};

void GImagesPreprocessor::loadAllImages()
{
	// REQUIRES:
	//		m_sImagesFolder has been defined to a valid filepath containing images
	// EFFECTS:
	//		Loads image matrices from images in m_sImagesFolder

	// Get list of filenames in m_sImagesFolder
	boost::filesystem::directory_iterator iterator(m_sImagesFolder);

	// Loop through files in m_sImagesFolder
	for (; iterator != boost::filesystem::directory_iterator(); ++iterator) {

		// Retrieve file at position "iterator"
		string filename = iterator->path().filename().string();

		cv::Mat image = cv::imread(m_sImagesFolder + filename, CV_LOAD_IMAGE_GRAYSCALE);

		m_vOriginalImages.push_back(image);
	}
};

void GImagesPreprocessor::sliceView(vector<cv::Mat> originalImages, vector<cv::Mat> &slicedImages, string type)
{
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
};

void GImagesPreprocessor::displayImage(cv::Mat image)
{
	cv::namedWindow("Test", CV_WINDOW_AUTOSIZE);
	cv::imshow("Test", image);

	cv::waitKey(0);
};