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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
//#include "pstream.h"

using namespace std;
using namespace cv;

pid_t pid, pid2;

void signalHandler(int signum)
{
    cout << "Interrupt signal (" << signum << ") received.\n";

    system("rm /tmp/vw_input");
    system("rm /tmp/vw_output");
    kill(pid, SIGTERM);
    kill(pid2, SIGTERM);
    exit(signum);

}

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
    fastNlMeansDenoisingColored(image, denoised, 2);
    medianBlur(denoised, denoised, 3);
    Mat hsv_image;
    cvtColor(denoised, hsv_image, CV_BGR2HSV);

    //***********DANGEROUS CODE AHEAD (but fast too(hopefully))******************

    signal(SIGINT, signalHandler);
    if (mkfifo("/tmp/vw_input", 0666) == -1)
    {
        perror("/tmp/vw_input already exists");
        system("rm /tmp/vw_input");
        exit(1);
    }
    if (mkfifo("/tmp/vw_output", 0666))
    {
        perror("/tmp/vw_output alreadt exists");
        system("rm /tmp/vw_input");
        system("rm /tmp/vw_output");
        exit(1);
    }

    pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
        exit(1);
    }
    else if (pid == 0)
    {
        //I am child
        //        usleep(200000);
        string arg = "vw -t --quiet -p /dev/stdout -i ";
        arg += argv[2];
        arg += " < /tmp/vw_input > /tmp/vw_output";
        //        system(arg.c_str());
        execl("/bin/sh", "sh", "-c", arg.c_str(), (char *) 0);
        exit(0);
    }
    // I AM PARENT
    int in_handle = open("/tmp/vw_input", O_WRONLY);
    if (in_handle == -1)
    {
        perror("Can't open /tmp/vw_input");
        kill(pid, SIGTERM);
        system("rm /tmp/vw_input");
        system("rm /tmp/vw_output");
        exit(1);
    }


    pid2 = fork();
    if (pid2 < 0)
    {
        perror("Fork failed");
        exit(1);
    }
    else if (pid2 == 0)
    {// I am child
        stringstream ss;
        string line;
        for (int i = 0; i < hsv_image.rows; ++i)
        {
            for (int j = 0; j < hsv_image.cols; ++j)
            {
                Vec3b &hsv = hsv_image.at<Vec3b>(i, j);

                ss.str("");
                line = " | ";
                ss << (int) hsv[0];
                ss << " ";
                ss << (int) hsv[1];
                ss << " ";
                ss << (int) hsv[2];
                line += ss.str();
                line += "\n";
                write(in_handle, line.c_str(), strlen(line.c_str()));
            }
        }
        close(in_handle);
        exit(0);
    }


    Mat image1(image.rows, image.cols, CV_8UC3, Scalar(0, 0, 0));
    //    image.copyTo(image);
    //    usleep(200000);
    int out_handle = open("/tmp/vw_output", O_RDONLY);
    if (out_handle == -1)
    {
        perror("Can't open /tmp/out_input");
        kill(pid, SIGTERM);
        kill(pid2, SIGTERM);
        system("rm /tmp/vw_input");
        system("rm /tmp/vw_output");
        exit(1);
    }
    char a, t;
    for (int i = 0; i < image.rows; ++i)
    {
        for (int j = 0; j < image.cols; ++j)
        {
            Vec3b &Color = image1.at<Vec3b>(i, j);
            read(out_handle, &a, 1);
            read(out_handle, &t, 1);
            if (t != '\n')
            {
                perror("prediction not terminated with null char");
            }
            switch (int(a - '0'))
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
    close(out_handle);
    close(in_handle);
    system("rm /tmp/vw_input");
    system("rm /tmp/vw_output");
    kill(pid, SIGTERM);
    kill(pid2, SIGTERM);

    imshow("Original Image", image);
    imshow("Denoised Image", denoised);
    imshow("Prediction", image1);
    moveWindow("Original Image", 100, 150);
    moveWindow("Denoised Image", 100, 150);
    moveWindow("Prediction", 100, 150);
    waitKey(0);

    return 0;
}

