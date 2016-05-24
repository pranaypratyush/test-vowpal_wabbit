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

//#include "pstream.h"
#include "vw_test.h"
#include <chrono>
using namespace std;
using namespace cv;
using namespace chrono;

typedef std::chrono::high_resolution_clock clk;

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
    auto t1 = clk::now();
    image.copyTo(denoised);
//    fastNlMeansDenoisingColored(image, denoised, 2);
    medianBlur(denoised, denoised, 3);
    Mat prediction(image.rows, image.cols, CV_8UC3, Scalar(0, 0, 0));
    
    //***********DANGEROUS CODE AHEAD (but fast too(hopefully))******************

    vw_test new_vw(argv[2]);
    auto t2 = clk::now();
    new_vw.getPredictions(denoised,prediction);
    auto t3 = clk::now();

    imshow("Original Image", image);
    imshow("Denoised Image", denoised);
    imshow("Prediction", prediction);
    moveWindow("Original Image", 100, 150);
    moveWindow("Denoised Image", 100, 150);
    moveWindow("Prediction", 100, 150);
    cout<<duration_cast<milliseconds>(t2 - t1).count()<<endl;
    cout<<duration_cast<milliseconds>(t3 - t2).count()<<endl;
    waitKey(0);

    return 0;
}

