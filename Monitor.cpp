#include "Monitor.h"

using namespace cv;
using namespace std;

Monitor::Monitor(){
}

Monitor::~Monitor(){
}

//#####################################################################################
//#####################################################################################
//verarbeitet InputStream + Ausgabe
void Monitor::process(const string& path){

	//*********************************************************************************
	//Standard Werte voreinstellen
	frameCount = 0;
	globalCarCount = 0;
	if(path == "0")		configFlip = 1;		//Webcam Stream wird "�ber Kopf" gedreht
	else				configFlip = 0;
	confDetect = 1;
	resetCount = 0;
	confCarWidth = 120;
	confCarHeight= 50;
	confInputStream = 0;
	confBackground = 0;
	
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
	//Config Men� erstellen
	createConfMenue();

	//#################################################################################
	//Streams und Berechnungen laufen lassen
	while(waitKey(30) == -1){

		//*****************************************************************************
		//Trackbars aus Config Men� auslesen
		checkConfMenue();
		
		//*****************************************************************************
		//Original Stream holen
		if(stream.getInputStream().read(aktFrame) == false){
			break;
		}
		frameCount++;

		//Bilddrehung um 180� wenn Kamera "�ber Kopf" h�ngt >> �ber Slider �nderbar
		if(configFlip == 1){
			flip(aktFrame,aktFrame,-1);
		}

		//*****************************************************************************
		//Objekterkennung, Hervorbebung und Z�hlung
		if(confDetect == 1){
			detectMotion();
			cout << "Autos: " << objCount << " || Konturen: " << contCount << endl;
		} else {														//wenn Detection off, zeige direkt InputStream - Fenster f�r Detection werden nicht mehr angezeigt
			confInputStream = 0; confBackground = 0;
			destroyWindow("Input Stream"); destroyWindow("Background");
			aktFrame.copyTo(outputFrame);
		}
		
		//*****************************************************************************
		//Anzeige der Monitor Frames und Verarbeitungsfenster
		if(confInputStream == 1) imshow("Input Stream", aktFrame);
		if(confBackground == 1)	imshow("Background", bgFrame);
		imshow("Street Monitor", outputFrame);
	}
	destroyAllWindows();
}

//#####################################################################################
//#####################################################################################
//Motion Detection + Z�hlung
void Monitor::detectMotion(){

	//*********************************************************************************
	//Canny Edge Detection TEST MODE
	//CannyThreshold(0, 0);
	
	//*********************************************************************************
	//Gaussian Background/Foreground Segmentation Berechnungen
	bgSub.operator()(aktFrame,fgMask);	//Aktualisiert die HintergrundMaske und berechnet die Vodergrund Maske
    bgSub.getBackgroundImage(bgFrame);	//gibt HintergrundFrame zur�ck

	//*********************************************************************************
	//Vodergrundbild bearbeiten und Konturen der Bewegung erkennen und einrahmen
	erode(fgMask,fgMask,Mat());
    dilate(fgMask,fgMask,Mat());
    findContours(fgMask,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);	//CV_RETR_EXTERNAL >> retrieve only extreme outer contours || CV_CHAIN_APPROX_NONE >> detect all pixels of each contour
	contCount = contours.size();			//Anzahl aller erkannten Objekte														// || CV_CHAIN_APPROX_SIMPLE >> compress contours and leaves only contours end points
	
	//Arrays an Anzahl gefundener Objekte anpassen (damit i f�r alle einheitlich ist)
	contoursPoly.resize(contours.size());
	boundRect.resize(contours.size());

	//gefundene Konturen ann�hernd in Polygone umwandeln und Grenzen als Rechtecke sichern
	for(int i=0; i < contours.size(); i++){
		approxPolyDP(Mat(contours[i]),contoursPoly[i],3,true);
		boundRect[i] = boundingRect(Mat(contoursPoly[i]));
	}

	//*********************************************************************************
	//Input in Ausgabe Stream kopieren und erkannte Konturen hinzuf�gen
	aktFrame.copyTo(outputFrame);
	Scalar color = Scalar(0,0,255);																		//Farbe der Objekt-Markierung und Schrift
	objCount = 0;																						//Anzahl der erkannten Fahrzeug-Objekte

	//Objekte als Fahrzeuge erkennen, zeichnen und z�hlen
	bool work = false;
	for(int i=0; i < boundRect.size(); i++){
		//verarbeitet nur Objekte die breiter 120px / h�her 50px  sind (z.B. Autos + LKW) >> �ber Slider einstellbar!
		if(boundRect[i].size().width > confCarWidth && boundRect[i].size().height > confCarHeight){	
			work = true;

			//drawContours(outputFrame,contoursPoly,i,color,1,8,vector<Vec4i>(),0,Point());				//zeichnet originale Kontur -- langsamer: //drawContours(outputFrame,contours,-1,Scalar(255,0,0),2);
			rectangle(outputFrame,boundRect[i].tl(),boundRect[i].br(),color,2,8,0);						//zeichnet Rechteck um Kontur
			objCount++;

			//Centroid Bestimmung des aktuellen Objekts und Fahrzeug Beschriftung (Nummer)
			Point center = Point(boundRect[i].x + (boundRect[i].width / 2), boundRect[i].y + (boundRect[i].height / 2));
			putText(outputFrame,intToString(objCount),center,FONT_HERSHEY_PLAIN,2.0,Scalar(0,0,255),2,8,false);

			//sichtbaren Ausschnitt f�r aktuell erkanntes Fahrzeug erzeugen
			Mat viewCar;
			aktFrame(boundRect[i]).copyTo(viewCar);

			//*********************************************************************************
			//Car Objekt erzeugen f�r neu erkanntes Fahrzeug
			Car newCar = Car(viewCar,center,globalCarCount+1);

			//Car Objekte vergleichen und newCar nur in Liste anlegen, wenn es noch nicht vorhanden ist
			checkCar(newCar);
			newCar.~Car();
		}
	}

	//*********************************************************************************
	//Objekte und Car-Fenster l�schen, wenn aktuell kein Fahrzeug erkannt wird
	if(work == false){
		for(int c=0; c <= cars.size(); c++){
			destroyWindow(intToString(c));
		}
		cars.clear();
	}
	
	//Anzeige der aktuellen Anzahl erkannter Fahrzeuge und Gesamtfahrzeuge (links oben im Street Monitor Fenster)
	putText(outputFrame,intToString(objCount),Point(5,50),FONT_HERSHEY_PLAIN,4.0,Scalar(0,0,255),2,8,false);				//Point(outputFrame.cols-45,50) rechts oben, Text ist aber linksb�ndig, bei gr��eren Zahlen schwierig... :(
	putText(outputFrame,intToString(globalCarCount),Point(5,100),FONT_HERSHEY_PLAIN,4.0,Scalar(0,0,255),2,8,false);

}

//#####################################################################################
//#####################################################################################
//erzeugt das Konfigurationsmen�
void Monitor::createConfMenue(){
	namedWindow("Konfigurator");
	resizeWindow("Konfigurator",260,310);
	createTrackbar("InputFlip","Konfigurator",&configFlip,1);		//flip InputStream
	createTrackbar("Detection","Konfigurator",&confDetect,1);
	createTrackbar("ResetCount","Konfigurator",&resetCount,1);
	createTrackbar("Input","Konfigurator",&confInputStream,1);
	createTrackbar("Background","Konfigurator",&confBackground,1);
	createTrackbar("CarWidth","Konfigurator",&confCarWidth,300);
	createTrackbar("CarHeight","Konfigurator",&confCarHeight,200);
}

//#####################################################################################
//#####################################################################################
//fragt Slider des Konfigurationsmen�s ab
void Monitor::checkConfMenue(){
	configFlip = getTrackbarPos("InputFlip","Konfigurator");
	confDetect = getTrackbarPos("Detection","Konfigurator");
	resetCount = getTrackbarPos("ResetCount","Konfigurator"); if(resetCount == 1) globalCarCount = 0;
	confCarWidth = getTrackbarPos("CarWidth","Konfigurator");
	confCarHeight = getTrackbarPos("CarHeight","Konfigurator");
	confInputStream = getTrackbarPos("Input","Konfigurator");	if(confInputStream == 0) destroyWindow("Input Stream");
	confBackground = getTrackbarPos("Background","Konfigurator");	if(confBackground == 0) destroyWindow("Background");
}

//#####################################################################################
//#####################################################################################
//vergleicht neues Car Objekt mit Liste vorhandener Car Objekte
//wenn newCar bereits vorhanden, aktualisiere Centroid & Face
//wenn newCar nicht vorhanden, f�ge es der Liste hinzu
void Monitor::checkCar(Car newCar){
	if(frameCount < 3){								//ersten Frames m�ssen noch initialisieren, erst dann wird gez�hlt
		globalCarCount = 0;							//da erstes Objekt meist das gesamte erste Frame! (Bug..haha)
	}
	else{
		bool found = false;
		for(int c=0; c < cars.size(); c++){
			//cout << "aktuell erfasste Fahrzeuge:" << cars.size() << endl;
			
			//Wenn centroid des gefundenen Fahrzeugs nahe (range) eines vorhandenen Fahrzeugs aus der Liste liegt, ist es wohl dasselbe Fahrzeug ?!
			//range = Centroid innerhalb halber Breite und H�he der Erkennungsrange
			if(newCar.getCenter().x > cars[c].getCenter().x-(confCarWidth/2) && newCar.getCenter().x < cars[c].getCenter().x+(confCarWidth/2)
					&& newCar.getCenter().y > cars[c].getCenter().y-(confCarHeight/2) && newCar.getCenter().y < cars[c].getCenter().y+(confCarHeight/2)){
				found = true;
				cout << "sameCarNr: " << cars[c].getNumber() << endl;
				cars[c].updateCenter(newCar.getCenter());
				cars[c].updateFace(newCar.getFace());
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
}

//#####################################################################################
//#####################################################################################
//Wandelt int in String um
std::string Monitor::intToString(int x){
	stringstream ss;
	ss << x; 
	string str = ss.str();	
	return str;
}