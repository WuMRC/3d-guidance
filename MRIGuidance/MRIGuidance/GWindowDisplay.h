#ifndef GWINDOWDISPLAY
#define GWINDOWDISPLAY

#include<opencv2/core/core.hpp>
#include <vector>
#include <string>

class GWindowDisplay {

public:
	
	// Classic Constructor
	GWindowDisplay();

	// Custom Constructor
	GWindowDisplay(std::string windowName, std::vector<cv::Mat> images, bool displayTrackbar);

	// Create initial window with trackbar
	void createWindow();

	// Displays window with images
	void displayWindow();

	// Updates window to display correct image and point
	void updateWindow(int row, int col, int image);

	// Destroys window
	void deleteWindow();

	// Getter functions
	int getImageNumber() const {return m_nImageNumber;};
	std::string getWindowName() const {return m_sWindowName;};
	int getImageVectorSize() const {return m_vImages.size();};

	// Classic Destructor
	~GWindowDisplay(void){};

private:

	// Vector of images to populate window
	std::vector<cv::Mat> m_vImages;

	// Name of window to populate
	std::string m_sWindowName;

	// Trackbar mode (true = on, false = off)
	bool m_bTrackbarOn;

	// Index of image vector
	int m_nImageNumber;

};

#endif