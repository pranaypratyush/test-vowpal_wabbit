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
    fastNlMeansDenoisingColored(image, denoised,10,10,7,29);
    Mat hsv_image;
    cvtColor(image, hsv_image, CV_BGR2HSV);

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
    usleep(200000);

    Mat image1(image.rows,image.cols,CV_8UC3,Scalar(0,0,0));
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
                cerr<<"Unknown prediction at ("<<i<<","<<j<<")."<<endl;
                return 0;
            }

        }
    }
    
//    erode(image1, image1, Mat(), Point(-1, 1), 1, 1, 1);
//    dilate(image1, image1, Mat(), Point(-1, 1), 1, 1, 1);
    medianBlur(image1, image1, 3);

    
    imshow("Original Image", image);
    imshow("Denoised Image", denoised);
    imshow("Prediction", image1);
    waitKey(0);
    destroyAllWindows();
}

