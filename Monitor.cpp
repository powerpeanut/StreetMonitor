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
	frameCount = 0;
	globalCarCount = 0;
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
	createTrackbar("Input","Konfigurator",&confInputStream,1);
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
		confInputStream = getTrackbarPos("Input","Konfigurator");	if(confInputStream == 0) destroyWindow("Input Stream");
		confBackground = getTrackbarPos("Background","Konfigurator");	if(confBackground == 0) destroyWindow("Background");

		//*****************************************************************************
		//Original Stream holen
		if(stream.getInputStream().read(aktFrame) == false){
			break;
		}
		frameCount++;

		//Bilddrehung um 180° wenn Kamera "über Kopf" hängt >> über Slider änderbar
		if(configFlip == 1){
			flip(aktFrame,aktFrame,-1);
		}

		//*****************************************************************************
		//Objekterkennung, Hervorbebung und Zählung
		detectMotion();
		cout << "Autos: " << objCount << " || Konturen: " << contCount << endl;
		
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
	
	//RNG rnd(12345);		//random Number für Farbe der Konturen

	//*********************************************************************************
	//Canny Edge Detection TEST MODE
	//CannyThreshold(0, 0);
	
	//*********************************************************************************
	//Gaussian Background/Foreground Segmentation Berechnungen
	bgSub.operator()(aktFrame,fgMask);	//Aktualisiert die HintergrundMaske und berechnet die Vodergrund Maske
    bgSub.getBackgroundImage(bgFrame);	//gibt HintergrundFrame zurück

	//*********************************************************************************
	//Vodergrundbild bearbeiten und Konturen der Bewegung erkennen und einrahmen
	erode(fgMask,fgMask,Mat());
    dilate(fgMask,fgMask,Mat());
    findContours(fgMask,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);	//CV_RETR_EXTERNAL >> retrieve only extreme outer contours || CV_CHAIN_APPROX_NONE >> detect all pixels of each contour
	contCount = contours.size();			//Anzahl aller erkannten Objekte														// || CV_CHAIN_APPROX_SIMPLE >> compress contours and leaves only contours end points
	
	//Arrays an Anzahl gefundener Objekte anpassen (damit i für alle einheitlich ist)
	contoursPoly.resize(contours.size());
	boundRect.resize(contours.size());

	//gefundene Konturen annähernd in Polygone umwandeln und Grenzen als Rechtecke sichern
	for(int i=0; i < contours.size(); i++){
		approxPolyDP(Mat(contours[i]),contoursPoly[i],3,true);
		boundRect[i] = boundingRect(Mat(contoursPoly[i]));
	}

	//*********************************************************************************
	//Input in Ausgabe Stream kopieren und erkannte Konturen hinzufügen
	aktFrame.copyTo(outputFrame);
	Scalar color = Scalar(0,0,255);																		//Farbe der Objekt-Markierung und Schrift
	objCount = 0;																						//Anzahl der erkannten Fahrzeug-Objekte

	//Objekte als Fahrzeuge erkennen, zeichnen und zählen
	bool work = false;
	for(int i=0; i < boundRect.size(); i++){
		//verarbeitet nur Objekte die breiter 90px / höher 50px  sind (z.B. Autos + LKW) >> über Slider einstellbar!
		if(boundRect[i].size().width > confCarWidth && boundRect[i].size().height > confCarHeight){	
			work = true;

			//Scalar color = Scalar(rnd.uniform(0,255),rnd.uniform(0,255),rnd.uniform(0,255));			//random Farbe für Kontur
			//drawContours(outputFrame,contoursPoly,i,color,1,8,vector<Vec4i>(),0,Point());				//zeichnet originale Kontur -- langsamer: //drawContours(outputFrame,contours,-1,Scalar(255,0,0),2);
			rectangle(outputFrame,boundRect[i].tl(),boundRect[i].br(),color,2,8,0);						//zeichnet Rechteck um Kontur
			objCount++;

			//Centroid Bestimmung des aktuellen Objekts und Fahrzeug Beschriftung (Nummer)
			Point center = Point(boundRect[i].x + (boundRect[i].width / 2), boundRect[i].y + (boundRect[i].height / 2));
			putText(outputFrame,intToString(objCount),center,FONT_HERSHEY_PLAIN,2.0,Scalar(0,0,255),2,8,false);

			//sichtbaren Ausschnitt für aktuell erkanntes Fahrzeug erzeugen
			Mat viewCar;
			aktFrame(boundRect[i]).copyTo(viewCar);

			//Car Objekt erzeugen für neu erkanntes Fahrzeug
			Car newCar = Car(viewCar,center,globalCarCount+1);

			//*********************************************************************************
			//Car Objekte vergleichen und Neues erst in Liste anlegen, wenn es noch nicht vorhanden ist					>> eigene Funktion auslagern?!??!
			if(frameCount < 2){								//ersten Frames müssen noch initialisieren, erst dann wird gezählt
				globalCarCount = 0;							//da erstes Objekt meist das gesamte erste Frame! (Bug..haha)
			}
			else{
				bool found = false;
				for(int c=0; c < cars.size(); c++){
					//cout << "aktuell erfasste Fahrzeuge:" << cars.size() << endl;
					
					//Wenn centroid des gefundenen Fahrzeugs nahe (range) eines vorhandenen Fahrzeugs aus der Liste liegt, ist es wohl dasselbe Fahrzeug ?!
					//range = Centroid innerhalb halber Breite und Höhe der Erkennungsrange
					if(newCar.getCenter().x > cars[c].getCenter().x-(confCarWidth/2) && newCar.getCenter().x < cars[c].getCenter().x+(confCarWidth/2)
							&& newCar.getCenter().y > cars[c].getCenter().y-(confCarHeight/2) && newCar.getCenter().y < cars[c].getCenter().y+(confCarHeight/2)){
						found = true;
						cout << "sameCarNr: " << cars[c].getNumber() << endl;
						cars[c].updateCenter(center);
						cars[c].updateFace(viewCar);
						imshow(intToString(objCount), cars[c].getFace());			//aktuelles Fahrzeug in eigenem Fenster anzeigen
					}
				}
				//kein bekanntes Fahrzeug im aktuellen Frame gefunden, Fahrzeug muss also neu sein
				if(found == false){
					cars.push_back(newCar);
					globalCarCount++;
					cout << "newCarNr: " << newCar.getNumber() << endl;
				}
			}
			newCar.~Car();
		}
	}

	//*********************************************************************************
	//Objekte und Car-Fenster löschen, wenn aktuell kein Fahrzeug erkannt wird
	if(work == false){
		for(int c=0; c <= cars.size(); c++){
			destroyWindow(intToString(c));
		}
		cars.clear();
	}
	
	//Anzeige der aktuellen Anzahl erkannter Fahrzeuge und Gesamtfahrzeuge (links oben im Street Monitor Fenster)
	putText(outputFrame,intToString(objCount),Point(5,50),FONT_HERSHEY_PLAIN,4.0,Scalar(0,0,255),2,8,false);				//Point(outputFrame.cols-45,50) rechts oben, Text ist aber linksbündig, bei größeren Zahlen schwierig... :(
	putText(outputFrame,intToString(globalCarCount),Point(5,100),FONT_HERSHEY_PLAIN,4.0,Scalar(0,0,255),2,8,false);

}

//#####################################################################################
//Wandelt int in String um
std::string Monitor::intToString(int x){
	stringstream ss;
	ss << x; 
	string str = ss.str();	
	return str;
}

//#####################################################################################
//Candy Edge Detector  TEST MODE - erkennt Umrisse
//http://docs.opencv.org/doc/tutorials/imgproc/imgtrans/canny_detector/canny_detector.html
void Monitor::CannyThreshold(int, void*){
	Mat src_gray, detected_edges, out;
	int edgeThresh = 1;
	int lowThreshold = 60;
	int ratio = 3;
	int kernel_size = 3;

	/// akt. Frame in Graustufenbild umwandeln
	cvtColor(aktFrame, src_gray, CV_BGR2GRAY);

	/// Reduce noise with a kernel 3x3
	blur(src_gray, detected_edges, Size(3,3));
	
	/// Canny detector
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
	
	/// Using Canny's output as a mask, we display our result
	out = Scalar::all(0);
	
	aktFrame.copyTo(out, detected_edges);
	imshow("Canny", out);
 }