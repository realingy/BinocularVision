#if 1
// 根据视差图像和相机标定参数，基于三角几何原理，得到点云结果。
#define  _SCL_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>  
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/io/io.h>  
#include <pcl/io/pcd_io.h>  
#include <iostream>  

using namespace pcl;
using namespace std;
using namespace cv;

void viewerOneOff(visualization::PCLVisualizer& viewer)
{
	viewer.setBackgroundColor(0.0, 0.0, 0.0);
}

int user_data;
Mat color;

Mat rectifyImageL, rectifyImageR;

Rect validROIL; //图像校正之后，会对图像进行裁剪，这里的validROI就是指裁剪之后的区域  
Rect validROIR;
Mat Q;
Mat xyz; //三维坐标
Ptr<StereoBM> bm = StereoBM::create(16, 9);
int blockSize = 1, uniquenessRatio = 0, numDisparities = 5;

void stereo_match_sgbm(int, void*) //SGBM匹配算法
{
	PointCloud<PointXYZRGB> cloud_a;
	PointCloud<PointXYZRGB>::Ptr cloud(new PointCloud<PointXYZRGB>);
	color = imread("rectified.png");  //校正好的左相机图像
	imshow("color",color);
	
	int rowNumber = color.rows;
	int colNumber = color.cols;

	cloud_a.height = rowNumber;
	cloud_a.width = colNumber;
	cloud_a.points.resize(cloud_a.width * cloud_a.height);

	bm->setBlockSize(2 * blockSize + 5); //SAD窗口大小，5~21之间为宜
	bm->setROI1(validROIL);
	bm->setROI2(validROIR);
	bm->setPreFilterCap(31);
	bm->setMinDisparity(0);  //最小视差，默认值为0, 可以是负值，int型
	bm->setNumDisparities(numDisparities * 16 + 16); //视差窗口，即最大视差值与最小视差值之差,窗口大小必须是16的整数倍，int型
	bm->setTextureThreshold(10);
	bm->setUniquenessRatio(uniquenessRatio); //uniquenessRatio主要可以防止误匹配
	bm->setSpeckleWindowSize(100);
	bm->setSpeckleRange(32);
	bm->setDisp12MaxDiff(-1);
	Mat disp, disp8;
	bm->compute(rectifyImageL, rectifyImageR, disp); //输入图像必须为灰度图
	disp.convertTo(disp8, CV_8U, 255 / ((numDisparities * 16 + 16)*16.)); //计算出的视差是CV_16S格式
	reprojectImageTo3D(disp, xyz, Q, true); //在实际求距离时，ReprojectTo3D出来的X / W, Y / W, Z / W都要乘以16(也就是W除以16)，才能得到正确的三维坐标信息。
	xyz = xyz * 16;
	imshow("disparity", disp8);

	waitKey(1);
	for (unsigned int u = 0; u < rowNumber; ++u)
	{
		for (unsigned int v = 0; v < colNumber; ++v)
		{
			/*unsigned int num = rowNumber*colNumber-(u*colNumber + v)-1;*/
			unsigned int num = u*colNumber + v;

			cloud_a.points[num].b = color.at<Vec3b>(u, v)[0];
			cloud_a.points[num].g = color.at<Vec3b>(u, v)[1];
			cloud_a.points[num].r = color.at<Vec3b>(u, v)[2];

			cloud_a.points[num].x = u; //xyz.at<Vec3f>(u, v)[0];
			cloud_a.points[num].y = v;//xyz.at<Vec3f>(u, v)[1];
			cloud_a.points[num].z = xyz.at<Vec3f>(u, v)[2];
		}
	}

	*cloud = cloud_a;

	visualization::CloudViewer viewer("Cloud Viewer");

	viewer.showCloud(cloud);

	viewer.runOnVisualizationThreadOnce(viewerOneOff);

	while (!viewer.wasStopped())
	{
		user_data = 9;
	}
}

Point origin;                                     //鼠标按下的起始点
Rect selection;                                   //定义矩形选框
bool selectObject = false;                        //是否选择对象

//--描述：鼠标操作回调--
static void onMouse(int event, int x, int y, int, void*)
{
	if (selectObject)
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);
		selection.height = std::abs(y - origin.y);
	}

	switch (event)
	{
	case EVENT_LBUTTONDOWN:   //鼠标左按钮按下的事件
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);
		selectObject = true;
		cout << origin << "in world coordinate is: " << xyz.at<Vec3f>(origin)<< endl<< " ori"<<xyz.at<Vec3f>(origin)[0];
		break;
	case EVENT_LBUTTONUP:    //鼠标左按钮释放的事件
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
			break;
	}
}


int main()
{
	Mat intrMatFirst, intrMatSec, distCoeffsFirst, distCoffesSec;
	Mat R, T, E, F, RFirst, RSec, PFirst, PSec;

	Rect validRoi[2];
	Mat viewLeft, viewRight;

	viewLeft = imread(".\\picture\\left_9.png", 1);//cameraIdLef  参数为相机采集的图像路径
	viewRight = imread(".\\picture\\right_9.png", 1); //cameraIRight

	imshow("viewLeft", viewLeft);
	waitKey(40);
	cout << "done" << endl;

	Size imageSize = viewLeft.size();
	FileStorage fs("intrinsics.yml", FileStorage::READ);
	if (fs.isOpened())
	{
		cout << "read";
		fs["M1"] >> intrMatFirst;fs["D1"] >> distCoeffsFirst;
		fs["M2"] >> intrMatSec;fs["D2"] >> distCoffesSec;

		fs["R"] >> R;
		fs["T"] >> T;
		fs["Q"] >> Q;
		cout << "M1" << intrMatFirst << endl << distCoeffsFirst;

		fs.release();
	}


	cout << "stereo rectify..." << endl;
	stereoRectify(intrMatFirst, distCoeffsFirst, intrMatSec, distCoffesSec, imageSize, R, T, RFirst,
		RSec, PFirst, PSec, Q, CALIB_ZERO_DISPARITY, -1, imageSize, &validROIL, &validROIR);

	//stereoRectify
	Mat rmapFirst[2], rmapSec[2], rviewFirst, rviewSec;
	initUndistortRectifyMap(intrMatFirst, distCoeffsFirst, RFirst, PFirst,
		imageSize, CV_16SC2, rmapFirst[0], rmapFirst[1]);//CV_16SC2
	initUndistortRectifyMap(intrMatSec, distCoffesSec, RSec, PSec,//CV_16SC2
		imageSize, CV_16SC2, rmapSec[0], rmapSec[1]);

	remap(viewLeft, rectifyImageL, rmapFirst[0], rmapFirst[1], INTER_LINEAR);
	remap(viewRight, rectifyImageR, rmapSec[0], rmapSec[1], INTER_LINEAR);
	imwrite("left_picture.png",rectifyImageL);
	cvtColor(rectifyImageL, rectifyImageL, CV_BGR2GRAY);
	cvtColor(rectifyImageR, rectifyImageR, CV_BGR2GRAY);

	imshow("remap_left", rectifyImageL);
	imshow("remap_right", rectifyImageR);

	//pcltest();
	//while (1);

	//--显示结果-------------------------------------------------------------------------------------
	namedWindow("disparity", WINDOW_NORMAL);

	//--创建SAD窗口 Trackbar-------------------------------------------------------------------------
	createTrackbar("BlockSize:\n", "disparity", &blockSize, 8, stereo_match_sgbm);

	//--创建视差唯一性百分比窗口 Trackbar------------------------------------------------------------
	createTrackbar("UniquenessRatio:\n", "disparity", &uniquenessRatio, 50, stereo_match_sgbm);

	//--创建视差窗口 Trackbar------------------------------------------------------------------------
	createTrackbar("NumDisparities:\n", "disparity", &numDisparities, 16, stereo_match_sgbm);

	//--鼠标响应函数setMouseCallback(窗口名称, 鼠标回调函数, 传给回调函数的参数，一般取0)------------
	setMouseCallback("disparity", onMouse, 0);

	stereo_match_sgbm(0, 0);   //--【需要调整参数的位置5】，本行调用sgbm算法

	waitKey(0);//必须要加waitKey ，否则可能存在无法显示图像问题

	// while (1);
	return 0;

}


#endif