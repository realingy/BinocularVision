#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int main()
{
	VideoCapture cap;
	cap.open(0); //������ͷ
	if (!cap.isOpened()) {	//�����Ƶ�����������򷵻�
		std::cout << "������ͷ����\n";
		return -1;
	}

	Mat frame;
	while (1)
	{
		cap >> frame; //�ȼ���cap.read(frame);
		if (frame.empty()) //���ĳ֡Ϊ�����˳�ѭ��
			break;
		imshow("video", frame);
		waitKey(20); //ÿ֡��ʱ 20 ����
	}
	cap.release(); //�ͷ���Դ
	return 0;
}

