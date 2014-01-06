#include "Monitor.h"

using namespace cv;
using namespace std;

Monitor::Monitor(){
}

Monitor::~Monitor(){
}

//#####################################################################################
//verarbeitet InputStream + Ausgabe
void Monitor::process(const string& path){

	//*********************************************************************************
	//Standard Werte voreinstellen
	if(path == "0")		configFlip = 1;		//Webcam Stream wird "über Kopf" gedreht
	else				configFlip = 0;
	confCarWidth = 90;
	confCarHeight= 50;
	confInputStream = 1;
	confBackground = 1;
	
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

	//*********************************************************************************
	//Config Menü erstellen
	namedWindow("Konfigurator");
	resizeWindow("Konfigurator",250,220);
	createTrackbar("InputFlip","Konfigurator",&configFlip,1);		//flip InputStream
	createTrackbar("InputStream","Konfigurator",&confInputStream,1);
	createTrackbar("Background","Konfigurator",&confBackground,1);
	createTrackbar("CarWidth","Konfigurator",&confCarWidth,200);
	createTrackbar("CarHeight","Konfigurator",&confCarHeight,200);

	//#################################################################################
	//Streams und Berechnungen laufen lassen
	while(waitKey(30) == -1){

		//*****************************************************************************
		//Trackbars aus Config Menü auslesen
		configFlip = getTrackbarPos("InputFlip","Konfigurator");
		confCarWidth = getTrackbarPos("CarWidth","Konfigurator");
		confCarHeight = getTrackbarPos("CarHeight","Konfigurator");
		confInputStream = getTrackbarPos("InputStream","Konfigurator");	if(confInputStream == 0) destroyWindow("Input Stream");
		confBackground = getTrackbarPos("Background","Konfigurator");	if(confBackground == 0) destroyWindow("Background");

		//*****************************************************************************
		//Original Stream holen
		if(stream.getInputStream().read(aktFrame) == false){
			break;
		}
		//Bilddrehung um 180° wenn Kamera "über Kopf" hängt >> über Slider änderbar
		if(configFlip == 1){
			flip(aktFrame,aktFrame,-1);
		}

		//*****************************************************************************
		//Objekterkennung, Hervorbebung und Zählung
		detectMotion();
		cout << "Autos: " << objCount << " || Konturen: " << countCont << endl;
		
		//*****************************************************************************
		//Anzeige der Frames
		if(confInputStream == 1) imshow("Input Stream", aktFrame);
		if(confBackground == 1)	imshow("Background", bgFrame);
		imshow("Street Monitor", outputFrame);
	}
	destroyAllWindows();
}


//#####################################################################################
//Motion Detection + Zählung
void Monitor::detectMotion(){
	
	RNG rng(12345);		//random Number für Farbe der Konturen
	
	//*********************************************************************************
	//Gaussian Background/Foreground Segmentation Berechnungen
	bgSub.operator()(aktFrame,fgMask);	//Aktualisiert die HintergrundMaske und berechnet die Vodergrund Maske
    bgSub.getBackgroundImage(bgFrame);	//gibt HintergrundFrame zurück

	//*********************************************************************************
	//Vodergrundbild bearbeiten und Konturen der Bewegung erkennen und einrahmen
	erode(fgMask,fgMask,Mat());
    dilate(fgMask,fgMask,Mat());
    findContours(fgMask,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);	//CV_RETR_EXTERNAL >> retrieve only extreme outer contours || CV_CHAIN_APPROX_NONE >> detect all pixels of each contour
	countCont = contours.size();																									// || CV_CHAIN_APPROX_SIMPLE >> compress contours and leaves only contours end points
	
	//gefundene Konturen annähernd in Polygone umwandeln und Grenzen als Rechtecke sichern
	contoursPoly.resize(contours.size());
	boundRect.resize(contours.size());
	for(int i=0; i < contours.size(); i++){
		approxPolyDP(Mat(contours[i]),contoursPoly[i],3,true);
		boundRect[i] = boundingRect(Mat(contoursPoly[i]));
	}

	//Input in Ausgabe Stream kopieren und erkannte Konturen hinzufügen
	aktFrame.copyTo(outputFrame);
	Scalar color = Scalar(0,0,255);
	objCount = 0;
	for(int i=0; i < boundRect.size(); i++){
		if(boundRect[i].size().width > confCarWidth && boundRect[i].size().height > confCarHeight){		//Zeichnet nur Objekte die breiter 90px / höher 50px  sind (z.B. Autos + LKW) >> über Slider änderbar
			//Scalar color = Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));			//random Farbe für Kontur
			//drawContours(outputFrame,contoursPoly,i,color,1,8,vector<Vec4i>(),0,Point());				//zeichnet originale Kontur -- langsamer: //drawContours(outputFrame,contours,-1,Scalar(255,0,0),2);
			rectangle(outputFrame,boundRect[i].tl(),boundRect[i].br(),color,2,8,0);						//zeichnet Rechteck um Kontur
			objCount++;

			//Fahrzeug Beschriftung (Nummer)
			Point center = Point(boundRect[i].x + (boundRect[i].width / 2), boundRect[i].y + (boundRect[i].height / 2));
			putText(outputFrame,intToString(objCount),center,FONT_HERSHEY_PLAIN,2.0,Scalar(0,0,255),2,8,false);
		}
	}
	
	//Anzeige für aktuelle Anzahl erkannter Fahrzeuge
	putText(outputFrame,intToString(objCount),Point(outputFrame.cols-45,50),FONT_HERSHEY_PLAIN,4.0,Scalar(0,0,255),2,8,false);
}

//#####################################################################################
//Wandelt int in String um
std::string Monitor::intToString(int x){
	stringstream ss;
	ss << x; 
	string str = ss.str();	
	return str;
}