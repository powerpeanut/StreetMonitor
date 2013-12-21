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
	//c erstes Videoframe merken
	int frameNumber = 0;
	namedWindow("First Frame");
	firstFrame.create(stream.getHeight(), stream.getWidth(), CV_BGR2GRAY);

	//*********************************************************************************
	//schwarz/weißes Binärbild erzeugen - Border mittels Slider
	namedWindow("Binary Street Monitor");
	binaryFrame.create(stream.getHeight(), stream.getWidth(), CV_8UC1);
	
	namedWindow("Slider");
	resizeWindow("Slider",700,40);
	createTrackbar("Schwellwert","Slider",0,255);
	setTrackbarPos("Schwellwert","Slider",120); //Startwert des Sliders
	int border = 120;


	//#################################################################################
	//Streams und Berechnungen laufen lassen
	while(waitKey(30) == -1){
		frameNumber++;

		//*****************************************************************************
		//Original Stream holen und anzeigen
		bool success = stream.getInputStream().read(aktFrame);
		if(success == false){
			break;
		}
		imshow("Street Monitor", aktFrame);

		//*****************************************************************************
		//Stream in Greyscale Video umwandeln und darstellen
		cvtColor(aktFrame, greyFrame, CV_BGR2GRAY);
		imshow("Grey Street Monitor", greyFrame);

		//Initialisierung des firstFrame nur bei erstem Aufruf
		//FirstFrame muss ohne Auto aufgenommen werden (reiner Hintergrund der Szene!)
		//evtl. Kalibrierungsmenü einplanen!
		if(frameNumber == 1){
			greyFrame.copyTo(firstFrame);
			imshow("First Frame", firstFrame);
		}

		//*****************************************************************************
		//Stream in Binärvideo umwandeln - später: Schwellwert mittels Slider einstellen
		border = getTrackbarPos("Schwellwert","Slider");

		for(int x=0; x < greyFrame.cols; x++){
			for(int y=0; y < greyFrame.rows; y++){
				uchar pixelGrey = greyFrame.at<uchar>(y,x);
				uchar pixelFirst = firstFrame.at<uchar>(y,x);

				//Pixel, die vom Wert im ersten Frame stark abweichen, werden auf weiß gesetzt, ansonsten schwarz
				//Wenn Abweichung größer 20/border (Graustufen)   //Über Slider kann Schwellwert verändert werden
				if(abs((int)pixelGrey - (int)pixelFirst) > border)
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