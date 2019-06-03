#include<opencv2/opencv.hpp>
#include<io.h>

using namespace cv;
using namespace std;

/*
���룺
	Location Ϊ�ļ������ڵ�λ��
	numΪ�ļ����������ַ���ʽ��
	EndLocationTypeΪ�ļ��ĺ�׺
�����
	FilesNameΪĿ¼�ļ��������и����͵��ļ����루Ŀ¼+�ļ���+�ļ����ͣ�
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

/*����esc�˳������¿ո����ȡͼ��*/
int main()
{
	Mat srcimage;
	char c;
	int temp = 0;//������
	VideoCapture cam(1);
	if (!cam.isOpened())
	{
		exit(0);
	}
	namedWindow("������ͷ��");
	while (true)
	{
		cam >> srcimage;
		imshow("������ͷ��", srcimage);
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