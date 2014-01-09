#ifndef CAR_H__
#define CAR_H__

#include <opencv2\opencv.hpp>
#include <string>

class Car{

public:
	Car();
	Car(cv::Mat startFace, cv::Point center, int number);
	~Car(void);
	void updateCenter(cv::Point center);
	void updateFace(cv::Mat face);
	cv::Point getCenter();
	cv::Mat getFace();
	int getNumber();

private:
	cv::Mat face;						//Ansicht
	cv::Point center;					//Centroid
	int number;

};

#endif