#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <string>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;

using namespace cv;
using namespace std;
  
ofstream outputFile;
string filename = "data.csv";    

int main()
{
	VideoCapture cap("/home/aman/Desktop/vib2.mp4"); // open the default camera
	if(!cap.isOpened())  // check if we succeeded
        {
		cout << "Error in using VideoCapture function." << endl;	
		return -1;
	}
	namedWindow("Original",CV_WINDOW_AUTOSIZE);
	namedWindow("Drawing",CV_WINDOW_AUTOSIZE);
	
	Vector<int> Y;
	Vector<int> T;
	int start_t = 0, end_t = 0, start_y = 0, end_y = 0;	
	int flag = 0; int sum_t = 0, n_osc = 0;
	
	outputFile.open(filename.c_str() );
	outputFile << "POSITION_X " << "," << "POSITION_Y" << "," << "TIME " << std::endl;
	
	int low_r = 80, low_b = 27, low_g = 13, high_r = 117, high_b = 99, high_g = 97;	//Initializes the track-bar values 	
	int count = 0;
	
	Mat canvas = Mat::zeros(480, 640, CV_8UC3);
	auto t1 = Clock::now();
	double time = 0;
	int inc = 0;
	while(1)
    	{
		Mat frame;
		
		count++;
		bool cap_status = cap.read(frame);
		auto t2 = Clock::now();

		// get a new frame from camera
		if (cap_status == 0)			//If status is false returns 1 to console
		{
			cout << "Error in capturing Frames" << endl;			
			return 1;
		}
		resize(frame, frame, Size(640, 480));
		
//<--------------------------------------------------------------------------------------------------------------------------->//
		Mat thresh;			
		inRange(frame,Scalar(low_b,low_g,low_r), Scalar(high_b,high_g,high_r),thresh);	//Threshold using inRange Function
	
		Mat element = getStructuringElement( MORPH_ELLIPSE, Size(5, 5));		//TO remove holes and other noises in thresholded-image
		dilate(thresh, thresh, element);
		erode(thresh, thresh, element);
		erode(thresh, thresh, element);	
		dilate(thresh, thresh, element);                                       
		
		
		//imshow("Thresholding", thresh);		//Shows the thresholded frames
//<----------------------------------------------------------------------------------------------------------------------------------->//

		
		
		Mat thresh_c = thresh.clone(); 		//Create a clone of thresholded image-thresh
		
		vector<vector<Point> > contours;	
		vector<Vec4i> hierarchy;

		findContours(thresh_c, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);		//Find contours and store it in contours
		Mat img(frame.size(), CV_8UC3, Scalar(0, 0, 0));						//Single channel black image is created
		
		// Filtering of Contours
				
		
		double area = -1;
		int index = -1;
		for (int idx = 0; idx < contours.size(); idx++) 					
        	{
			if (contourArea(contours[idx]) > 600)							// To remove noises sets threshold value as 5000
			{
				if (area < contourArea(contours[idx]))
				{	
					area = contourArea(contours[idx]);
					index = idx;
				}
			}
		}
		
		if (index >= 0 )
		{
			
			drawContours(frame, contours, index, Scalar(255,0,0), CV_FILLED, 8);			
			Moments M = moments(contours[index]); 	
			int cx = int(M.m10 / M.m00);
    			int cy = int(M.m01 / M.m00);
			
			int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
			outputFile << cx << "," << cy << "," << time<< std::endl;
			Y.push_back(cy);
			T.push_back(time);
			time = time + 16.66666666666666666666666666667; 
			circle(canvas, Point(cx, cy), 1, Scalar(0, 255, 0), CV_FILLED, 8);
			circle(frame, Point(cx, cy), 3, Scalar(0, 255, 0), CV_FILLED, 8);
		}
		
		//imshow("Drawing", canvas);
		
		double clock = cap.get(CV_CAP_PROP_POS_MSEC);
		std::string str = std::to_string((int)(clock));
    		cv::putText(frame, str, cv::Point(50,50), FONT_HERSHEY_PLAIN, 3, CV_RGB(255,0,255));
		imshow("Original", frame);
		if (waitKey(1) == 27)
			break;
	}
	
	//int maxVal = *max_element(Y.begin(),Y.end());
	//int minVal = *min_element(Y.begin(),Y.end());
	//int array[maxVal - minVal + 1] = {0};
	
	for (int i = 0; i < Y.size(); i++)
	{
		if(Y[i] > Y[i + 1] && Y[i] >= Y[i - 1] && i > 0)
		{
			if (flag == 1)
			{
				cout << "Time -> " << T[i] - start_t << endl;
				cout << "////////////////////////////" << endl;
				sum_t = sum_t + T[i] - start_t ; 
				n_osc++;
				flag = 0;
			}			
			start_t = T[i];
			start_y = Y[i];
			cout << i << endl;
		}
		else if(Y[i] < Y[i + 1] && Y[i] <= Y[i - 1] && i > 0 && (-end_y + start_y ) > 10)
		{
			end_t = T[i];
			end_y = Y[i];
			cout << i << endl;
			cout << "START -> " << start_y << " END ->" << end_y << endl;
			cout <<  "Amplitude -> " << -end_y + start_y << endl;
			flag = 1;
		}

	}
	double time_period = (sum_t / n_osc) ;
	double stiffness = (4 * (22 / 7) * (22 / 7) * (0.2265 + 0.315) ) / ((time_period) * (time_period)) * (1000000);

	cout << "************************************* " << endl;
	cout << "" << endl;
	cout << "Time Period -> " << time_period << "ms."<< endl; 
	cout << "Stiffness Constant -> " << stiffness << " N/m." <<endl;
	cout << "Natural Frequency -> "	<< 1000 / time_period  << " Hz" << endl;
	cout << "Number of Oscillations -> " << n_osc << endl;
	outputFile.close();
	cap.release();
	
	return 0;
}

