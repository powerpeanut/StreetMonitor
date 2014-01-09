#include <iostream>
#include <opencv2\opencv.hpp>
#include <string>

#include "Monitor.h"

using namespace cv;
using namespace std;

Monitor mon;

char getChoice();

//#################################################################################
int main(){
	Monitor mon;
	char c = 0;
	string s = "";

	while(c != 'e'){ 
		cout << "######################## Street Stream ########################\n" << endl;

		//Quelle wählen
		cout << "        =============== Quelle aussuchen ================" << endl;
		cout << "\nTaste + Enter: \n"
				"\te - Monitor beenden\n"
				"\t0 - Webcam starten\n"
				"\t1 - Video 1 starten (Dateiname: Monitor1.avi)\n"
				"\t2 - Video 2 starten (Dateiname: Monitor2.avi)\n"
				"\t3 - Video 3 starten (Dateiname: Monitor3.avi)\n";
		cout << "EINGABE: ";

		c = getChoice();

		if(c == 'e'){
			cout << "Street Monitor beendet" << endl;
			return c;
		}

		switch(c){
			case '0':
				cout << "Webcam start...\nLeertaste in Monitor zum Beenden" << endl;
				mon = Monitor();
				mon.process("0"); // 0=Kamera - Pfad für Videodatei
				cout << "***************************************************************\n" << endl;
				break;
			case '1':
			case '2':
			case '3':
				cout << "Monitor" << c << ".avi start...\nLeertaste in Monitor zum Beenden" << endl;
				mon = Monitor();
				s = ""; s = s + c;
				mon.process("Monitor" + s + ".avi");
				cout << "***************************************************************\n" << endl;
				break;
			default:
				;
		}
	}
	return 0;
}

//#################################################################################
//Eingabe abfragen
char getChoice(){
	string st = "";
	cin >> st;
	char* cc = const_cast<char*>(st.c_str());
	return cc[0];
}