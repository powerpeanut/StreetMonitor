#include <opencv2\opencv.hpp>
#include <string>

#include "Monitor.h"

using namespace cv;
using namespace std;

Monitor::Monitor(){
	
}

Monitor::~Monitor(){
}

void Monitor::process(string path){
	stream.setInputPath(path);
	stream.startStream();

	//Frames lesen, ausgeben, lastFrame sichern
	while(waitKey(30) == -1){
		lastStreamFrame = aktStreamFrame;
		//Videoframes lesen
		bool success = stream.getInputStream().read(aktStreamFrame);
		if(success == false){
			break;
		}
		stream.showStream(aktStreamFrame);

		detectMotion();

	}
	destroyAllWindows();
}

void Monitor::detectMotion(){
	//some code here um frames zu vergleichen und Bewegung zu erkennen
}