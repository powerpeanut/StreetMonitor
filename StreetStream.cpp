#include <opencv2\opencv.hpp>
#include <string>

#include "StreetStream.h"

using namespace cv;
using namespace std;

StreetStream::StreetStream(){
	
}

StreetStream::~StreetStream(){
}

void StreetStream::setInputPath(const string& path){
	inputPath = path;
}

void StreetStream::startStream(){
	//Videodatei öffnen >> videoCapture.open(path);
	//oder Kamera öffnen >> videoCapture.open(0);  ((keinen Pfad übergeben sondern nur das Device (0=Standarddevice)))
	if(inputPath == "0"){
		inputStream.open(0);
	}
	else{
		inputStream.open(inputPath);
	}
	width = inputStream.get(CV_CAP_PROP_FRAME_WIDTH);
	height = inputStream.get(CV_CAP_PROP_FRAME_HEIGHT);
}

VideoCapture StreetStream::getInputStream(){
	return inputStream;
}

int StreetStream::getWidth(){
	return width;
}

int StreetStream::getHeight(){
	return height;
}
