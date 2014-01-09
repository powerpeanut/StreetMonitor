#include <iostream>
#include <opencv2\opencv.hpp>
#include <string>

#include "Monitor.h"

using namespace cv;
using namespace std;

string getSource();

//#################################################################################
int main(){
	Monitor mon;
	string source = "nothing";

	///// SWITCH CASE BAUM (if else wegen string) um unterschiedliche Videos zu w�hlen ...
	//evtl noch video per pfad w�hlen lassen (fenster??)
	//nach video ende oder abbruch des monitos hierhin zur�ckkehren!!!
	//dabei alle alten fenster schlie�en!!
	while(source != "ende"){    
		cout << "######################## Street Stream ########################\n" << endl;

		//Quelle w�hlen
		cout << "=============== QUELLE WAEHLEN ================" << endl;
		source = getSource();

		//Quelle starten
		if(source == "0"){
			mon.process("0"); // 0=Kamera - Pfad f�r Videodatei
		}
		else if(source == "1"){
			//mon.process("Micro-dance.avi");
			//mon.process("TestNils2.avi");

			//## Kamera sollte am besten senkrecht zur Stra�e ausgerichtet sein
			mon.process("Bochum1.avi"); //http://www.youtube.com/watch?v=Jc22qUyuN7k
			//mon.process("Bochum2.avi"); //http://www.youtube.com/watch?v=ZD1zeSSJZ7k
		}
		else if(source == "ende"){
			break;
		}
	}
	return 0;
}

//#################################################################################
//User Abfrage, beendet erst, wenn g�ltige eingabe erkannt
//ob Videodatei oder Webcam gew�hlt werden soll (ende = exit)
string getSource(){
	string source = "nothing";

	while(true){
		cout << "Waehle 0 fuer Webcam oder 1 fuer Videodatei: ";
		cin >> source;
		if(source == "0"){
			break;
		}
		else if(source == "1"){
			break;
		}
		else if(source == "ende"){
			break;
		}
	}
	cout << "gewaehlt: " << source << endl;
	destroyAllWindows();
	return source;
}





