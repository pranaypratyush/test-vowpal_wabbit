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
#include <sys/types.h>   // Types used in sys/socket.h and netinet/in.h
#include <netinet/in.h>  // Internet domain address structures and functions
#include <sys/socket.h>  // Structures and functions used for socket API
#include <netdb.h>       // Used for domain/DNS hostname lookup
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <opencv-3.1.0-dev/opencv2/core/matx.hpp>

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
    fastNlMeansDenoisingColored(image, denoised, 2);
    medianBlur(denoised, denoised, 3);
    Mat hsv_image;
    cvtColor(denoised, hsv_image, CV_BGR2HSV);

    system("killall vw");
    string command = "vw -t --daemon --quiet --port 26542 -i ";
    command += argv[2];
    system(command.c_str());


    int socketHandle;
    if ((socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0)
    {
        close(socketHandle);
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in localSocketInfo;
    struct hostent *hPtr;

    bzero(&localSocketInfo, sizeof (sockaddr_in)); // Clear structure memory

    // Get system information
    /*
        if ((hPtr = gethostbyname("localhost")) == NULL)
        {
            cerr << "System DNS name resolution not configured properly." << endl;
            cerr << "Error number: " << ECONNREFUSED << endl;
            exit(EXIT_FAILURE);
        }
     */
    // Load system information for remote socket server into socket data structures

    //memcpy((char *) &localSocketInfo.sin_addr, hPtr->h_addr, hPtr->h_length);
    localSocketInfo.sin_family = AF_INET;
    localSocketInfo.sin_port = htons((u_short) 26542); // Set port number
    inet_aton("127.0.0.1", &localSocketInfo.sin_addr);

    usleep(200000);
    if ((connect(socketHandle, (struct sockaddr *) &localSocketInfo, sizeof (sockaddr_in)) < 0))
    {
        close(socketHandle);
        exit(EXIT_FAILURE);
    }



    Mat image1(image.rows, image.cols, CV_8UC3, Scalar(0, 0, 0));
    image.copyTo(image);


    String payload;
    for (int i = 0; i < image.rows; ++i)
    {
        for (int j = 0; j < image.cols; ++j)
        {
            Vec3b &Color = image1.at<Vec3b>(i, j);
            Vec3b &hsv = hsv_image.at<Vec3b>(i, j);
            ostringstream ss("");
            payload = " | ";
            ss << (int) hsv[0];
            payload += ss.str();
            payload += " ";
            ss.str("");
            ss << (int) hsv[1];
            payload += ss.str();
            payload += " ";
            ss.str("");
            ss << (int) hsv[2];
            payload += ss.str();
            payload += "\n";
            send(socketHandle, payload.c_str(), strlen(payload.c_str()) + 1, 0);
            char a[2];
            recv(socketHandle, &a, sizeof (a), 0);

            switch (int(a[0]) - '0')
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
    system("killall vw");

    //    for(int i=0;i<1;i++)
    //    {
    //        erode(image1,image1,Mat());
    //        dilate(image1,image1,Mat());
    //    }


    //    erode(image1, image1, Mat(), Point(-1, 1), 1, 1, 1);
    //    dilate(image1, image1, Mat(), Point(-1, 1), 1, 1, 1);
    //    medianBlur(image1, image1, 3);

    //    cvtColor(image1, hsv_image, CV_BGR2HSV);
    //    Mat denoised1;
    //    fastNlMeansDenoisingColored(image, denoised1,2);
    //    cvtColor(denoised1, hsv_image, CV_BGR2HSV );
    //    Mat image_canny(image.rows, image.cols, CV_8UC3, Scalar(0, 0, 0));
    //    Mat h_channel, _h_channel;
    //    //extracting the h channel
    //    extractChannel(hsv_image, h_channel, 0);
    //    //http://www.academypublisher.com/proc/isip09/papers/isip09p109.pdf
    //    
    //    Mat gray;
    //    cvtColor(denoised1,gray,CV_BGR2GRAY);
    //    double otsu_thresh_val = cv::threshold(h_channel, _h_channel, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    //    Canny(h_channel, image_canny, otsu_thresh_val*0.75 , otsu_thresh_val);
    //    cout<<otsu_thresh_val<<endl;
    //    
    //    namedWindow( "Original Image", WINDOW_AUTOSIZE );
    //    namedWindow( "Denoised Image", WINDOW_AUTOSIZE );
    //    namedWindow( "Prediction", WINDOW_AUTOSIZE );
    //    namedWindow( "Edge Map", WINDOW_AUTOSIZE );
    imshow("Original Image", image);
    imshow("Denoised Image", denoised);
    imshow("Prediction", image1);
    //    imshow("Edge Map", image_canny);
    //    imshow("Gray",h_channel);
    moveWindow("Original Image", 100, 150);
    moveWindow("Denoised Image", 100, 150);
    moveWindow("Prediction", 100, 150);
    //    moveWindow( "Edge Map", 100,150 );
    //    moveWindow( "Gray", 100,150 );

    waitKey(0);
    destroyAllWindows();
    return 0;
}

