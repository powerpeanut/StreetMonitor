#ifndef MONITOR_H__
#define MONITOR_H__

#include <opencv2\opencv.hpp>
#include <string>
#include <vector>

#include "StreetStream.h"

class Monitor{

public:
	Monitor(void);
	~Monitor(void);
	void process(const std::string& path);	//verarbeitet InputStream + Ausgabe

private:
	StreetStream stream;				//reiner InputStream
	cv::Mat aktFrame;					//reiner InputFrame
	cv::Mat bgFrame;					//errechneter HintergrundFrame
	cv::Mat fgMask;						//Vodergrund Maske
	cv::Mat outputFrame;				//verarbeitetes AusgabeFrame
    cv::BackgroundSubtractorMOG2 bgSub; //Gaussian Mixture-based Background/Foreground Segmentation Algorithm aus openCV

	//Vektor mit Vektoren von Points für Konturen der erkannten Objekte >> Eine Kontur = Vektor der Punkte; mehrere = Vektor mit Vektoren der Punkte (quasi Array)
	std::vector<std::vector<cv::Point> > contours;
	std::vector<std::vector<cv::Point> > contoursPoly;
	std::vector<cv::Rect> boundRect;
	int countCont, objCount;

	void detectMotion();				//Motion Detection
	std::string intToString(int x);		//int to String

	int configFlip;						//für Bilddrehung (0 aus, 1 ein)
	int confCarWidth, confCarHeight;	//zu erkennende Fahrzeugbreite und -höhe
	int confInputStream, confBackground;//MatObjekte de/aktivieren
};

#endif