#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace std;
/**
* @function main
*/
int main( int argc, const char** argv )
{	
	CvCapture* capture;	
	capture = cvCaptureFromCAM(0);
	if (!capture)
	{		
		cout << "Camera couldn't be opened.";
		int c = cvWaitKey(5000);	
		return 0;
	}

	// create a window to display camera frame
	cvNamedWindow("Camera", CV_WINDOW_AUTOSIZE);
	IplImage* frame;
	while (true)
	{
		//Read the video stream		
		frame = cvQueryFrame( capture );		

		// display frame
		cvShowImage("Camera", frame);

		int c = cvWaitKey(100);
		if( (char)c == 27 ) { 
			// release resource
			cvReleaseImage(&frame);	
			exit(0); 
		}
	}
	
	return 0;
}