#ifndef __DISP2DEPTH__
#define __DISP2DEPTH__

#include <opencv2/opencv.hpp>
#include <iostream>

class DepthBuild
{
public:
	DepthBuild();
	~DepthBuild();

	void work(cv::Mat dispMap, cv::Mat& depthMap, cv::Mat K);

private:

};


#endif //__DISP2DEPTH__