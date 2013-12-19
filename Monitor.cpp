#include <opencv2\opencv.hpp>
#include <string>

#include "Monitor.h"

using namespace cv;
using namespace std;

Monitor::Monitor(){
	
}

Monitor::~Monitor(){
}

void Monitor::process(const string& path){

	//*********************************************************************************
	//Original Stream
	namedWindow("Street Monitor");
	stream.setInputPath(path);
	stream.startStream();

	//*********************************************************************************
	//Stream in Graustufen
	namedWindow("Grey Street Monitor");
	greyFrame.create(stream.getHeight(), stream.getWidth(), CV_BGR2GRAY);

	//*********************************************************************************
	//schwarz/weiﬂes Bin‰rbild erzeugen - Border mittels Slider
	namedWindow("Binary Street Monitor");
	binaryFrame.create(stream.getHeight(), stream.getWidth(), CV_8UC1);
	
	namedWindow("Slider");
	resizeWindow("Slider",700,40);
	createTrackbar("Schwellwert","Slider",0,255);
	setTrackbarPos("Schwellwert","Slider",20); //Startwert des Sliders
	int border = 20;


	//#################################################################################
	//Streams und Berechnungen laufen lassen
	bool first = true;
	while(waitKey(30) == -1){
		//letzten Frame sichern
		if(first == false)
			lastFrame = aktFrame;

		//*****************************************************************************
		//Original Stream holen und anzeigen
		bool success = stream.getInputStream().read(aktFrame);
		if(success == false)
			break;
		imshow("Street Monitor", aktFrame);

		//*****************************************************************************
		//Initialisierung des lastFrame nur bei erstem Aufruf
		if(first == true){
			lastFrame = aktFrame;
			first = false;
		}

		//*****************************************************************************
		//Stream in Greyscale Video umwandeln und darstellen
		cvtColor(aktFrame, greyFrame, CV_BGR2GRAY);
		imshow("Grey Street Monitor", greyFrame);

		//*****************************************************************************
		//Stream in Bin‰rvideo umwandeln - sp‰ter: Schwellwert mittels Slider einstellen
		border = getTrackbarPos("Schwellwert","Slider");

		for(int x=0; x < greyFrame.cols; x++){
			for(int y=0; y < greyFrame.rows; y++){
				uchar pixelGrey = greyFrame.at<uchar>(y,x);
				uchar pixelLast = lastFrame.at<uchar>(y,x);

				//Pixel, die vom Wert im ersten Frame stark abweichen, werden auf weiﬂ gesetzt, ansonsten schwarz
				//Wenn Abweichung grˆﬂer 20/border (Graustufen)
				//‹ber Slider kann Schwellwert ver‰ndert werden
				if(abs((int)pixelGrey - (int)pixelLast) > border)
					binaryFrame.at<uchar>(y,x) = (uchar) 255;
				else
					binaryFrame.at<uchar>(y,x) = (uchar) 0;
			}
		}
		imshow("Binary Street Monitor", binaryFrame);

		//Bewegungs- und Objekterkennung
		detectMotion();

	}
	destroyAllWindows();
}

void Monitor::detectMotion(){
	//some code here um frames zu vergleichen und Bewegung zu erkennen
}