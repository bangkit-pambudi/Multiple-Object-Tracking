#ifndef OBJEK_H
#define OBJEK_H
#include <string>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class Object
{
public:
	Object(){
	//set values for default constructor
	setType("Object");
	setColor(Scalar(0,0,0));
    }
	~Object(void){
	}

	Object(string name){

	setType(name);

	if(name=="blue"){

		//TODO: use "calibration mode" to find HSV min
		//and HSV max values

		setHSVmin(Scalar(94,109,20));
		setHSVmax(Scalar(112,255,255));

		//BGR value for Blue:
		//setColor(Scalar(255,0,0));

	}
	if(name=="green"){

		//TODO: use "calibration mode" to find HSV min
		//and HSV max values

		setHSVmin(Scalar(25,60,70));
		setHSVmax(Scalar(97,255,255));

		//BGR value for Green:
		setColor(Scalar(0,255,0));

	}

	if(name=="red"){

		//TODO: use "calibration mode" to find HSV min
		//and HSV max values

		setHSVmin(Scalar(120,140,50));
		setHSVmax(Scalar(180,255,255));

		//BGR value for Red:
		setColor(Scalar(0,0,255));

	}
}

int getXPos(){

	return xPos;

}

void setXPos(int x){

	xPos = x;

}

int getYPos(){

	return yPos;

}

void setYPos(int y){

	yPos = y;

}

Scalar getHSVmin(){

	return HSVmin;

}
Scalar getHSVmax(){

	return HSVmax;
}

void setHSVmin(Scalar min){

	HSVmin = min;
}

void setHSVmax(Scalar max){

	HSVmax = max;
}

	string getType(){return type;}
	void setType(string t){type = t;}

	Scalar getColor(){
		return Color;
	}
	void setColor(Scalar c){

		Color = c;
	}

private:

	int xPos, yPos;
	string type;
	Scalar HSVmin, HSVmax;
	Scalar Color;
};
#endif // OBJEK_H
