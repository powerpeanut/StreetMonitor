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

	bool flip = false; //wenn Kamera "über Kopf" gedreht >> später ins Config Menü!?

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
		//Objekterkennung, Hervorbebung und Zählung
		detectMotion();
		cout << "Autos: " << objCount << " || Konturen: " << countCont << endl;
		
		//*****************************************************************************
		//Anzeige der Frames	
		imshow("Background", bgFrame);
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
		if(boundRect[i].size().width > 100 && boundRect[i].size().height > 50){					//Zeichnet nur Objekte die breiter 60px / höher 50px  sind (z.B. Autos + LKW) >> ConfigMode? 
			//Scalar color = Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));	//random Farbe für Kontur
			//drawContours(outputFrame,contoursPoly,i,color,1,8,vector<Vec4i>(),0,Point());		//zeichnet originale Kontur -- langsamer: //drawContours(outputFrame,contours,-1,Scalar(255,0,0),2);
			rectangle(outputFrame,boundRect[i].tl(),boundRect[i].br(),color,2,8,0);				//zeichnet Rechteck um Kontur
			objCount++;
		}
	}
	
	//Anzeige für aktuell Anzahl erkannter Fahrzeuge
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