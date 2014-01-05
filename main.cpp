#include <iostream>
#include <opencv2\opencv.hpp>
#include <string>

#include "Monitor.h"

using namespace cv;
using namespace std;

int main(){
	cout << "Street Stream Start\n" << endl;

	Monitor mon;
	//mon.process("0"); // 0=Kamera - Pfad für Videodatei

	//mon.process("Micro-dance.avi");
	//mon.process("StreetNils.mp4");

	//## Kamera am besten senkrecht zur Straße ausrichten
	mon.process("Bochum1.avi"); //http://www.youtube.com/watch?v=Jc22qUyuN7k
	//mon.process("Bochum2.avi"); //http://www.youtube.com/watch?v=ZD1zeSSJZ7k

	return 0;
}