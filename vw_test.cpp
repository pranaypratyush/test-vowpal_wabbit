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
#include <cstdlib>

using namespace std;
using namespace cv;

const string vw_test::in_path = "/tmp/vw_input";
const string vw_test::out_path = "/tmp/vw_output";

void vw_test::vw_test_init(int parallels, char *path_to_vw_model)
{
    this->parallels = parallels;


    for (int i = 0; i < parallels; i++)
    {
        string temp_ipath = in_path + ((char)(i + '0'));
        string temp_opath = out_path + ((char)(i + '0'));
        if (mkfifo(temp_ipath.c_str(), 0666) == -1)
        {
            perror("/tmp/vw_input already exists");
            //            system("rm /tmp/vw_input");
            exit(1);
        }
        if (mkfifo(temp_opath.c_str(), 0666) == -1)
        {
            perror("/tmp/vw_output already exists");
            //            system("rm /tmp/vw_input");
            //            system("rm /tmp/vw_output");
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
            arg += " < ";
            arg += temp_ipath;
            arg += " > ";
            arg += temp_opath;
            //        system(arg.c_str());
            if (execl("/bin/sh", "sh", "-c", arg.c_str(), (char *) 0) == -1)
            {
                perror("Could not run vw. Is vw installed ?");
                exit(1);
            }
            exit(0);
        }
        //        in_path = in_path.substr(0, in_path.length() - 1);
        //        out_path = out_path.substr(0, out_path.length() - 1);
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
    //    thread in_thread(vw_test::write_to_input, hsv_image, in_handle);
    //    thread out_thread(vw_test::read_from_output, prediction, out_handle);
    thread* in_thread = new thread[parallels];
    thread* out_thread = new thread[parallels];
    for (int i = 0; i < parallels; i++)
    {
        in_thread[i] = thread(vw_test::write_to_input, hsv_image, i,parallels);
        out_thread[i] = thread(vw_test::read_from_output, prediction, i,parallels);
    }
    for (int i = parallels-1; i > 0; i--)
    {
        out_thread[i].join();
        in_thread[i].join();
    }

    //    usleep(900000);
}

void vw_test::clean_up()
{
    
    system("rm /tmp/vw_input");
    system("rm /tmp/vw_output");
    kill(pid, SIGTERM);
}

vw_test::vw_test(int parallels, char *path_to_vw_model)
{
    vw_test_init(parallels, path_to_vw_model);
}

vw_test::~vw_test()
{
    clean_up();
}

void vw_test::write_to_input(Mat hsv_image, int p_no, int parallels)
{

    int in_handle = open((in_path + ((char)(p_no + '0')) ).c_str(), O_WRONLY);
    if (in_handle == -1)
    {
        perror("Can't open /tmp/vw_input");
        close(in_handle);
    }
    //    stringstream ss;
    //    string line;
    char buffer[20] = " | ";
    char temp[6];
    for (int i = (hsv_image.rows / parallels) * p_no; i < (hsv_image.rows / parallels)*(p_no + 1); ++i)
    {
        for (int j = 0; j < hsv_image.cols; ++j)
        {
            Vec3b &hsv = hsv_image.at<Vec3b>(i, j);

            //            ss.str("");
            //            line = " | ";
            //            ss << (int) hsv[0];
            //            ss << " ";
            //            ss << (int) hsv[1];
            //            ss << " ";
            //            ss << (int) hsv[2];
            //            line += ss.str();
            //            line += "\n";
            strcpy(buffer, " | ");
            strcpy(temp, "");

            snprintf(temp, 4, "%d", (int) hsv[0]);
            strcat(buffer, temp);
            strcat(buffer, " ");
            snprintf(temp, 4, "%d", (int) hsv[1]);
            strcat(buffer, temp);
            strcat(buffer, " ");
            snprintf(temp, 4, "%d", (int) hsv[2]);
            strcat(buffer, temp);
            strcat(buffer, "\n");
//            printf(buffer);
            write(in_handle, buffer, strlen(buffer));
        }
    }
    close(in_handle);
}

void vw_test::read_from_output(Mat prediction, int p_no,int parallels)
{
    int out_handle = open((out_path +((char)(p_no + '0'))).c_str(), O_RDONLY);
    if (out_handle == -1)
    {
        perror("Can't open /tmp/out_input");
        return;
    }
    char a[2];
    for (int i = (prediction.rows / parallels) * p_no; i < (prediction.rows / parallels)*(p_no + 1); ++i)
    {
        for (int j = 0; j < prediction.cols; ++j)
        {
            Vec3b &Color = prediction.at<Vec3b>(i, j);
            read(out_handle, &a, 2);
            //            read(out_handle, &t, 1);
            if (a[1] != '\n')
            {
                perror("prediction not terminated with null char");
            }
            switch (int(a[0] - '0'))
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