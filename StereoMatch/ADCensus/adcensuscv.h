#ifndef ADCENSUSCV_H
#define ADCENSUSCV_H

#include <opencv2/opencv.hpp>

// using namespace cv;

// AD-Censusƥ����ۼ���

class ADCensusCV
{
public:
    ADCensusCV(const cv::Mat &leftImage, const cv::Mat &rightImage, cv::Size censusWin, float lambdaAD, float lambdaCensus);
    float ad(int wL, int hL, int wR, int hR) const; //adƥ�����
    float census(int wL, int hL, int wR, int hR) const; //censusƥ�����
    float adCensus(int wL, int hL, int wR, int hR) const; //ad-censusƥ�����

private:
    cv::Mat leftImage;
    cv::Mat rightImage;
    cv::Size censusWin;
    float lambdaAD;
    float lambdaCensus;
};

#endif // ADCENSUSCV_H
