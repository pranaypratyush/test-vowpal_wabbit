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
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>


class vw_test {
public:
    pid_t pid;
    int parallels;
    
    void vw_test_init(int parallels, char *path_to_vw_model);
    void getPredictions(cv::Mat original, cv::Mat prediction);
    void clean_up();
    vw_test(int parallels,char *path_to_vw_model);
    ~vw_test();
private:
    static void write_to_input(cv::Mat, int p_no,int parallels);
    static void read_from_output(cv::Mat, int p_no,int parallels);
    const static std::string in_path; 
    const static std::string out_path; 
};

#endif /* VW_TEST_H */

