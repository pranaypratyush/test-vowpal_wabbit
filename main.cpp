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
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR); // Read the file
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
    command += "--quiet";
    system(command.c_str());
    usleep(2000000);

    ifstream predict("predict");
    for (int i = 0; i < hsv_image.rows; ++i)
    {
        for (int j = 0; j < hsv_image.cols; ++j)
        {
            Vec3b &hsv = hsv_image.at<Vec3b>(i, j);
            getline(predict, line);

            switch (stoi(line))
            {
                
            }
        }
    }
}

