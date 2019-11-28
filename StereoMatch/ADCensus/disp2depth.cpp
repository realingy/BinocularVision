#include "disp2depth.h"

/**************************************************************************
�Ӳ�ĵ�λ�����أ�pixel������ȵĵ�λ�����Ǻ��ף�mm����ʾ��������ƽ��˫Ŀ�Ӿ���
���ι�ϵ���˴����ٻ�ͼ�Ƶ����ܼ򵥣������Եõ�������Ӳ�����ȵ�ת����ʽ��
					depth = ( f * baseline) / disp
��ʽ�У�depth��ʾ���ͼ��f��ʾ��һ���Ľ��࣬Ҳ�����ڲ��е�fx
baseline�������������֮��ľ��룬�������߾���
disp���Ӳ�ֵ��
**************************************************************************/
DepthBuild::DepthBuild()
{
}

DepthBuild::~DepthBuild()
{

}

//�Ӳ�ͼת���ͼ
// ���룺�Ӳ�ͼ������ڲξ���
// ��������ͼ
void DepthBuild::work(cv::Mat dispMap, cv::Mat &depthMap, cv::Mat K)
{
    int type = dispMap.type();
	
    float fx = K.at<float>(0, 0);
    float fy = K.at<float>(1, 1);
    float cx = K.at<float>(0, 2);
    float cy = K.at<float>(1, 2);
    float baseline = 65; //���߾���65mm

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
                if (!dispData[id])  continue;  //��ֹ0��
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