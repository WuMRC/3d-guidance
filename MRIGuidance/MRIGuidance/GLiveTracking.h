#ifndef GLIVETRACKING
#define GLIVETRACKING

#include "GWindowDisplay.h"
#include "GMagneticTracker.h"
#include <opencv2/core/core.hpp>
#include <vector>

class GLiveTracking {

public:

	// Classic Constructor
	GLiveTracking();

	// Track without using magnetic tracker
	GLiveTracking(std::vector<cv::Mat> axialImages, cv::Mat T);

	// Track with magnetic tracker
	GLiveTracking(std::vector<cv::Mat> axialImages, cv::Mat T, GMagneticTracker *tracker);

	// Slice axial view to obtain coronal and sagittal views
	void sliceAllViews();

	// Display image windows
	void displayWindows();

	// Perform live tracking
	void track();

	// Classic Destructor
	~GLiveTracking(void){};

private:

	// Windows for images
	GWindowDisplay *m_pAxialWindow, *m_pCoronalWindow, *m_pSagittalWindow;

	// Axial images
	std::vector<cv::Mat> m_vAxialImages;

	// Magnetic tracker
	GMagneticTracker *m_pTracker;

	// Transformation matrix from physical coords to coords in axial frame
	cv::Mat m_T;

	// Slice specific view of axial images
	void sliceView(std::vector<cv::Mat> originalImages, std::vector<cv::Mat> &slicedImages, std::string type);
};

#endif