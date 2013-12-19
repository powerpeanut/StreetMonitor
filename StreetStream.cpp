#include <opencv2\opencv.hpp>
#include <string>

#include "StreetStream.h"

using namespace cv;
using namespace std;

StreetStream::StreetStream(){
	namedWindow("StreetMonitor");
}

StreetStream::~StreetStream(){
}

void StreetStream::setInputPath(string path){
	inputPath = path;
}

void StreetStream::startStream(){
	//Videodatei �ffnen >> videoCapture.open(path);
	//oder Kamera �ffnen >> videoCapture.open(0);  ((keinen Pfad �bergeben sondern nur das Device (0=Standarddevice)))
	if(inputPath == "0")
		inputStream.open(0);
	else
		inputStream.open(inputPath);
}

VideoCapture StreetStream::getInputStream(){
	return inputStream;
}

void StreetStream::showStream(Mat aktStreamFrame){
	imshow("StreetMonitor", aktStreamFrame);
}
