#include "adcensuscv.h"

ADCensusCV::ADCensusCV(const cv::Mat &leftImage, const cv::Mat &rightImage, cv::Size censusWin, float lambdaAD, float lambdaCensus)
{
    this->leftImage = leftImage;
    this->rightImage = rightImage;
    this->censusWin = censusWin;
    this->lambdaAD = lambdaAD;
    this->lambdaCensus = lambdaCensus;
}

// ����ͼ���ADƥ����ۼ���
// ���룺Ŀ���������ͼ���е�����λ��
// �����Ŀ���������ͼ���е����ƶȣ�ֵԽС���ƶ�Խ�ߣ���Ϊƥ���Ŀ����Ծ�Խ��
float ADCensusCV::ad(int wL, int hL, int wR, int hR) const
{
    float dist = 0;	//��ʼ�Ӳ�
    const cv::Vec3b &colorLP = leftImage.at<cv::Vec3b>(hL, wL);
    const cv::Vec3b &colorRP = rightImage.at<cv::Vec3b>(hR, wR);

    for(uchar color = 0; color < 3; ++color)
    {
        dist += std::abs(colorLP[color] - colorRP[color]);
    }
    return (dist / 3); //rgb���������Ӳ��ƽ��ֵ
}

// ����ͼ���censusƥ����ۼ���
// ���룺Ŀ���������ͼ���е�����λ��
// �����Ŀ���������ͼ���е����ƶȣ�ֵԽС���ƶ�Խ�ߣ���Ϊƥ���Ŀ����Ծ�Խ��
float ADCensusCV::census(int wL, int hL, int wR, int hR) const
{
    float dist = 0;
    const cv::Vec3b &colorRefL = leftImage.at<cv::Vec3b>(hL, wL); //��ͼĿ���p
    const cv::Vec3b &colorRefR = rightImage.at<cv::Vec3b>(hR, wR); //��ͼĿ���p

	//����census�������أ����㴰���ڸ���λ���������ƶȣ������ΪĿ�������ƶ�
    for(int h = -censusWin.height / 2; h <= censusWin.height / 2; ++h)
    {
        for(int w = -censusWin.width / 2; w <= censusWin.width / 2; ++w)
        {
            const cv::Vec3b &colorLP = leftImage.at<cv::Vec3b>(hL + h, wL + w);
            const cv::Vec3b &colorRP = rightImage.at<cv::Vec3b>(hR + h, wR + w);
            for(uchar color = 0; color < 3; ++color)
            {
                // bool diff = (colorLP[color] < colorRefL[color]) ^ (colorRP[color] < colorRefR[color]);
                bool diff = (colorLP[color] - colorRefL[color]) * (colorRP[color] - colorRefR[color]) < 0;
                dist += (diff)? 1: 0;
            }
        }
    }

    return dist;
}

// AD��Censusƥ����۽��
// ���룺Ŀ���������ͼ���е�����λ��
// �����Ŀ���������ͼ���е����ƶȣ�ֵԽС���ƶ�Խ�ߣ���Ϊƥ���Ŀ����Ծ�Խ��
float ADCensusCV::adCensus(int wL, int hL, int wR, int hR) const
{
    float dist;

    // compute Absolute Difference cost
    float cAD = ad(wL, hL, wR, hR);

    // compute Census cost
    float cCensus = census(wL, hL, wR, hR);

    // combine the two costs
	// ������ƥ����۽��й�һ���������
    dist = 1 - exp(-cAD / lambdaAD); 
    dist += 1 - exp(-cCensus / lambdaCensus);

    return dist;
}
