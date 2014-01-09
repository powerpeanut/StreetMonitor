#include "Car.h"

using namespace cv;
using namespace std;

Car::Car(){

}

Car::Car(Mat startFace, Point center, int number){
	Car::face = startFace;
	Car::center = center;
	Car::number = number;

	//cout << "new car" << endl;
}

Car::~Car(){
}

//Centroid updaten
void Car::updateCenter(Point center){
	Car::center = center;
}

Point Car::getCenter(){
	return Car::center;
}

//Ansicht aktualisieren
void Car::updateFace(Mat face){
	Car::face = face;
}

Mat Car::getFace(){
	return Car::face;
}

int Car::getNumber(){
	return Car::number;
}