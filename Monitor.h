#ifndef MONITOR_H__
#define MONITOR_H__

#include <opencv2\opencv.hpp>
#include <string>

#include "StreetStream.h"

class Monitor{

public:
	Monitor(void);
	~Monitor(void);
	void process(const std::string& path);
	void detectMotion();

private:
	cv::Mat aktFrame;
	cv::Mat firstFrame;
	cv::Mat binaryFrame;
	cv::Mat outputFrame;
	StreetStream stream;
};

#endif