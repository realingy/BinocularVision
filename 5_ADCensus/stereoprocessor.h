#ifndef STEREOPROCESSOR_H
#define STEREOPROCESSOR_H

#include "adcensuscv.h"				//ad-censusƥ��������
#include "aggregation.h"			//�ۺ����
#include "scanlineoptimization.h"	//ɨ�����Ż����
#include "disparityrefinement.h"	//�Ӳ�ͼУ��
#include <omp.h>
#include "common.h"

using namespace std;

// ����ƥ�䴦����

class StereoProcessor
{
public:
    StereoProcessor(uint dMin, uint dMax, cv::Mat leftImage, cv::Mat rightImage, cv::Size censusWin, float defaultBorderCost,
                    float lambdaAD, float lambdaCensus, string savePath, uint aggregatingIterations,
                    uint colorThreshold1, uint colorThreshold2, uint maxLength1, uint maxLength2, uint colorDifference,
                    float pi1, float pi2, uint dispTolerance, uint votingThreshold, float votingRatioThreshold,
                    uint maxSearchDepth, uint blurKernelSize, uint cannyThreshold1, uint cannyThreshold2, uint cannyKernelSize);
    ~StereoProcessor();
    bool init(string &error);
    bool compute();
    cv::Mat getDisparity();

private:
    int dMin;
    int dMax;
    cv::Mat images[2];
    cv::Size censusWin;
    float defaultBorderCost;
    float lambdaAD;
    float lambdaCensus;
    string savePath;
    uint aggregatingIterations;
    uint colorThreshold1;
    uint colorThreshold2;
    uint maxLength1;
    uint maxLength2;
    uint colorDifference;
    float pi1;
    float pi2;
    uint dispTolerance;
    uint votingThreshold;
    float votingRatioThreshold;
    uint maxSearchDepth;
    uint blurKernelSize;
    uint cannyThreshold1;
    uint cannyThreshold2;
    uint cannyKernelSize;
    bool validParams, dispComputed;

    vector<vector<cv::Mat> > costMaps;
    cv::Size imgSize;
    ADCensusCV *adCensus;
    Aggregation *aggregation;
    DisparityRefinement *dispRef;
    cv::Mat disparityMap, floatDisparityMap;

    void costInitialization();
    void costAggregation();
    void scanlineOptimization();
    void outlierElimination();
    void regionVoting();
    void properInterpolation();
    void discontinuityAdjustment();
    void subpixelEnhancement();

    cv::Mat cost2disparity(int imageNo);

    template <typename T>
    void saveDisparity(const cv::Mat &disp, string filename, bool stretch = true);
};

#endif // STEREOPROCESSOR_H
