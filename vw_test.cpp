/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   vw_test.cpp`
 * Author: pranay
 *
 * Created on 8 May, 2016, 3:36 PM
 */
#include "vw_test.h"
#include <thread>
#include <chrono>

using namespace std;
using namespace cv;

void vw_test::vw_test_init(char *path_to_vw_model)
{
    if (mkfifo("/tmp/vw_input", 0666) == -1)
    {
        perror("/tmp/vw_input already exists");
        system("rm /tmp/vw_input");
        exit(1);
    }
    if (mkfifo("/tmp/vw_output", 0666))
    {
        perror("/tmp/vw_output already exists");
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
        arg += path_to_vw_model;
        arg += " < /tmp/vw_input > /tmp/vw_output";
        //        system(arg.c_str());
        if (execl("/bin/sh", "sh", "-c", arg.c_str(), (char *) 0) == -1)
        {
            perror("Could not run vw. Is vw installed ?");
            exit(1);
        }
        exit(0);
    }

    // I AM PARENT
    ///dangerous here
    //be extremely cautious 



}

void vw_test::getPredictions(Mat original, Mat prediction)
{
    Mat hsv_image;
    cvtColor(original, hsv_image, CV_BGR2HSV);
    //    prediction = Mat(original.rows, original.cols, CV_8UC3, Scalar(0, 0, 0));
    thread in_thread(vw_test::write_to_input, hsv_image, in_handle);
    thread out_thread(vw_test::read_from_output, prediction, out_handle);

   
    out_thread.join();
    in_thread.join();
//    usleep(900000);
}

void vw_test::clean_up()
{
    close(out_handle);
    close(in_handle);
    system("rm /tmp/vw_input");
    system("rm /tmp/vw_output");
    kill(pid, SIGTERM);
}

vw_test::vw_test(char *path_to_vw_model)
{
    vw_test_init(path_to_vw_model);
}

vw_test::~vw_test()
{
    clean_up();
}

void vw_test::write_to_input(Mat hsv_image, int in_handle)
{

    in_handle = open("/tmp/vw_input", O_WRONLY);
    if (in_handle == -1)
    {
        perror("Can't open /tmp/vw_input");
        close(in_handle);
    }
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
}

void vw_test::read_from_output(Mat prediction, int out_handle)
{
    out_handle = open("/tmp/vw_output", O_RDONLY);
    if (out_handle == -1)
    {
        perror("Can't open /tmp/out_input");
        return;
    }
    char a, t;
    for (int i = 0; i < prediction.rows; ++i)
    {
        for (int j = 0; j < prediction.cols; ++j)
        {
            Vec3b &Color = prediction.at<Vec3b>(i, j);
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
                return;
            }

        }
    }
    close(out_handle);
}