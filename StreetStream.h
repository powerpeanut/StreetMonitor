#ifndef STREETSTREAM_H__
#define STREETSTREAM_H__

#include <opencv2\opencv.hpp>
#include <string>

class StreetStream{

public:
	StreetStream(void);
	~StreetStream(void);
	void setInputPath(const std::string& path);
	void startStream();
	cv::VideoCapture getInputStream();
	int getWidth();
	int getHeight();

private:
	std::string inputPath; // als kompletter Pfad: "C:\\blabla\\pfad.avi" -- doppelbackslash oder 0 für Kamera (Device (0=Standarddevice))
	cv::VideoCapture inputStream;
	cv::Mat streamFrame;
	int width;
	int height;
};

#endif