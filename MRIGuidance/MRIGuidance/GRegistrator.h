#ifndef GREGISTRATOR
#define GREGISTRATOR

#include <opencv2/core/core.hpp>
#include <string>
#include <vector>

class GRegistrator
{
public:
	// Classic Constructor
	GRegistrator();

	GRegistrator(int numberOfFiducials, int registrationMode, std::string windowName, std::vector<cv::Mat> images);

	void displayImageWindow();
	void registerFiducials();

	// Classic Destructor
	~GRegistrator(void){};

private:
	// Number of fiducial markers to be registered
	int m_nNumberOfFiducials;

	// Mode used for regsitering fiducials
	int m_nRegistrationMode;

	// Name of window being clicked on
	std::string m_sWindowName;

	// Points to current image index of vector of images being displayed
	int m_nImageIndex;

	// Contains image matrices
	std::vector<cv::Mat> m_vImages;

	// Matrix containing physical coordinates of fiducial markers
	cv::Mat m_PhysicalMat;

	// Matrix containing pixel coordinates of fiducial markers
	cv::Mat m_PixelMat;

	void onMousePixel(int e, int x, int y, int d, void *ptr);
	void pushPhysicalCoord();
};
#endif GREGISTRATOR