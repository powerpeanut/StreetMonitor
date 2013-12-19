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

void StreetStream::startMonitor(){
	//Videodatei öffnen >> videoCapture.open(path);
	//oder Kamera öffnen >> videoCapture.open(0);  ((keinen Pfad übergeben sondern nur das Device (0=Standarddevice)))
	if(inputPath == "0")
		inputStream.open(0);
	else
		inputStream.open(inputPath);

	//Videoframes lesen
	while(waitKey(30) == -1){
		//Mat streamFrame;
		//Videoframes lesen
		bool success = inputStream.read(streamFrame);
		if(success == false){
			break;
		}
		imshow("StreetMonitor", streamFrame);
	}
	destroyAllWindows();
}

