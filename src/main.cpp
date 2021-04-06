

#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <time.h>

#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

Scalar lower_blue = Scalar (94,109,20);
Scalar upper_blue =  Scalar(113,255,255);

Scalar lower_red = Scalar(170,50,90);
Scalar upper_red = Scalar(184,255,255);

Scalar lower_green = Scalar(50,50,50);
Scalar upper_green = Scalar(80,220,200);

//Scalar lower_siluet = Scalar()
//Scalar upper_siluet = Scalar()

Scalar blue = Scalar(255,0,0);
Scalar red = Scalar(0,0,255);
Scalar green = Scalar(0,255,0);

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName2 = "Thresholded Image";
const string windowName4 = "Deteksi";
const string trackbarWindowName = "Trackbars";


//The following for canny edge detec
Mat dst, detected_edges;
Mat src, src_gray;

string intToString(int number){

	std::stringstream ss;
	ss << number;
	return ss.str();
}

void drawCircle (Scalar color,Mat threshold,Mat HSV, Mat &cameraFeed){
    int a =0;
	vector <Vec3f> circles;
    cv::HoughCircles(threshold, circles, HOUGH_GRADIENT, 2, threshold.rows/4,100,50,10,800 );
    for( size_t i = 0; i < circles.size(); i++ )
    {
        a++;
        Vec3i c = circles[i];
        Point center = Point(c[0], c[1]);
        // circle center
        circle( cameraFeed, center,1,color);
        // circle outline
        int radius = c[2];
        circle( cameraFeed, center, radius,color,3,LINE_AA);
        putText(cameraFeed,intToString(a),cv::Point(c[0],c[1]-10),1,2,color);
        putText(cameraFeed,format("r %d",radius),Point(c[0]+9,c[1]+10),5,0.8,color,1);
        putText(cameraFeed,format("L %d",(22/7)*radius*radius),Point(c[0]+9,c[1]+25),5,0.8,color,1);
        putText(cameraFeed,format("K %d",(22/7)*2*radius),Point(c[0]+9,c[1]+35),5,0.8,color,1);

    }
        if(color==Scalar(0,255,0))
        {putText(cameraFeed,intToString(circles.size()),Point(220,15),5,0.5,color,1);}
        if(color==Scalar (0,0,255))
        {putText(cameraFeed,intToString(circles.size()),Point(370,15),5,0.5,color,1);}
        if(color==Scalar (255,0,0))
        {putText(cameraFeed,intToString(circles.size()),Point(530,15),5,0.5,color,1);}
        putText(cameraFeed,"Red Circle  :",Point(420,15),5,0.7,Scalar(0,0,255),1);
        putText(cameraFeed,"Green Circle:",Point(100,15),5,0.7,Scalar(0,255,0),1);
        putText(cameraFeed,"Blue Circle :",Point(260,15),5,0.7,Scalar(255,0,0),1);
    }

void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

    medianBlur(thresh, thresh, 5);
    //GaussianBlur(thresh, thresh, Size(3, 3), 5);
    //Canny(thresh,thresh,20,30);

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);

	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
	//GaussianBlur(thresh, thresh, Size(3, 3), 5);
	//Canny(thresh,thresh,20,30);
	//Canny(thresh,thresh,20,30);

}
void drawContour(Scalar color,Mat threshold,Mat HSV, Mat &cameraFeed){

    GaussianBlur(threshold, threshold, Size(3, 3), 5);
    Canny(threshold,threshold,60,120);

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,RETR_LIST,CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	for(int i =0; i<hierarchy.size(); i++)
        {drawContours(cameraFeed,contours,i,color,1,LINE_4,hierarchy);
	}
}

void allprocedure(Scalar Low,Scalar High,Mat threshold,Mat HSV, Mat &cameraFeed,Mat &cameraFeed2,Scalar color){
            inRange(HSV,Low,High,threshold);
			morphOps(threshold);
			drawCircle (color,threshold,HSV,cameraFeed2);
			drawCircle (color,threshold,HSV,cameraFeed);
			drawContour(color,threshold,HSV,cameraFeed2);
}

int main(int argc, char* argv[])
{

	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	Mat threshold;
	Mat HSV;
	Mat cameraFeed2;

	VideoCapture capture(0);
	capture.open(0);

	capture.set(CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
    int frameCounter = 0;
    int tick = 0;
    int fps;
	time_t timeBegin=time(0);

	waitKey(1000);
	while(1){

        frameCounter++;
        time_t timeNow=time(0)-timeBegin;
        if (timeNow - tick >= 1)
        {
            tick++;
            fps = frameCounter;
            frameCounter = 0;
        }
        //time(&start);
		//store image to matrixmatrix
		capture.read(cameraFeed);
        capture.read(cameraFeed2);
        rectangle(cameraFeed2,Point(0,900),
          Point(900,0),
          Scalar(0,0,0),-1,
           8);

		src = cameraFeed;

  		if( !src.data )
  		{ return -1; }

            //membuat box
            rectangle(cameraFeed,Point(0,20),
            Point(900,0),
            Scalar(0,0,0),-1,
            8);
            rectangle(cameraFeed,Point(0,20),
            Point(900,0),
            Scalar(255,255,255),1,
            8);
            rectangle(cameraFeed2,Point(0,20),
            Point(900,0),
            Scalar(255,255,255),1,
            8);

            putText(cameraFeed,format("FPS=%d", fps ),Point(5,15),5,0.5,Scalar(255,255,255),1);
            putText(cameraFeed2,format("FPS=%d", fps ),Point(5,15),5,0.5,Scalar(255,255,255),1);

			cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
			allprocedure(lower_red,upper_red,threshold,HSV, cameraFeed,cameraFeed2,red);
			allprocedure(lower_blue,upper_blue,threshold,HSV, cameraFeed,cameraFeed2,blue);
			allprocedure(lower_green,upper_green,threshold,HSV,cameraFeed,cameraFeed2,green);

            imshow(windowName4,cameraFeed2);
            imshow(windowName,cameraFeed);


		waitKey(25);


	}
	return 0;
}
