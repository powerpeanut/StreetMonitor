#ifndef STREETSTREAM_H__
#define STREETSTREAM_H__

#include <opencv2\opencv.hpp>
#include <string>

class StreetStream{

public:
	StreetStream(void);
	~StreetStream(void);
	void setInputPath(std::string path);
	void showStreetMonitor(cv::Mat& streamFrame);
	void startMonitor();

private:
	const string inputPath; // als kompletter Pfad: "C:\\blabla\\pfad.avi" -- doppelbackslash oder 0 f�r Kamera (Device (0=Standarddevice))
	cv::VideoCapture inputStream;
	cv::Mat streamFrame;
};

#endif