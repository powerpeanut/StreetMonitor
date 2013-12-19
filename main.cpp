#include <iostream>
#include "StreetStream.h"
using namespace std;

#include <opencv2\opencv.hpp>
#include <string>
using namespace cv;

int main(){
	cout << "Hello World" << endl;
	
	StreetStream cam;
	cam.setInputPath("0");
	cam.startMonitor();

	return 0;
}