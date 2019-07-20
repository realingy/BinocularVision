#include<opencv2/opencv.hpp>
#include<io.h>

using namespace cv;
using namespace std;

/*
输入：
	Location 为文件夹所在的位置
	num为文件的命名（字符形式）
	EndLocationType为文件的后缀
结果：
	FilesName为目录文件夹下所有该类型的文件存入（目录+文件名+文件类型）
*/
string FileLocation(string Location, int num, string EndLocationType)
{
	string imageFilename;
	stringstream StrStm1;
	StrStm1 << num + 1;
	StrStm1 >> imageFilename;
	imageFilename += EndLocationType;
	return Location + imageFilename;
}

/*按下esc退出，按下空格键截取图像*/
int main()
{
	Mat srcimage;
	char c;
	int temp = 0;//计数器
	VideoCapture cam(1);
	if (!cam.isOpened())
	{
		exit(0);
	}
	namedWindow("【摄像头】");
	while (true)
	{
		cam >> srcimage;
		imshow("【摄像头】", srcimage);
		c = waitKey(30);
		if (c == ' ')
		{
			imwrite(FileLocation("G:/LeftPicture/left/", temp, ".jpg"), srcimage);
			temp++;
		}
		if (c == 27)
		{
			break;
		}
	}
	return 0;
}