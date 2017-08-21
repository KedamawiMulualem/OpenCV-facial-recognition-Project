//Name:Kedamawi Mulualem
//Date:5/31/2017
//UDC Robotics lab 105A
//Backward camera project
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <sstream>
#include <string>
#include <iostream>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <fstream>
#include <stdio.h>

using namespace std;
using namespace cv;





/** Function Headers */
Mat detect_crop_save(Mat frame);
void crop_face(Mat image, Point start, Point finish, int i);
void is_face(Mat &img, Point &face_high, Point &face_low, Point &eyes, int i);
void flat_image(Mat img);
/** Global variables */
const int FRAME_WIDTH = 640;;
const int FRAME_HEIGHT = 480;
int ans = 0;
int count_faces = 0;
//clasifier xml files
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);
/** @function main */
int main(int argc, const char** argv)
{
	
	
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	Mat frame;

	//-- 1. Load the cascades
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading\n"); return -1; };

	//-- 2. Read the video stream

	if (capture.open(0))
	{
		Mat gray_frame;
		Mat detected;


		while (true)
		{
		top:
			namedWindow("yetegegnew fit", CV_WINDOW_AUTOSIZE);
			
			capture.read(frame);
			detected = detect_crop_save(frame);
			imshow("yetegegnew fit", detected);
			waitKey(1);


		}
	}
	return 0;
}

/** @function detectAndDisplay */
Mat detect_crop_save(Mat frame)
{
	vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(40, 30));

	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		Point right_corner_face(faces[i].x + faces[i].width, faces[i].y + faces[i].height*1.1);
		Point left_corner_face(faces[i].x + faces[i].width*0.09 - faces[i].width*0.16, faces[i].y + faces[i].height*0.001 - faces[i].height*0.25);

		Point right_corner_face_crop(0, 0);
		Point left_corner_face_crop(0, 0);

	
		Mat faceROI = frame_gray(faces[i]);
		vector<Rect> eyes;

		//-- In each face, detect eyes
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(40, 30));
		//if you detect eye in the face area that means the detected face must me a real face so start setting up points to crop the face
		right_corner_face_crop = right_corner_face;
		left_corner_face_crop = left_corner_face;

		//check if the points to crop the face are inside the image bounders if they are not set them up to the border of the  of the image 
		if (right_corner_face.x <= 0) {
			right_corner_face_crop.x = 0;
		}

		if (right_corner_face.y < 0) {
			right_corner_face_crop.y = 0;
		}

		if (right_corner_face.x >FRAME_WIDTH) {
			right_corner_face_crop.x = FRAME_WIDTH;
		}


		if (right_corner_face.y >FRAME_HEIGHT) {
			right_corner_face_crop.y = FRAME_HEIGHT;
		}

		if (left_corner_face.x <0) {
			left_corner_face_crop.x = 0;
		}


		if (left_corner_face.y <0) {
			left_corner_face_crop.y = 0;
		}
		for (size_t j = 0; j < eyes.size(); j++)
		{	//find center of eyes
			Point center(faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5);
	
			//call is face functoin that checks if each of the  detected eyes are inside the area of their respectd faces
			is_face(frame, left_corner_face_crop, right_corner_face_crop, center, i);
			
		}

	}
	
	//-- Show what you have got
	return frame;
}

//a function that check if the detected face is actual human face
void is_face(Mat &img, Point &face_high, Point &face_low, Point &eyes, int i)
{	
	bool answer = face_high.y < eyes.y&& eyes.y< face_low.y;
	if (answer) {
		//if the detected face is real crop the face area
		crop_face(img, face_high, face_low, i);
	}




}
void crop_face(Mat image, Point left_corner_face_crop, Point right_corner_face_crop, int i) {
	cout << i << endl;
	//variabele to save the croped face
	Mat face_crop;
	//draw function to draw the area where face is detected
	rectangle(image, left_corner_face_crop, right_corner_face_crop, Scalar(255, 255, 0), 1, 8, 0);
	putText(image, "id no.: " + to_string(count_faces), left_corner_face_crop, 1.1, 2, Scalar(255, 200, 100), 1, 1, false);
	//functon to crop the face area out of the whole image and save it to a face_crop variabel
	image(Rect(left_corner_face_crop.x, left_corner_face_crop.y, right_corner_face_crop.x - left_corner_face_crop.x, right_corner_face_crop.y -left_corner_face_crop.y)).copyTo(face_crop);
	//resize the croped image into 200X200 image and save it into the same variable
	resize(face_crop, face_crop, Size(200, 200), 0, 0, CV_INTER_LINEAR);
	//change the image into black and white (gray)
	cvtColor(face_crop,face_crop, CV_BGR2GRAY);
	//save the image into the computer
	imwrite(to_string(i)+"face_"+to_string(count_faces)+"kd.jpg"  , face_crop);
	//increase the count face variabe to make the name of the next image to be saved diffrent from the current one
	
	count_faces++;

}
