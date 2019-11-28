//程序功能：立体匹配与测距,输出视差图(测距，即深度恢复)
//视差图空洞填充、深度图获取
//视差图保存并用MATLAB显示、图像融合、点云保存及显示

//需要修改的参数：程序中共计5个位置需要根据应用场景来修改参数（已在代码旁做了备注）

//注意：第5处需要修改的地方，用来选择是运行SGBM或者运行BM算法，二者选一运行即可。

#include <opencv2/opencv.hpp>  
#include <iostream>  
#include <fstream>

using namespace std;
using namespace cv;

//摄像头单目的分辨率########--【需要调整参数的位置1】--#############
const int imageWidth = 640;
const int imageHeight = 480;

Size imageSize = Size(imageWidth, imageHeight);

Mat rgbImageL, grayImageL;
Mat rgbImageR, grayImageR;
Mat rectifyImageL, rectifyImageR;

//图像校正之后，会对图像进行裁剪，这里的validROI就是指裁剪之后的区域  
Rect validROIL;
Rect validROIR;

Mat mapLx, mapLy, mapRx, mapRy;                   //映射表  
Mat Rl, Rr, Pl, Pr, Q;                            //校正旋转矩阵R，投影矩阵P, 重投影矩阵Q
Mat xyz;                                          //三维坐标

Point origin;                                     //鼠标按下的起始点
Rect selection;                                   //定义矩形选框
bool selectObject = false;                        //是否选择对象

Ptr<StereoBM> bm = StereoBM::create(16, 9);		  //opencv中自带的BM立体匹配接口


//########--【以下双目的标定参数为：需要调整参数的位置2】--#############
//相机双目标定的结果与如下各参数的对应关系--#############

/*左目相机标定参数------------------------
fc_left_x   0            cc_left_x
0           fc_left_y    cc_left_y
0           0            1
-----------------------------------------*/

Mat cameraMatrixL = (Mat_<double>(3, 3)
	<< 779.28782, 0.000000, 350.84667,
	   0.000000, 778.44777, 114.78532,
	   0.000000, 0.000000, 1.000000);

//镜头畸变参数
Mat distCoeffL = (Mat_<double>(5, 1) << 0.05556, 0.27900, -0.01951, 0.00751, 0.00000);
//[kc_left_01,  kc_left_02,  kc_left_03,  kc_left_04,   kc_left_05]

/*右目相机标定参数------------------------
fc_right_x   0              cc_right_x
0            fc_right_y     cc_right_y
0            0              1
-----------------------------------------*/
Mat cameraMatrixR = (Mat_<double>(3, 3)
	<< 778.05193, 0.000000, 330.90542,
	   0.000000, 776.58851, 144.82327,
	   0.000000, 0.000000, 1.000000);

//镜头畸变参数
Mat distCoeffR = (Mat_<double>(5, 1) << 0.11635, -0.00121, -0.02270, -0.00039, 0.00000);
//[kc_right_01,  kc_right_02,  kc_right_03,  kc_right_04,   kc_right_05]

Mat T = (Mat_<double>(3, 1) << -51.45259, 0.33218, 0.93544);    //T平移向量
																 //[T_01,        T_02,       T_03]

// 旋转矩阵是一维的？
Mat rec = (Mat_<double>(3, 1) << 0.02186, 0.02906, -0.00787);   //rec旋转向量
																//[rec_01,     rec_02,     rec_03]

//########--双目的标定参数填写完毕-----------------------------------------------------------------------

Mat R; //R相机外参旋转矩阵，用于中间计算

// 保存三维坐标
void saveXYZ(const char*filename, const Mat & mat) 
{ 
	//限制最大的Z距//########--【动态调整参数的位置，去掉超出距离限制的点云】--#############
	const double max_z =800 /*1.0e3*/;
	ofstream fp(filename); 
	if (!fp.is_open()) { std::cout << "open cloud point file" << endl; fp.close(); return; }
	for (int y = 0; y<mat.cols; y++)
	{ 
		for (int x = 0; x<mat.rows; x++)
		{ 
			Vec3f point = mat.at<Vec3f>(x, y); 
			if (fabs(point[2] - max_z)<FLT_EPSILON || fabs(point[2])>max_z)
				continue; 
			//符号为MATLAB显示方便
			fp << point[0] << " " << point[1] << " " << - point[2] << endl;
		}
	}
	fp.close(); 
}

// 保存视差图
void saveDistance(const char*filename, const Mat & mat)
{
	//限制最大的Z距########--【动态调整参数的位置，去掉超出距离限制的深度】--#############
	const double max_z = 1.0e3;
	Mat Dist(mat.rows, mat.cols, CV_16UC1, Scalar(255));
	for (int y = 0; y<mat.cols; y++)
	{
		for (int x = 0; x<mat.rows; x++)
		{
			Vec3f point = mat.at<Vec3f>(x, y);
			if (fabs(point[2] - max_z)<FLT_EPSILON || fabs(point[2])>max_z)
				point[2] = 0;
			Dist.at<uint16_t>(x, y) = uint16_t(point[2]);
		}
	}
	//normalize(Dist, Dist, 0, 255, NORM_MINMAX, CV_8UC1);
	//深度图像素值为mm,超过Window自带图片查看器显示范围，显示黑色，需要归一化
	//但爱奇艺播放器可以直接显示原图
	imwrite(filename, Dist);
}

//--------------------------------------------------------------------------------------------------------
void GenerateFalseMap(cv::Mat &src, cv::Mat &disp)//视差图填色
{ 
	float max_val = 255.0f;
	float map[8][4] = { { 0,0,0,114 },{ 0,0,1,185 },{ 1,0,0,114 },{ 1,0,1,174 },
	{ 0,1,0,114 },{ 0,1,1,185 },{ 1,1,0,114 },{ 1,1,1,0 } };
	float sum = 0;
	for (int i = 0; i<8; i++)
		sum += map[i][3];

	float weights[8];   
	float cumsum[8];  
	cumsum[0] = 0;
	for (int i = 0; i<7; i++) {
		weights[i] = sum / map[i][3];
		cumsum[i + 1] = cumsum[i] + map[i][3] / sum;
	}

	int height_ = src.rows;
	int width_ = src.cols;
 
	for (int v = 0; v<height_; v++) {
		for (int u = 0; u<width_; u++) {
 
			float val = std::min(std::max(src.data[v*width_ + u] / max_val, 0.0f), 1.0f);

			int i;
			for (i = 0; i<7; i++)
				if (val<cumsum[i + 1])
					break;
 
			float   w = 1.0 - (val - cumsum[i])*weights[i];
			uchar r = (uchar)((w*map[i][0] + (1.0 - w)*map[i + 1][0]) * 255.0);
			uchar g = (uchar)((w*map[i][1] + (1.0 - w)*map[i + 1][1]) * 255.0);
			uchar b = (uchar)((w*map[i][2] + (1.0 - w)*map[i + 1][2]) * 255.0);
			 
			disp.data[v*width_ * 3 + 3 * u + 0] = b; //rgb内存连续存放 
			disp.data[v*width_ * 3 + 3 * u + 1] = g;
			disp.data[v*width_ * 3 + 3 * u + 2] = r;
		}
	}
}

void saveDisp(const char* filename, const Mat& mat)
{
	FILE* fp = fopen(filename, "wt");
	fprintf(fp, "%02d\n", mat.rows);
	fprintf(fp, "%02d\n", mat.cols);
	for (int y = 0; y < mat.rows; y++)
	{
		for (int x = 0; x < mat.cols; x++)
		{
			//int disp = (int)mat.at<float>(y, x);	// 这里视差矩阵是CV_16S 格式的，故用 short 类型读取
			//fprintf(fp, "%d\n", disp);			// 若视差矩阵是 CV_32F 格式，则用 float 类型读取

			float disp = mat.at<float>(y, x);	// 这里视差矩阵是CV_16S 格式的，故用 short 类型读取
			fprintf(fp, "%f\n", disp);			// 若视差矩阵是 CV_32F 格式，则用 float 类型读取
		}
		//fprintf(fp, "\n");
	}
	fclose(fp);
}

//视差图补洞
void FixDisparity(Mat & disp, int numberOfDisparities)
{
	Mat disp1;
	float lastPixel =10;
	float minDisparity = 3;// algorithm parameters that can be modified  31
	for (int i = 0; i < disp.rows; i++)
	{
		for (int j = numberOfDisparities; j < disp.cols; j++)
		{
			if (disp.at<float>(i, j) <= minDisparity)
				disp.at<float>(i, j) = lastPixel;
			else
				lastPixel = disp.at<float>(i, j);
		}
	}
	int an = 4; // algorithm parameters that can be modified
	copyMakeBorder(disp, disp1, an, an, an, an, BORDER_REPLICATE);
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(an * 2 + 1, an * 2 + 1));
	morphologyEx(disp1, disp1, CV_MOP_OPEN, element);
	morphologyEx(disp1, disp1, CV_MOP_CLOSE, element);
	disp = disp1(Range(an, disp.rows - an), Range(an, disp.cols - an)).clone();
}

//--BM算法立体匹配--------------------------------------------------------------------
void stereo_match_bm(int, void*)
{
	//BM算法相关的参数，【需要调整参数的位置3，仅用于BM算法有效】--############
	int blockSize = 18, uniquenessRatio = 5,  numDisparities = 7;

	bm->setBlockSize(2 * blockSize + 5);                           //SAD窗口大小，5~21之间为宜
	bm->setROI1(validROIL);                                        //左右视图的有效像素区域
	bm->setROI2(validROIR);
	bm->setPreFilterCap(61);                                       //预处理滤波器值
	bm->setMinDisparity(2);                                        //最小视差，默认值为0, 可以是负值，int型
	bm->setNumDisparities(numDisparities * 16 );                   //视差窗口，即最大视差值与最小视差值之差,16的整数倍
	bm->setTextureThreshold(10);
	bm->setUniquenessRatio(uniquenessRatio);                       //视差唯一性百分比,uniquenessRatio主要可以防止误匹配
	bm->setSpeckleWindowSize(100);                                 //检查视差连通区域变化度的窗口大小
	bm->setSpeckleRange(32);                                       //32视差变化阈值，当窗口内视差变化大于阈值时，该窗口内的视差清零
	bm->setDisp12MaxDiff(-1);
	Mat disp;
	Mat disp8U = Mat(disp.rows, disp.cols, CV_8UC1);               //显示
	bm->compute(rectifyImageL, rectifyImageR, disp);               //输入图像必须为灰度图

	reprojectImageTo3D(disp, xyz, Q, true);                        //在实际求距离时，ReprojectTo3D出来的X / W, Y / W, Z / W都要乘以16(也就是W除以16)
	xyz = xyz * 16;
	saveXYZ("xyz.txt", xyz); //输出点云
	imwrite("深度图.bmp", xyz);
	disp.convertTo(disp, CV_32F, 1.0 / 16); 
//	FixDisparity(disp, 120);//
	normalize(disp, disp8U, 0, 255, NORM_MINMAX, CV_8UC1);

	medianBlur(disp8U, disp8U, 9);  //中值滤波
	saveDisp("bmdisp.txt", disp);
	Mat dispcolor(disp8U.size(), CV_8UC3);
	GenerateFalseMap(disp8U, dispcolor);
	imshow("disparity", dispcolor);
}

//SGBM匹配算法
void stereo_match_sgbm(int, void*)
{
	// 其中minDisparity是控制匹配搜索的第一个参数，代表了匹配搜索从哪里开始，
	// ndisparities表示最大搜索视差数
	// uniquenessRatio表示匹配功能函数
	// 这三个参数比较重要，可以根据实验给予参数值。
	int mindisparity = 2;                                                 //最小视差
	int SADWindowSize =16;                                                //滑动窗口的大小
	int ndisparities = 112;                                               //最大的视差，要被16整除
	cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(mindisparity, ndisparities, SADWindowSize);

	int P1 = 4 * rectifyImageL.channels() * SADWindowSize* SADWindowSize;  //惩罚系数1
	int P2 = 32 * rectifyImageL.channels() * SADWindowSize* SADWindowSize; //惩罚系数2
	sgbm->setP1(P1);
	sgbm->setP2(P2);

	sgbm->setPreFilterCap(60);                                             //滤波系数
	sgbm->setUniquenessRatio(1);                                           //代价方程概率因子
	sgbm->setSpeckleRange(50);                                             //相邻像素点的视差值浮动范围
	sgbm->setSpeckleWindowSize(32);                                        //针对散斑滤波的窗口大小
	sgbm->setDisp12MaxDiff(1);                                             //视差图的像素点检查
	sgbm->setMode(cv::StereoSGBM::MODE_SGBM_3WAY);  

	Mat disp;
	sgbm->compute(rectifyImageL, rectifyImageR, disp);

	Mat disp8U = Mat(disp.rows, disp.cols, CV_8UC1);                       //显示

	reprojectImageTo3D(disp, xyz, Q, true);                                //在实际求距离时，ReprojectTo3D出来的X / W, Y / W, Z / W都要乘以16(也就是W除以16)
	xyz = xyz * 16;
	saveXYZ("xyz.txt", xyz);//输出点云
	saveDistance("DistMap.png",xyz);//输出深度图必须是png格式 bmp,jpg会发生截断
	disp.convertTo(disp, CV_32F, 1.0 / 16);   //除以16得到真实视差值,disp.convertTo(disp, CV_32F, 1.0 );
	//FixDisparity(disp,120);//视差图补洞
	normalize(disp, disp8U, 0, 255, NORM_MINMAX, CV_8UC1);
	//medianBlur(disp8U, disp8U, 3);//中值滤波
	Mat dispcolor(disp8U.size(), CV_8UC3);

	//GenerateFalseMap(disp8U, dispcolor);//彩色视差图
	//imshow("disparity", dispcolor);
	saveDisp("sgbmdisp.txt", disp);
	imshow("disparity", disp8U);

}

//--描述：鼠标操作回调--------------------------------------------------
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
	case EVENT_LBUTTONDOWN:             //鼠标左按钮按下的事件
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);
		selectObject = true;
		cout << origin << "in world coordinate is: " << xyz.at<Vec3f>(origin) << endl;
		break;
	case EVENT_LBUTTONUP:               //鼠标左按钮释放的事件
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
			break;
	}
}

//--主函数---------------------------------------------------------------------------
int main()
{
	// Rodrigues变换
	Rodrigues(rec, R); //旋转向量和旋转矩阵的转换，R是3*3矩阵
	// --立体校正---------------------------------------------------------------------
	// --得到左右相机的校正后的旋转矩阵Rl/Rr、投影矩阵Pl/Pr，重投影矩阵Q。
	stereoRectify(cameraMatrixL, distCoeffL, cameraMatrixR, distCoeffR, imageSize, R, T, Rl, Rr, Pl, Pr, Q, CALIB_ZERO_DISPARITY,
		0, imageSize, &validROIL, &validROIR);
	initUndistortRectifyMap(cameraMatrixL, distCoeffL, Rl, Pl, imageSize, CV_32FC1, mapLx, mapLy);
	initUndistortRectifyMap(cameraMatrixR, distCoeffR, Rr, Pr, imageSize, CV_32FC1, mapRx, mapRy);

	std::cout << "rec:\n " << rec << std::endl;
	std::cout << "R:\n " << R << std::endl;
	std::cout << "Rl:\n " << Rl << std::endl;
	std::cout << "Rr:\n " << Rr << std::endl;
	std::cout << "Pl:\n " << Pl << std::endl;
	std::cout << "Pr:\n " << Pr << std::endl;
	std::cout << "Q:\n " << Q << std::endl;

	//--读取图片，【需要调整参数的位置4】------------------------------------------
	rgbImageL = imread("L.bmp", IMREAD_COLOR);
	rgbImageR = imread("R.bmp", IMREAD_COLOR);
	cvtColor(rgbImageL, grayImageL, CV_BGR2GRAY);
	cvtColor(rgbImageR, grayImageR, CV_BGR2GRAY);

	namedWindow("ImageL Before Rectify", WINDOW_NORMAL);  imshow("ImageL Before Rectify", grayImageL);
	namedWindow("ImageR Before Rectify", WINDOW_NORMAL);  imshow("ImageR Before Rectify", grayImageR);

	grayImageL.copyTo(rectifyImageL);
	grayImageR.copyTo(rectifyImageR);

	//--经过remap之后，左右相机的图像已经共面并且行对准----------------------------
	remap(grayImageL, rectifyImageL, mapLx, mapLy, INTER_LINEAR);
	remap(grayImageR, rectifyImageR, mapRx, mapRy, INTER_LINEAR);

	//--把校正结果显示出来---------------------------------------------------------
	Mat rgbRectifyImageL, rgbRectifyImageR;
	cvtColor(rectifyImageL, rgbRectifyImageL, CV_GRAY2BGR);  
	cvtColor(rectifyImageR, rgbRectifyImageR, CV_GRAY2BGR);

	namedWindow("ImageL After Rectify", WINDOW_NORMAL); imshow("ImageL After Rectify", rgbRectifyImageL);
	namedWindow("ImageR After Rectify", WINDOW_NORMAL); imshow("ImageR After Rectify", rgbRectifyImageR);

	//--显示在同一张图上-----------------------------------------------------------
	Mat canvas;
	double sf;
	int w, h;
	sf = 600. / MAX(imageSize.width, imageSize.height);
	w = cvRound(imageSize.width * sf);
	h = cvRound(imageSize.height * sf);
	canvas.create(h, w * 2, CV_8UC3); //注意通道

	//--左图像画到画布上-----------------------------------------------------------
	Mat canvasPart = canvas(Rect(w * 0, 0, w, h));                                //得到画布的一部分
	resize(rgbRectifyImageL, canvasPart, canvasPart.size(), 0, 0, INTER_AREA);    //把图像缩放到跟canvasPart一样大小
	Rect vroiL(cvRound(validROIL.x*sf), cvRound(validROIL.y*sf),                  //获得被截取的区域
		cvRound(validROIL.width*sf), cvRound(validROIL.height*sf));
	//rectangle(canvasPart, vroiL, Scalar(0, 0, 255), 3, 8);                     
	cout << "Painted ImageL" << endl;

	//--右图像画到画布上----------------------------------------------------------
	canvasPart = canvas(Rect(w, 0, w, h));                                        //获得画布的另一部分
	resize(rgbRectifyImageR, canvasPart, canvasPart.size(), 0, 0, INTER_LINEAR);
	Rect vroiR(cvRound(validROIR.x * sf), cvRound(validROIR.y*sf),
		cvRound(validROIR.width * sf), cvRound(validROIR.height * sf));
	//rectangle(canvasPart, vroiR, Scalar(0, 0, 255), 3, 8);
	cout << "Painted ImageR" << endl;

	//--画上对应的线条-----------------------------------------------------------
	for (int i = 0; i < canvas.rows; i += 16)
		line(canvas, Point(0, i), Point(canvas.cols, i), Scalar(0, 0, 255), 1, 8);
	namedWindow("rectified", WINDOW_NORMAL); imshow("rectified", canvas);

	//--显示结果-----------------------------------------------------------------
	namedWindow("disparity", WINDOW_NORMAL);

	//--创建SAD窗口 Trackbar-----------------------------------------------------
	//createTrackbar("BlockSize:\n", "disparity", &blockSize, 8, stereo_match);

	//--创建视差唯一性百分比窗口 Trackbar----------------------------------------
	//createTrackbar("UniquenessRatio:\n", "disparity", &uniquenessRatio, 50, stereo_match);

	//--创建视差窗口 Trackbar----------------------------------------------------
	//createTrackbar("NumDisparities:\n", "disparity", &numDisparities, 16, stereo_match);

	//--鼠标响应函数setMouseCallback(窗口名称, 鼠标回调函数, 传给回调函数的参数，一般取0)---
	setMouseCallback("disparity", onMouse, 0);

	//--【需要调整参数的位置5】，本行调用sgbm算法，下一行调用BM算法，二选一进行距离测量。
	stereo_match_sgbm(0, 0);
	//stereo_match_bm(0, 0);

	waitKey(0);
	return 0;
}