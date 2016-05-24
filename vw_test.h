/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   vw_test.h
 * Author: pranay
 *
 * Created on 24 May, 2016, 7:39 AM
 */

#ifndef VW_TEST_H
#define VW_TEST_H

#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>


class vw_test {
public:
    pid_t pid, pid2;
    int in_handle, out_handle;
    void vw_test_init(char *path_to_vw_model);
    void getPredictions(cv::Mat *original, cv::Mat *prediction);
    void clean_up();
    vw_test(char *path_to_vw_model);
    ~vw_test();
};

#endif /* VW_TEST_H */

