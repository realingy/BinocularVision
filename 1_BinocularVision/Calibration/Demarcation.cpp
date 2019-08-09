#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/calib3d/calib3d.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<iostream>
#include<fstream>
#include<vector>

using namespace cv;
using namespace std;

int main()
{
	std::vector<std::string>filenames;

	//相机拍摄的标定板的图片路径以及标定结果的保存文件。
	string infilename = "left.txt";
	string outfilename = "left_calib.txt";

	//标定所用图片的文件路径，每一行保存一个标定图片的路径
	ifstream fin(infilename);
	ofstream fout(outfilename);

	//读取每一幅图像，从中提取角点，然后对角点进行亚像素精细化，获取每个角点在像素坐标系中的坐标
	cout << "开始提取角点.....\n"<< endl;
	int imagecount = 0;//图像的数量
	Size imagesize;//图像的尺寸
	Size boardSize = Size(11, 8);//标定板上每行每列的内角点数目
	vector<Point2f>imagePointsBuf;//保存每幅图像上检测到的角点坐标
	vector<vector<Point2f>>imagePointsSeq;//保存所有图像上检测到的角点的坐标
	char filename[100];

	if (fin.is_open())
	{
		while (!fin.eof())//是否读取完毕
		{
			fin.getline(filename, sizeof(filename) / sizeof(char));//一次读取一行
			filenames.push_back(filename);//保存文件名
			Mat imageInput = imread(filename);
			if (imagecount == 0)//读入第一张图片时，获取图像的宽和高信息。
			{
				imagesize.width = imageInput.cols;
				imagesize.height = imageInput.rows;
				cout << "imagewidth=" << imagesize.width;
				cout << "\nimageheight=" << imagesize.height;
			}
			imagecount++;
			/*
			开始提取每一张图像上的角点
			*/
			if (findChessboardCorners(imageInput, boardSize, imagePointsBuf) == 0)
			{
				cout << "找不到角点";
				exit(1);
			}
			else
			{
				Mat viewGray;
				cvtColor(imageInput, viewGray, COLOR_BGR2GRAY);//将图片转化为灰度图，转化为8位图检测效率高
				find4QuadCornerSubpix(viewGray, imagePointsBuf, Size(3, 3));//亚像素精细化，对粗提取的角点进行精确化 ,size()为角点搜索窗口的尺寸
				imagePointsSeq.push_back(imagePointsBuf);//保存亚像素化后的角点坐标
				drawChessboardCorners(viewGray, boardSize, imagePointsBuf, true);//在图像上显示角点的位置
				imshow("Camera Calibration ", viewGray);
				imwrite("test.jpg", viewGray);
				waitKey(500);
			}
		}

		cout << "\nimagecount=" << imagecount; //标定图片的数量

		//计算每张图片上的角点总数
		int cornerNum = boardSize.width*boardSize.height;
		//计算所有标定图片的角点总数目
		int total = imagePointsSeq.size()*cornerNum;
		cout << "所有标定图片总的角点数目=：\n" << total;
		for (int i = 0; i < total; i++)
		{
			int num = i / cornerNum;//cornerNum是每张图片上的角点个数
			int p = i % cornerNum;
			if ((p==0))//为了进行调试，读取图片的索引
			{
				cout << "\n第" << num + 1 << "张图片的数据-->:\n";
			}
			
			/*输出每张图片上各个角点的位置信息*/
			cout << p + 1 << ":(" << imagePointsSeq[num][p].x;
			cout << imagePointsSeq[num][p].y << ")\n";
			if ((p+1)%3==0)
			{
				cout << endl;
			}
		}
		cout << "角点提取完成";
		/*开始标定<摄像机标定时，世界坐标系原点坐标位于标定板左上角（第一个方格的左上角）*/

		cout << "开始标定";
		//棋盘三维信息，设置棋盘在世界坐标系的坐标
		Size squareSize = Size(5,5);//表定板上每个棋盘格的实际大小
		vector<int>pointCounts;//每幅图像的角点数量
		vector<vector<Point3f>>objectPoints;//所有标定图像的角点在世界坐标系中的真实坐标
		Mat cameraMatrix = Mat(3, 3, CV_64F, Scalar::all(0));//相机的内参矩阵[fx,fy,r,u0,0,v0,0,0,1]
		Mat distCoeffs = Mat(1, 5, CV_64F, Scalar::all(0));//5个畸变参数：k1,k2,p1,p2,k3
		vector<Mat>rvecsMat;//旋转矩阵
		vector<Mat>tvecsMat;//平移向量

		/*初始化标定板上角点的三维坐标*/
		int i, j, t;
		for ( t = 0; t < imagecount; t++)
		{
			vector<Point3f>temPointSet;
			for ( i = 0; i < boardSize.height; i++)
			{
				for (j = 0; j < boardSize.width; j++)
				{
					Point3f realPoint;
					realPoint.x = i * squareSize.width;
					realPoint.y = j * squareSize.height;
					realPoint.z= 0;//利用张氏标定时，假设的世界坐标系原点所在的平面与图像平面重合，所以标定板的z方向的坐标为0
					temPointSet.push_back(realPoint);
				}
			}
			objectPoints.push_back(temPointSet);//保存所有角点的世界坐标
		}
		/*记录每幅图像中的角点数量，假定每幅图像都可以看到完整的标定板*/
		for ( i = 0; i < imagecount; i++)
		{
			pointCounts.push_back(boardSize.width*boardSize.height);
		}
		/*开始标定*/
		int flag = 0; //| cv::CALIB_FIX_K3
		calibrateCamera(objectPoints, imagePointsSeq, imagesize, 
			cameraMatrix, distCoeffs, rvecsMat, tvecsMat,flag);
		cout << "标定完成\n";
		/*对标定结果进行评价*/
		cout << "开始评价标定结果\n"; 
		double totalErr=0;//所有图像的平均误差总和
		double err = 0;//每幅图像的平均误差
		vector<Point2f>imagePoints2;//保存根据标定结果重新计算的到的投影点的坐标

		cout << "每幅图像的标定误差\n";
		for ( i = 0; i < imagecount; i++)
		{
			vector<Point3f>tempPointset = objectPoints[i];//得到每张标定板图像的原世界坐标
			/*通过得到的摄像机的内外参数，对空间的三维点进行重投影计算，得到新的投影点ImagePoints2(在像素坐标系下的点的坐标）*/
			projectPoints(tempPointset, rvecsMat[i], tvecsMat[i], cameraMatrix, distCoeffs, imagePoints2);
			//计算新的投影点和旧的投影点之间的误差
			vector<Point2f>tempImagePoint = imagePointsSeq[i];//得到每张标定板上的角点原像素坐标
			Mat tempImagePointMat = Mat(1, tempImagePoint.size(), CV_32FC2);//将标定板上的角点拉直排布
			Mat imagePoints2Mat = Mat(1, imagePoints2.size(), CV_32FC2);//将标定板上的角点拉直排布
			for (int j = 0; j < tempImagePoint.size(); j++)//分别得到标定板原来的各个角点的像素坐标
			{
				//得到由世界坐标根据标定结果重投影得到的像素坐标
				imagePoints2Mat.at<Vec2f>(0, j) = Vec2f(imagePoints2[j].x, imagePoints2[j].y);
				//得到标定板原来的各个焦点的像素坐标
				tempImagePointMat.at<Vec2f>(0, j) = Vec2f(tempImagePoint[j].x, tempImagePoint[j].y);
			}
			/*计算每幅图像上标定的误差*/
			err = norm(imagePoints2Mat, tempImagePointMat, NORM_L2);
			totalErr += err /= pointCounts[i];//求取所有标定图像的平均误差
			cout << "第" << i + 1 << "幅图像的平均误差：" << err << "像素\n";
			fout << "第" << i + 1 << "幅图像的平均误差：" << err << "像素\n";
		}
		/*每张图像的平均总误差*/
		cout << "总体平均误差" << totalErr / imagecount << "像素\n";
		fout << "总体平均误差" << totalErr / imagecount << "像素\n";
		cout << "评价完成\n";
		cout << "保存标定结果。。。。\n";
		Mat rotationMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
		fout << "相机内参数矩阵\n"  ;
		fout << cameraMatrix;
		fout << "畸变系数\n";
		fout << distCoeffs;
		for (int i = 0; i < imagecount; i++)
		{
			fout << "第" << i + 1 << "幅图像的旋转向量:\n";
			fout << rvecsMat[i];
			Rodrigues(rvecsMat[i], rotationMatrix);
			fout << "第" << i + 1 << "幅图像的旋转矩阵:\n";
			fout << rotationMatrix;
			fout << "第" << i + 1 << "幅图像的平移向量:\n";
			fout << tvecsMat[i];
		}
		cout << "保存完成\n";
		/*显示标定结果*/
		Mat mapx = Mat(imagesize, CV_32FC1);
		Mat mapy = Mat(imagesize, CV_32FC1);
		Mat R = Mat::eye(3, 3, CV_32F);
		cout << "显示矫正图像\n";
		for (int i = 0; i !=imagecount; i++)
		{
			cout << "Frame#" << i + 1;
			/*计算凸显畸变校正的映射矩阵mapx,mapy(不进行立体矫正，立体校正需要使用双摄*/
			initUndistortRectifyMap(cameraMatrix, distCoeffs, R, cameraMatrix, imagesize, CV_32FC1, mapx, mapy);
			Mat imageSource = imread(filenames[i]);
			Mat newimage = imageSource.clone();
			/*另外一种不需要转换矩阵的方式
			udistort(imageSource,newiamge,cameraMatrix,distCoeffs);
			*/
			remap(imageSource, newimage, mapx, mapy, INTER_LINEAR);
			imshow("原始图像",imageSource );
			imshow("矫正后的图像", newimage);
			waitKey();
		}
     }
}

