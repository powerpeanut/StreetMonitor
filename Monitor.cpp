#include "Monitor.h"

using namespace cv;
using namespace std;

Monitor::Monitor(){
}

Monitor::~Monitor(){
}

void Monitor::process(const string& path){

	bool flip = true; //wenn Kamera "über Kopf" gedreht >> später ins Config Menü!?

	//*********************************************************************************
	//Original Stream
	namedWindow("Input Stream");
	stream.setInputPath(path);
	stream.startStream();

	//*********************************************************************************
	//Hintergrund Frame vorbereiten
	namedWindow("Background");
	bgFrame.create(stream.getHeight(), stream.getWidth(), CV_8UC3);

	//*********************************************************************************
	//Output Street Monitor vorbereiten
	namedWindow("Street Monitor");
	outputFrame.create(stream.getHeight(), stream.getWidth(), CV_8UC3);

	//*********************************************************************************
	//Gaussian Background/Foreground Segmentation vorbereiten
	bgSub.set("nmixtures",3);			//set number of gaussian mixtures
	bgSub.set("detectShadows",false);	//turn the shadow detection off


	//#################################################################################
	//Streams und Berechnungen laufen lassen
	while(waitKey(30) == -1){

		//*****************************************************************************
		//Original Stream holen und anzeigen
		if(stream.getInputStream().read(aktFrame) == false){
			break;
		}
		//Bilddrehung um 180° wenn Kamera "über Kopf" hängt
		if(flip){
			cv::flip(aktFrame,aktFrame,-1);
		}
		imshow("Input Stream", aktFrame);

		//*****************************************************************************
		//Gaussian Background/Foreground Segmentation Berechnungen
		bgSub.operator()(aktFrame,fgMask);	//Aktualisiert die HintergrundMaske und berechnet die Vodergrund Maske
        bgSub.getBackgroundImage(bgFrame);	//gibt HintergrundFrame zurück

		//*****************************************************************************
		//Vodergrundbild bearbeiten und Umrisse erkennen
		erode(fgMask,fgMask,Mat());
        dilate(fgMask,fgMask,Mat());
        findContours(fgMask,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);	//CV_RETR_EXTERNAL >> retrieve only external contours || CV_CHAIN_APPROX_NONE >> detect all pixels of each contour

		//Background in Ausgabe Stream kopieren und Konturen hinzufügen
        aktFrame.copyTo(outputFrame);
        drawContours(outputFrame,contours,-1,Scalar(255,0,0),2);
		
		//*****************************************************************************
		//Anzeige der Frames	
		imshow("Background", bgFrame);
		imshow("Street Monitor", outputFrame);

		//Bewegungs- und Objekterkennung
		detectMotion();

	}
	destroyAllWindows();
}

void Monitor::detectMotion(){
	//some code here um frames zu vergleichen und Bewegung zu erkennen
}