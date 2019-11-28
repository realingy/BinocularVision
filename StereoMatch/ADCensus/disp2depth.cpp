#include "disp2depth.h"

/**************************************************************************
视差的单位是像素（pixel），深度的单位往往是毫米（mm）表示。而根据平行双目视觉的
几何关系（此处不再画图推导，很简单），可以得到下面的视差与深度的转换公式：
					depth = ( f * baseline) / disp
上式中，depth表示深度图；f表示归一化的焦距，也就是内参中的fx
baseline是两个相机光心之间的距离，称作基线距离
disp是视差值。
**************************************************************************/
DepthBuild::DepthBuild()
{
}

DepthBuild::~DepthBuild()
{

}

//视差图转深度图
// 输入：视差图、相机内参矩阵
// 输出：深度图
void DepthBuild::work(cv::Mat dispMap, cv::Mat &depthMap, cv::Mat K)
{
    int type = dispMap.type();
	
    float fx = K.at<float>(0, 0);
    float fy = K.at<float>(1, 1);
    float cx = K.at<float>(0, 2);
    float cy = K.at<float>(1, 2);
    float baseline = 65; //基线距离65mm

    if (type == CV_8U)
    {
        const float PI = 3.14159265358;
        int height = dispMap.rows;
        int width = dispMap.cols;

        uchar* dispData = (uchar*)dispMap.data;
        ushort* depthData = (ushort*)depthMap.data;
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                int id = i*width + j;
                if (!dispData[id])  continue;  //防止0除
                depthData[id] = ushort( (float)fx *baseline / ((float)dispData[id]) );
            }
        }
    }
    else
    {
        std::cout << "please confirm dispImg's type!" << std::endl;
        cv::waitKey(0);
    }
}