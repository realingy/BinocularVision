#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

#if 0
string FileLocation(string Location, int num, string EndLocationType)
{
	string imageFilename;
	stringstream StrStm1;
	StrStm1 << num + 1;
	StrStm1 >> imageFilename;
	imageFilename += EndLocationType;
	return Location + imageFilename;
}

int main()
{
	for (int i = 0; i < 31; i++)
	{
		Mat image = imread(FileLocation("G:/RightCameraPicture/", i, ".jpg"));
		Mat loadimage = image(Rect(0, 0, image.cols / 2, image.rows));
		imwrite(FileLocation("G:/SegPicture/right/", i, ".jpg"), loadimage);
	}
	return 0;
}
#endif