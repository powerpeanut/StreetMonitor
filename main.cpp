#include <iostream>
#include <opencv2\opencv.hpp>
#include <string>

#include "Monitor.h"

using namespace cv;
using namespace std;

string source = "nothing";
void getSource();

//#################################################################################
int main(){
	Monitor mon;

	cout << "######################## Street Stream ########################\n" << endl;

	//Quelle wählen
	getSource();

	if(source == "0"){
		mon.process("0"); // 0=Kamera - Pfad für Videodatei
	}
	else if(source == "1"){
		//mon.process("Micro-dance.avi");
		//mon.process("TestNils2.avi");

		//## Kamera sollte am besten senkrecht zur Straße ausgerichtet sein
		mon.process("Bochum1.avi"); //http://www.youtube.com/watch?v=Jc22qUyuN7k
		//mon.process("Bochum2.avi"); //http://www.youtube.com/watch?v=ZD1zeSSJZ7k
	}
	return 0;
}

//#################################################################################
//User Abfrage ob Videodatei oder Webcam gewählt werden soll
void getSource(){
	cout << "=============== QUELLE WAEHLEN ================" << endl;

	while(true){
		cout << "Waehle 0 fuer Webcam oder 1 fuer Videodatei: ";
		cin >> source;
		if(source == "0"){
			break;
		}
		else if(source == "1"){
			source = "1";
			break;
		}
	}
	cout << "gewaehlt: " << source << endl;
	destroyAllWindows();
}





