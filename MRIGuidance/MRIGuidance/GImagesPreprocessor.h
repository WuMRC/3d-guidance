#ifndef GIMAGESPREPROCESSOR
#define GIMAGESPREPROCESSOR

#include <string>
#include <vector>
#include <opencv2/core/core.hpp>

class GImagesPreprocessor
{
public:
	// Classic Constructor
	GImagesPreprocessor();

	// Custom Constructor
	// Sets m_sImagesFolder
	GImagesPreprocessor(std::string imagesFolder);

	// Public Member Functions
	void loadAllImages();

	void displayImage(cv::Mat image);

	void sliceView(std::vector<cv::Mat> originalImages, std::vector<cv::Mat> &slicedImages, std::string type);

	std::vector<cv::Mat> getImages() const {return m_vOriginalImages;};
	// Classic Destructor
	~GImagesPreprocessor(void){};

private:
	// Contains the folderpath to initial images
	std::string m_sImagesFolder;

	// Contains the image matrices for images in m_sImagesFolder
	std::vector<cv::Mat> m_vOriginalImages;
};

#endif