#include <iostream>
#include <opencv2\opencv.hpp>
#include <string>

#include "Monitor.h"

using namespace cv;
using namespace std;

int main(){
	cout << "Street Stream Start\n" << endl;

	Monitor mon;
	mon.process("0"); // 0=Kamera - Pfad für Videodatei

	return 0;
}