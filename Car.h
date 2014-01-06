#ifndef CAR_H__
#define CAR_H__

#include <opencv2\opencv.hpp>
#include <string>

class Car{

public:
	Car(cv::Mat face);
	~Car(void);

private:
	cv::Mat face;

};

#endif