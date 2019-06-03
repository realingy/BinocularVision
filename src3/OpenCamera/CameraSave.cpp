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
	StrStm1 >>imageFilename;
	imageFilename += EndLocationType;
	return Location + imageFilename;
}

/*����esc�˳������¿ո����ȡͼ��*/
int main()
{
	VideoCapture capture(0);

	char c;
	int temp = 0;//������
	while (1)
	{
		Mat frame; //����frame�洢����֡
		capture >> frame;  //��ȡ��ǰ֡
		imshow("video capture", frame);   //��ʾ��ǰ֡
		c = waitKey(30);    //��ʱ
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