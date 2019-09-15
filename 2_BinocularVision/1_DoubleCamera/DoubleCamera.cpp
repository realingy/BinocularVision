#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <iostream>

using namespace cv;

int main()
{
	VideoCapture cap;
	cap.open(1); //������ͷ

	if (!cap.isOpened())//�����Ƶ�����������򷵻�
	{
		std::cout << "������ͷ����\n";
		return -1;
	}

	cvWaitKey(30);
	Mat frame;//���ڱ���ÿһ֡ͼ��

	//system("C:\\\"Program Files (x86)\"\\\"CHUSEI 3D Webcam\"\\\"CHUSEI 3D Webcam.exe\"");

	while (1)
	{
		cap >> frame; //�ȼ��� cap.read(frame);
		if (frame.empty()) //���ĳ֡Ϊ�����˳�ѭ��
			break;

		imshow("�� ˫Ŀԭʼ��ͼ�� ", frame); //��ʾ˫Ŀԭʼͼ�� ԭʼ�ֱ���Ϊ 640 * 480
			
		Mat DoubleImage;
		resize(frame, DoubleImage, Size(640, 240), (0, 0), (0, 0), INTER_AREA); // ����ֱ�����Сһ��

		imshow("�� ˫Ŀ��С��ͼ�� ", DoubleImage); //��ʾͼ��

		Mat LeftImage = DoubleImage(Rect(0, 0, 320, 240)); //�ָ�õ�����ͼ
		Mat RightImage = DoubleImage(Rect(320, 0, 320, 240)); //�ָ�õ�����ͼ
		imshow("�� ����ͼ�� ", LeftImage); //��ʾ����ͼ
		imshow("�� ����ͼ�� ", RightImage); //��ʾ����ͼ

		char c = cvWaitKey(30);
		if (c == 27)//Esc ���˳�
		{
			break;
		}
	} 
	cap.release();//�ͷ���Դ

	return 0;
}


