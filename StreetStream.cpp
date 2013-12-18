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

void StreetStream::showStreetMonitor(cv::Mat& streamFrame){
	imshow("StreetMonitor", streamFrame);
}

void StreetStream::startMonitor(){
	//Videodatei öffnen
	//videoCapture.open(path);

	//oder Kamera öffnen
	//videoCapture.open(0); //keinen Pfad übergeben sondern nur das Device (0=Standarddevice)
	inputStream.open(inputPath);

	//Videoframes lesen
	while(true){
		Mat videoFrame;  //kann in c++ auch ohne () direkt initialisiert werden!!
		//Videoframe lesen
		bool success = inputPath.read(streamFrame);
		if(success == false){
			break;
		}
		showStreetMonitor(streamFrame);
		//30 ms auf einen Tastendruck warten, bei Tastendruck beenden
	}
}
