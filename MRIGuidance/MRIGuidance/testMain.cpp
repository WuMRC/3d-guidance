#include "GConfigParser.h"
#include "GImagesPreprocessor.h"
#include "GRegistrator.h"
#include "GMagneticTracker.h"
#include "GLiveTracking.h"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <cstdlib>

using namespace std;

void onMouse(int event, int x, int y, int, void* param);

int main()
{
	//GConfigParser *parser = new GConfigParser("ConfigurationData.xml");
	//parser->parseConfiguration();

	//GImagesPreprocessor *preprocessor = new GImagesPreprocessor(parser->getImagesLocation());
	//preprocessor->loadAllImages();

	//GRegistrator *registrator;
	//
	//if (parser->getRegistrationMode() == 1) {
	//	registrator = new GRegistrator(parser->getNumberOfFiducials(), parser->getRegistrationMode(), "Axial", preprocessor->getImages());
	//}
	//else if (parser->getRegistrationMode() == 2) {
	//	GMagneticTracker *tracker = new GMagneticTracker(parser->getMetricMode(), parser->getRate());
	//	registrator = new GRegistrator(parser->getNumberOfFiducials(), parser->getRegistrationMode(), "Axial", preprocessor->getImages(), tracker);
	//}

	//registrator->displayImageWindow();
	//registrator->registerFiducials();
	//cv::Mat T;
	//registrator->computeTransformation(T);

	//GLiveTracking *liveTracker = new GLiveTracking(preprocessor->getImages(), T);
	//liveTracker->sliceAllViews();
	//liveTracker->displayWindows();
	//liveTracker->track();

	//delete liveTracker;
	//delete parser;
	//delete preprocessor;

	cv::Mat mat1 = cv::Mat(3, 1, CV_64FC1);
	cv::Mat mat2 = mat1;

	cv::Mat tempVec = mat1;

	const double PI = 3.14159265358;
	const double theta = PI / 6;
	const double theta2 = PI / 3;

	cv::Mat trans = cv::Mat::eye(4, 4, CV_64FC1);
	cv::Mat rot1 = trans, rot2 = trans;

	trans.at<double>(0,3) = -1;
	trans.at<double>(1,3) = -1;
	trans.at<double>(2,3) = -1;

	rot1.at<double>(1,1) = cos(theta);
	rot1.at<double>(1,2) = -sin(theta);
	rot1.at<double>(2,1) = sin(theta);
	rot1.at<double>(2,2) = cos(theta);
	
	rot2.at<double>(0,0) = cos(theta2);
	rot2.at<double>(0,2) = sin(theta2);
	rot2.at<double>(2,0) = -sin(theta2);
	rot2.at<double>(2,2) = cos(theta2);

	cv::Mat transformer = rot1;

	cv::Mat rot = cv::Mat::eye(3, 3, CV_64FC1);
	rot.at<double>(1,1) = cos(theta);
	rot.at<double>(1,2) = -sin(theta);
	rot.at<double>(2,1) = sin(theta);
	rot.at<double>(2,2) = cos(theta);

	cv::Mat rot_ = cv::Mat::eye(3, 3, CV_64FC1);
	rot_.at<double>(0,0) = cos(theta2);
	rot_.at<double>(0,2) = sin(theta2);
	rot_.at<double>(2,0) = -sin(theta2);
	rot_.at<double>(2,2) = cos(theta2);

	const double numPoints = 15;

	for (int i = 0; i < numPoints; ++i) {
		tempVec.at<double>(0,0) = i;
		tempVec.at<double>(1,0) = i + 1;
		tempVec.at<double>(2,0) = i + 2;
		
		//tempVec.at<double>(0,0) = rand() % 100;
		//tempVec.at<double>(1,0) = rand() % 100;
		//tempVec.at<double>(2,0) = rand() % 100;

		cv::hconcat(mat1, tempVec, mat1);

		cv::Mat homogVec = cv::Mat::ones(4, 1, CV_64FC1);
		homogVec.at<double>(0,0) = tempVec.at<double>(0,0);
		homogVec.at<double>(1,0) = tempVec.at<double>(1,0);
		homogVec.at<double>(2,0) = tempVec.at<double>(2,0);

		homogVec = transformer * homogVec;

		cv::Mat storeVec = cv::Mat(3, 1, CV_64FC1);
		storeVec.at<double>(0,0) = homogVec.at<double>(0,0);
		storeVec.at<double>(1,0) = homogVec.at<double>(1,0);
		storeVec.at<double>(2,0) = homogVec.at<double>(2,0);

		cv::hconcat(mat2,  storeVec, mat2);
	}

	mat1 = mat1.colRange(1, mat1.cols);
	mat2 = mat2.colRange(1, mat2.cols);

	cv::Mat mat1Vec, mat2Vec;
	cv::reduce(mat1, mat1Vec, 1, CV_REDUCE_AVG);
	cv::reduce(mat2, mat2Vec, 1, CV_REDUCE_AVG);

	cv::Mat nOnes = cv::Mat::ones(1, numPoints, CV_64FC1);
	cv::Mat demeanedMat1 = mat1 - mat1Vec * nOnes;
	cv::Mat demeanedMat2 = mat2 - mat2Vec * nOnes;

	cv::Mat demeanedMat2T;
	cv::transpose(demeanedMat2, demeanedMat2T);

	cv::Mat H = demeanedMat1 * demeanedMat2T;

	cv::Mat S, U, V, R, t, T;

	cv::SVD::compute(H, S, U, V);

	cv::transpose(U, U);
	cv::transpose(V, V);
	R = V * U;

	if (cv::determinant(R) < 0) {
		V.at<double>(0, 2) *= -1; //-V.at<double>(0, numPoints - 1);
		V.at<double>(1, 2) *= -1; //-V.at<double>(1, numPoints - 1);
		V.at<double>(2, 2) *= -1; //-V.at<double>(2, numPoints - 1);
	}
	R = V * U;

	t = mat2Vec - R * mat1Vec;

	
	cv::Mat homogeneousRow = cv::Mat::zeros(1, 4, CV_64FC1);
	homogeneousRow.at<double>(0,3) = 1;
	
	cv::hconcat(R, t, T);
	cv::vconcat(T, homogeneousRow, T);

	cout << "Determinant of R: " << cv::determinant(R) << endl;

	for (int i = 0; i < numPoints; ++i) {
		cv::Mat temp = cv::Mat::ones(4, 1, CV_64FC1);
		temp.at<double>(0,0) = mat1.at<double>(0,i);
		temp.at<double>(1,0) = mat1.at<double>(1,i);
		temp.at<double>(2,0) = mat1.at<double>(2,i);

		cv::Mat transformed = T * temp;
		
		cout << "Expected: <" << mat2.at<double>(0,i) << ", " << mat2.at<double>(1,i) << ", " << mat2.at<double>(2,i) << '>' << endl;
		cout << "Actual: <" << transformed.at<double>(0,0) << ", " << transformed.at<double>(1,0) << ", " << transformed.at<double>(2,0) << '>' << endl;
		cout << endl;
	}

	return 0;
}

void onMouse(int event, int x, int y, int, void* param) {

	if (event != CV_EVENT_LBUTTONDOWN) {
		return;
	}

	cv::Point pt = cv::Point(x,y);
	cout << pt.x << ' ' << pt.y << ' ' << endl;
}