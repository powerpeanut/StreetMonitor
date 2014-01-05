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
	void process(const std::string& path);
	void detectMotion();

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
};

#endif