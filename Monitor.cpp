#include <opencv2\opencv.hpp>
#include <string>

#include "Monitor.h"

using namespace cv;
using namespace std;

Monitor::Monitor(){
	
}

Monitor::~Monitor(){
}

void Monitor::openStream(string path){
	stream.setInputPath(path);
	stream.startStream();

	//Frames lesen, ausgeben, lastFrame sichern
	while(waitKey(30) == -1){
		//Videoframes lesen
		bool success = stream.getInputStream().read(aktStreamFrame);
		if(success == false){
			break;
		}
		stream.showStream(aktStreamFrame);

		//some code here um frames zu vergleichen und Bewegung zu erkennen

		lastStreamFrame = aktStreamFrame;
	}
	destroyAllWindows();
}