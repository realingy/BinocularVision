#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int main()
{
	VideoCapture cap;
	cap.open(0); //打开摄像头
	if (!cap.isOpened()) {	//如果视频不能正常打开则返回
		std::cout << "打开摄像头错误！\n";
		return -1;
	}

	Mat frame;
	while (1)
	{
		cap >> frame; //等价于cap.read(frame);
		if (frame.empty()) //如果某帧为空则退出循环
			break;
		imshow("video", frame);
		waitKey(20); //每帧延时 20 毫秒
	}
	cap.release(); //释放资源
	return 0;
}

