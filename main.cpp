#include <iostream>
#include "StreetStream.h"
using namespace std;

int main(){
	cout << "Hello World" << endl;
	
	StreetStream cam;
	cam.setInputPath("0");
	cam.startMonitor();
	return 0;
}