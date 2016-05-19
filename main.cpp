/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: pranay
 *
 * Created on 8 May, 2016, 3:36 PM
 */

#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <opencv-3.1.0-dev/opencv2/highgui.hpp>
#include <opencv-3.1.0-dev/opencv2/imgproc.hpp>
//#include "pstream.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << " Usage: display_image ImageToLoadAndDisplay" << endl;
        return -1;
    }

    Mat image;
    Mat denoised;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR); // Read the file
    fastNlMeansDenoisingColored(image, denoised, 4, 7, 21);
    Mat hsv_image;
    cvtColor(denoised, hsv_image, CV_BGR2HSV);

    ofstream temp("temp.txt");
    ostringstream ss;
    string line = "";
    for (int i = 0; i < hsv_image.rows; ++i)
    {
        for (int j = 0; j < hsv_image.cols; ++j)
        {
            Vec3b &hsv = hsv_image.at<Vec3b>(i, j);

            ss.str("");
            line = "| ";
            ss << (int) hsv[0];
            ss << " ";
            ss << (int) hsv[1];
            ss << " ";
            ss << (int) hsv[2];
            line += ss.str();
            line += "\n";
            temp << line;
        }
    }
    temp.close();
    //http://pstreams.sourceforge.net/
    // run a process and create a streambuf that reads its stdout and stderr

    //    redi::ipstream proc("", redi::pstreams::pstderr);
    //    std::string line;
    //    // read child's stdout
    //    while (std::getline(proc.out(), line))
    //        std::cout << "stdout: " << line << 'n';
    string command = "vw -t -p predict ";
    command += " -d ";
    command += "temp.txt";
    command += " -i ";
    command += argv[2];
    command += " --quiet ";
    system(command.c_str());
//    usleep(200000);

    Mat image1(image.rows, image.cols, CV_8UC3, Scalar(0, 0, 0));
    image.copyTo(image);
    ifstream predict("predict");
    double d;
    for (int i = 0; i < image.rows; ++i)
    {
        for (int j = 0; j < image.cols; ++j)
        {
            Vec3b &Color = image1.at<Vec3b>(i, j);
            predict >> d;

            switch (int(d))
            {
            case 1:
                Color[0] = 0;
                Color[1] = 0;
                Color[2] = 0;
                break;

            case 2:
                Color[0] = 0;
                Color[1] = 255;
                Color[2] = 255;
                break;

            case 3:
                Color[0] = 0;
                Color[1] = 255;
                Color[2] = 0;
                break;

            case 4:
                Color[0] = 0;
                Color[1] = 0;
                Color[2] = 255;
                break;
            default:
                cerr << "Unknown prediction at (" << i << "," << j << ")." << endl;
                return 0;
            }

        }
    }


    //    erode(image1, image1, Mat(), Point(-1, 1), 1, 1, 1);
    //    dilate(image1, image1, Mat(), Point(-1, 1), 1, 1, 1);
    //    medianBlur(image1, image1, 3);

    //    cvtColor(image1, hsv_image, CV_BGR2HSV);
    Mat denoised1;
    fastNlMeansDenoisingColored(image, denoised1,2);
    cvtColor(denoised1, hsv_image, CV_BGR2HSV );
    Mat image_canny(image.rows, image.cols, CV_8UC3, Scalar(0, 0, 0));
    Mat h_channel, _h_channel;
    //extracting the h channel
    extractChannel(hsv_image, h_channel, 0);
    //http://www.academypublisher.com/proc/isip09/papers/isip09p109.pdf
    
    Mat gray;
    cvtColor(denoised1,gray,CV_BGR2GRAY);
    double otsu_thresh_val = cv::threshold(h_channel, _h_channel, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    Canny(h_channel, image_canny, otsu_thresh_val*0.75 , otsu_thresh_val);
    cout<<otsu_thresh_val<<endl;
    
    namedWindow( "Original Image", WINDOW_AUTOSIZE );
    namedWindow( "Denoised Image", WINDOW_AUTOSIZE );
    namedWindow( "Prediction", WINDOW_AUTOSIZE );
    namedWindow( "Edge Map", WINDOW_AUTOSIZE );
    imshow("Original Image", image);
    imshow("Denoised Image", denoised);
    imshow("Prediction", image1);
    imshow("Edge Map", image_canny);
    imshow("Gray",h_channel);
    moveWindow( "Original Image", 10,50 );
    moveWindow( "Denoised Image", 10,50 );
    moveWindow( "Prediction", 10,50 );
    moveWindow( "Edge Map", 10,50 );
    waitKey(0);
    
    destroyAllWindows();
}

