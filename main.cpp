#include <iostream>
#include <opencv2\opencv.hpp>
#include <string>

#include "StreetStream.h"

using namespace cv;
using namespace std;

int main(){
	cout << "Hello World" << endl;
	
	StreetStream cam;
	cam.setInputPath("0"); // 0=Kamera - Pfad für Videodatei
	cam.startMonitor();

	return 0;
}