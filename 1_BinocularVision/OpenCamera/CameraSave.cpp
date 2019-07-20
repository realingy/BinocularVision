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
	StrStm1 >>imageFilename;
	imageFilename += EndLocationType;
	return Location + imageFilename;
}

/*按下esc退出，按下空格键截取图像*/
int main()
{
	VideoCapture capture(0);

	char c;
	int temp = 0;//计数器
	while (1)
	{
		Mat frame; //定义frame存储视屏帧
		capture >> frame;  //读取当前帧
		imshow("video capture", frame);   //显示当前帧
		c = waitKey(30);    //延时
		if (c == ' ')
		{
			imwrite(FileLocation("D:\\", temp, ".jpg"), frame);
			temp++;
		}
		if (c == 27)
		{
			break;
		}
	}

	return 0;

}