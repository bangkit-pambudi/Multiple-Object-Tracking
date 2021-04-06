#include <iostream>
#include <sstream>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <vector>
#include "objek.h"

int iLowH = 0;
int iHighH = 255;
int iLowS = 0;
int iHighS = 255;
int iLowV = 0;
int iHighV = 255;

void Control();
void drawObject(vector<Object> theObjects,Mat &frame, Mat &temp, vector< vector<Point> > contours, vector<Vec4i> hierarchy);
void drawObject(vector<Object> theObjects,Mat &frame);
void Morphological(Mat &thresh);
void MaskingBlue(Mat &frame, Mat &hsv, Mat &thresh);
void MaskingRed (Mat &frame, Mat &hsv, Mat &thresh);
void MaskingGreen (Mat &frame, Mat &hsv, Mat &thresh);
void trackFilteredObject(Mat threshold,Mat HSV, Mat &cameraFeed);
void trackFilteredObject(Object theObject,Mat threshold,Mat HSV, Mat &cameraFeed);

using namespace std;

Mat frame, frame_gray,frame_2;
Mat frameLAB, frameHSV;
Mat Thresh;
Mat dst, detected_edges;
Mat mask, masked, maskedb, maskedr, maskerg;

int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
const char* window_name = "Edge Map";
int edgeThresh = 1;
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const int MIN_OBJECT_AREA = 20*20;
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
const int MAX_NUM_OBJECTS=50;

string intToString(int number){

    std::stringstream ss;
    ss << number;
    return ss.str();
}

void drawCircle (Object theObject,Mat threshold,Mat HSV, Mat &cameraFeed)
{
    int a=0;

	vector <Vec3f> circles1;
    cv::HoughCircles(threshold, circles1, HOUGH_GRADIENT, 2, threshold.rows/4,100,50,10,800 );
    for( size_t i = 0; i < circles1.size(); i++ )
    {
        a++;
        Vec3i c1 = circles1[i];
        Point center1 = Point(c1[0], c1[1]);
        // circle center
        circle( cameraFeed, center1,5,theObject.getColor());
        // circle outline
        int radius1 = c1[2];
        circle( cameraFeed, center1, radius1,theObject.getColor(),3,LINE_AA);
        putText(cameraFeed,intToString(circles1.size()),Point(540,15),5,0.5,theObject.getColor(),1);
        putText(cameraFeed,intToString(a),Point(c1[0]+9,c1[1]),5,0.5,theObject.getColor(),1);
        putText(cameraFeed,"r",Point(c1[0]+9,c1[1]+10),5,0.5,theObject.getColor(),1);
        putText(cameraFeed,intToString(radius1),Point(c1[0]+10,c1[1]+10),5,0.5,theObject.getColor(),1);
        putText(cameraFeed,"L",Point(c1[0]+9,c1[1]+20),5,0.5,theObject.getColor(),1);
        putText(cameraFeed,intToString((22/7)*radius1*radius1),Point(c1[0]+10,c1[1]+20),5,0.5,theObject.getColor(),1);
        putText(cameraFeed,"K",Point(c1[0]+9,c1[1]+30),5,0.5,theObject.getColor(),1);
        putText(cameraFeed,intToString((22/7)*2*radius1),Point(c1[0]+10,c1[1]+30),5,0.5,theObject.getColor(),1);
    }

    putText(cameraFeed,"RedCircle : ",Point(420,15),5,0.5,Scalar(0,0,255),1);
    putText(cameraFeed,"GreenCircle : ",Point(100,15),5,0.5,Scalar(0,255,0),1);
    putText(cameraFeed,"BlueCircle : ",Point(260,15),5,0.5,Scalar(250,0,0),1);
}


int main(int argc, char* argv[]) {
	//bool calibrationMode = true;

/*	if(calibrationMode){
		//create slider bars for HSV filtering
		createTrackbars();
	}*/

    VideoCapture cap(0);
	if (!cap.isOpened()) {
		cout << "Gagal membuka kamera !" << endl;
		return -1;
    }

	cap.set(CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	cap.set(CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	waitKey(1000);

	//Control();
	while (1) {

		bool bSuccess = cap.read(frame);

		if (!bSuccess) {
			cout << "Gagal membaca frame" << endl;
			break;
		}

		cap.read(frame);
		cap.read(frame_2);


			cvtColor(frame,frameHSV,COLOR_BGR2HSV);
			//GaussianBlur( frame, frameHSV, Size( 15, 15 ), 0, 0 );

			//inRange(frameHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), Thresh);

			//imshow("Tresh",Thresh);
            MaskingBlue(frame, mask, maskedb);
            MaskingRed(frame, mask, maskedr);
            MaskingGreen(frame, mask, maskerg);


            //bitwise_and(maskedr, maskedb, masked);
	  		//createTrackbar( "Min imgThresholded :", window_name, &lowThreshold, max_lowThreshold);

			//trackFilteredObject(imgThresholded,frameHSV,frame);
			//imshow("Original",frame);

            //imshow("Masked", maskedb);
            //imshow("Masked2", maskedr);
            //imshow("Masked3", maskerg);
		//show frames
		imshow("HASIL",frame);
		imshow("Det",frame_2);

		//imshow("Original", frame);
		//imshow("Tresh", imgThresholded);
		waitKey(25);
     }
     return 0;
}

void Control(){
	//create window for trackbars
    namedWindow("Control", WINDOW_AUTOSIZE); //create a window called "Control"

    createTrackbar("LowH", "Control", &iLowH, 255); //Hue (0 - 179)
    createTrackbar("HighH", "Control", &iHighH, 255);

    createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
    createTrackbar("HighS", "Control", &iHighS, 255);

    createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
    createTrackbar("HighV", "Control", &iHighV, 255);
}
void MaskingBlue(Mat &frame, Mat &hsv, Mat &thresh){
    Mat blu;
    Object blue("blue");
	//first find blue objects
	//GaussianBlur(frame, hsv, Size(15, 15), 1, 1);
	cvtColor(hsv, hsv, COLOR_BGR2HSV);
	inRange(hsv, blue.getHSVmin(), blue.getHSVmax(),thresh);
	//bitwise_and(frame, frame, thresh, blu=thresh);
	Morphological(thresh);
	//trackFilteredObject(blue,thresh,frameHSV,frame);
}
void MaskingRed (Mat &frame, Mat &hsv, Mat &thresh){
    //then reds
    Mat rd;
    Object red("red");
    //GaussianBlur(frame, hsv, Size(15, 15), 1, 1);
	cvtColor(hsv,hsv,COLOR_BGR2HSV);
	inRange(hsv,red.getHSVmin(),red.getHSVmax(),thresh);
	//bitwise_and(frame, frame, thresh, rd=thresh);
	Morphological(thresh);
	//trackFilteredObject(red,thresh,frameHSV,frame);
}
void MaskingGreen (Mat &frame, Mat hsv, Mat thresh){
    Object green("green");
    Mat grn;
    //then greens
    //GaussianBlur(frame, hsv, Size(15, 15), 1, 1);
	cvtColor(hsv,hsv,COLOR_BGR2HSV);
	inRange(hsv,green.getHSVmin(),green.getHSVmax(),thresh);;
	//bitwise_and(frame, frame, thresh, grn=thresh);
	Morphological(thresh);
    drawCircle(green,thresh,hsv,frame);
	//trackFilteredObject(green,thresh,frameHSV,frame);
}
void Morphological(Mat &thresh){

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(5,5));
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(5,5));

	medianBlur(thresh,thresh,15);

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);

	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
}
/*
void drawObject(vector<Object> theObjects,Mat &frame, Mat &temp, vector< vector<Point> > contours, vector<Vec4i> hierarchy){

    vector<Object>::iterator obs;
    obs = theObjects.begin();
	for(obs!=theObjects.end();;){
        int i = 0;
        drawContours(frame,contours,i,theObjects.at(i).getColor(),3,8,hierarchy);
        circle(frame,Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()),5,theObjects.at(i).getColor());
        putText(frame,intToString(theObjects.at(i).getXPos())+ " , " + intToString(theObjects.at(i).getYPos()),cv::Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()+20),1,1,theObjects.at(i).getColor());
        putText(frame,theObjects.at(i).getType(),cv::Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()-20),1,2,theObjects.at(i).getColor());
        i++;
	}
}

void drawObject(vector<Object> theObjects,Mat &frame){

    vector<Object>::iterator obs;
    obs=theObjects.begin();
    for(obs!=theObjects.end();;){
        int i = 0;
        circle(frame,Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()),10,cv::Scalar(0,0,255));
        putText(frame,intToString(theObjects.at(i).getXPos())+ " , " + intToString(theObjects.at(i).getYPos()), Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()+20),1,1,Scalar(0,255,0));
        putText(frame,theObjects.at(i).getType(),cv::Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()-30),1,2,theObjects.at(i).getColor());
        i++;
	}
}

void trackFilteredObject(Mat threshold,Mat HSV, Mat &cameraFeed) {
	vector <Object> objects;
	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	//double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS)
		{
			for (int index = 0; index >= 0; index = hierarchy[index][0])
			{
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;
				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA)
				{
					Object object;

					object.setXPos(moment.m10/area);
					object.setYPos(moment.m01/area);

					objects.push_back(object);

					objectFound = true;

				}
				else objectFound = false;
			}
			//let user know you found an object
			if(objectFound ==true)
			{
				//draw object location on screen
				drawObject(objects,cameraFeed);
			}
		}
		else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}

void trackFilteredObject(Object theObject,Mat threshold,Mat HSV, Mat &cameraFeed){

	vector <Object> objects;
	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,RETR_CCOMP,CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	//double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

		//if the area is less than 20 px by 20px then it is probably just noise
		//if the area is the same as the 3/2 of the image size, probably just a bad filter
		//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA){

					Object object;

					object.setXPos(moment.m10/area);
					object.setYPos(moment.m01/area);
					object.setType(theObject.getType());
					object.setColor(theObject.getColor());

					objects.push_back(object);

					objectFound = true;

				}else objectFound = false;
			}
			//let user know you found an object
			if(objectFound ==true){
				//draw object location on screen
				drawObject(objects,cameraFeed,temp,contours,hierarchy);}

		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}*/

