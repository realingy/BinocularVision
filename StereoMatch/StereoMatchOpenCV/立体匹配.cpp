//�����ܣ�����ƥ������,����Ӳ�ͼ(��࣬����Ȼָ�)
//�Ӳ�ͼ�ն���䡢���ͼ��ȡ
//�Ӳ�ͼ���沢��MATLAB��ʾ��ͼ���ںϡ����Ʊ��漰��ʾ

//��Ҫ�޸ĵĲ����������й���5��λ����Ҫ����Ӧ�ó������޸Ĳ��������ڴ��������˱�ע��

//ע�⣺��5����Ҫ�޸ĵĵط�������ѡ��������SGBM��������BM�㷨������ѡһ���м��ɡ�

#include <opencv2/opencv.hpp>  
#include <iostream>  
#include <fstream>

using namespace std;
using namespace cv;

//����ͷ��Ŀ�ķֱ���########--����Ҫ����������λ��1��--#############
const int imageWidth = 640;
const int imageHeight = 480;

Size imageSize = Size(imageWidth, imageHeight);

Mat rgbImageL, grayImageL;
Mat rgbImageR, grayImageR;
Mat rectifyImageL, rectifyImageR;

//ͼ��У��֮�󣬻��ͼ����вü��������validROI����ָ�ü�֮�������  
Rect validROIL;
Rect validROIR;

Mat mapLx, mapLy, mapRx, mapRy;                   //ӳ���  
Mat Rl, Rr, Pl, Pr, Q;                            //У����ת����R��ͶӰ����P, ��ͶӰ����Q
Mat xyz;                                          //��ά����

Point origin;                                     //��갴�µ���ʼ��
Rect selection;                                   //�������ѡ��
bool selectObject = false;                        //�Ƿ�ѡ�����

Ptr<StereoBM> bm = StereoBM::create(16, 9);		  //opencv���Դ���BM����ƥ��ӿ�


//########--������˫Ŀ�ı궨����Ϊ����Ҫ����������λ��2��--#############
//���˫Ŀ�궨�Ľ�������¸������Ķ�Ӧ��ϵ--#############

/*��Ŀ����궨����------------------------
fc_left_x   0            cc_left_x
0           fc_left_y    cc_left_y
0           0            1
-----------------------------------------*/

Mat cameraMatrixL = (Mat_<double>(3, 3)
	<< 779.28782, 0.000000, 350.84667,
	   0.000000, 778.44777, 114.78532,
	   0.000000, 0.000000, 1.000000);

//��ͷ�������
Mat distCoeffL = (Mat_<double>(5, 1) << 0.05556, 0.27900, -0.01951, 0.00751, 0.00000);
//[kc_left_01,  kc_left_02,  kc_left_03,  kc_left_04,   kc_left_05]

/*��Ŀ����궨����------------------------
fc_right_x   0              cc_right_x
0            fc_right_y     cc_right_y
0            0              1
-----------------------------------------*/
Mat cameraMatrixR = (Mat_<double>(3, 3)
	<< 778.05193, 0.000000, 330.90542,
	   0.000000, 776.58851, 144.82327,
	   0.000000, 0.000000, 1.000000);

//��ͷ�������
Mat distCoeffR = (Mat_<double>(5, 1) << 0.11635, -0.00121, -0.02270, -0.00039, 0.00000);
//[kc_right_01,  kc_right_02,  kc_right_03,  kc_right_04,   kc_right_05]

Mat T = (Mat_<double>(3, 1) << -51.45259, 0.33218, 0.93544);    //Tƽ������
																 //[T_01,        T_02,       T_03]

// ��ת������һά�ģ�
Mat rec = (Mat_<double>(3, 1) << 0.02186, 0.02906, -0.00787);   //rec��ת����
																//[rec_01,     rec_02,     rec_03]

//########--˫Ŀ�ı궨������д���-----------------------------------------------------------------------

Mat R; //R��������ת���������м����

// ������ά����
void saveXYZ(const char*filename, const Mat & mat) 
{ 
	//��������Z��//########--����̬����������λ�ã�ȥ�������������Ƶĵ��ơ�--#############
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
			//����ΪMATLAB��ʾ����
			fp << point[0] << " " << point[1] << " " << - point[2] << endl;
		}
	}
	fp.close(); 
}

// �����Ӳ�ͼ
void saveDistance(const char*filename, const Mat & mat)
{
	//��������Z��########--����̬����������λ�ã�ȥ�������������Ƶ���ȡ�--#############
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
	//���ͼ����ֵΪmm,����Window�Դ�ͼƬ�鿴����ʾ��Χ����ʾ��ɫ����Ҫ��һ��
	//�������ղ���������ֱ����ʾԭͼ
	imwrite(filename, Dist);
}

//--------------------------------------------------------------------------------------------------------
void GenerateFalseMap(cv::Mat &src, cv::Mat &disp)//�Ӳ�ͼ��ɫ
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
			 
			disp.data[v*width_ * 3 + 3 * u + 0] = b; //rgb�ڴ�������� 
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
			//int disp = (int)mat.at<float>(y, x);	// �����Ӳ������CV_16S ��ʽ�ģ����� short ���Ͷ�ȡ
			//fprintf(fp, "%d\n", disp);			// ���Ӳ������ CV_32F ��ʽ������ float ���Ͷ�ȡ

			float disp = mat.at<float>(y, x);	// �����Ӳ������CV_16S ��ʽ�ģ����� short ���Ͷ�ȡ
			fprintf(fp, "%f\n", disp);			// ���Ӳ������ CV_32F ��ʽ������ float ���Ͷ�ȡ
		}
		//fprintf(fp, "\n");
	}
	fclose(fp);
}

//�Ӳ�ͼ����
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

//--BM�㷨����ƥ��--------------------------------------------------------------------
void stereo_match_bm(int, void*)
{
	//BM�㷨��صĲ���������Ҫ����������λ��3��������BM�㷨��Ч��--############
	int blockSize = 18, uniquenessRatio = 5,  numDisparities = 7;

	bm->setBlockSize(2 * blockSize + 5);                           //SAD���ڴ�С��5~21֮��Ϊ��
	bm->setROI1(validROIL);                                        //������ͼ����Ч��������
	bm->setROI2(validROIR);
	bm->setPreFilterCap(61);                                       //Ԥ�����˲���ֵ
	bm->setMinDisparity(2);                                        //��С�ӲĬ��ֵΪ0, �����Ǹ�ֵ��int��
	bm->setNumDisparities(numDisparities * 16 );                   //�Ӳ�ڣ�������Ӳ�ֵ����С�Ӳ�ֵ֮��,16��������
	bm->setTextureThreshold(10);
	bm->setUniquenessRatio(uniquenessRatio);                       //�Ӳ�Ψһ�԰ٷֱ�,uniquenessRatio��Ҫ���Է�ֹ��ƥ��
	bm->setSpeckleWindowSize(100);                                 //����Ӳ���ͨ����仯�ȵĴ��ڴ�С
	bm->setSpeckleRange(32);                                       //32�Ӳ�仯��ֵ�����������Ӳ�仯������ֵʱ���ô����ڵ��Ӳ�����
	bm->setDisp12MaxDiff(-1);
	Mat disp;
	Mat disp8U = Mat(disp.rows, disp.cols, CV_8UC1);               //��ʾ
	bm->compute(rectifyImageL, rectifyImageR, disp);               //����ͼ�����Ϊ�Ҷ�ͼ

	reprojectImageTo3D(disp, xyz, Q, true);                        //��ʵ�������ʱ��ReprojectTo3D������X / W, Y / W, Z / W��Ҫ����16(Ҳ����W����16)
	xyz = xyz * 16;
	saveXYZ("xyz.txt", xyz); //�������
	imwrite("���ͼ.bmp", xyz);
	disp.convertTo(disp, CV_32F, 1.0 / 16); 
//	FixDisparity(disp, 120);//
	normalize(disp, disp8U, 0, 255, NORM_MINMAX, CV_8UC1);

	medianBlur(disp8U, disp8U, 9);  //��ֵ�˲�
	saveDisp("bmdisp.txt", disp);
	Mat dispcolor(disp8U.size(), CV_8UC3);
	GenerateFalseMap(disp8U, dispcolor);
	imshow("disparity", dispcolor);
}

//SGBMƥ���㷨
void stereo_match_sgbm(int, void*)
{
	// ����minDisparity�ǿ���ƥ�������ĵ�һ��������������ƥ�����������￪ʼ��
	// ndisparities��ʾ��������Ӳ���
	// uniquenessRatio��ʾƥ�书�ܺ���
	// �����������Ƚ���Ҫ�����Ը���ʵ��������ֵ��
	int mindisparity = 2;                                                 //��С�Ӳ�
	int SADWindowSize =16;                                                //�������ڵĴ�С
	int ndisparities = 112;                                               //�����ӲҪ��16����
	cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(mindisparity, ndisparities, SADWindowSize);

	int P1 = 4 * rectifyImageL.channels() * SADWindowSize* SADWindowSize;  //�ͷ�ϵ��1
	int P2 = 32 * rectifyImageL.channels() * SADWindowSize* SADWindowSize; //�ͷ�ϵ��2
	sgbm->setP1(P1);
	sgbm->setP2(P2);

	sgbm->setPreFilterCap(60);                                             //�˲�ϵ��
	sgbm->setUniquenessRatio(1);                                           //���۷��̸�������
	sgbm->setSpeckleRange(50);                                             //�������ص���Ӳ�ֵ������Χ
	sgbm->setSpeckleWindowSize(32);                                        //���ɢ���˲��Ĵ��ڴ�С
	sgbm->setDisp12MaxDiff(1);                                             //�Ӳ�ͼ�����ص���
	sgbm->setMode(cv::StereoSGBM::MODE_SGBM_3WAY);  

	Mat disp;
	sgbm->compute(rectifyImageL, rectifyImageR, disp);

	Mat disp8U = Mat(disp.rows, disp.cols, CV_8UC1);                       //��ʾ

	reprojectImageTo3D(disp, xyz, Q, true);                                //��ʵ�������ʱ��ReprojectTo3D������X / W, Y / W, Z / W��Ҫ����16(Ҳ����W����16)
	xyz = xyz * 16;
	saveXYZ("xyz.txt", xyz);//�������
	saveDistance("DistMap.png",xyz);//������ͼ������png��ʽ bmp,jpg�ᷢ���ض�
	disp.convertTo(disp, CV_32F, 1.0 / 16);   //����16�õ���ʵ�Ӳ�ֵ,disp.convertTo(disp, CV_32F, 1.0 );
	//FixDisparity(disp,120);//�Ӳ�ͼ����
	normalize(disp, disp8U, 0, 255, NORM_MINMAX, CV_8UC1);
	//medianBlur(disp8U, disp8U, 3);//��ֵ�˲�
	Mat dispcolor(disp8U.size(), CV_8UC3);

	//GenerateFalseMap(disp8U, dispcolor);//��ɫ�Ӳ�ͼ
	//imshow("disparity", dispcolor);
	saveDisp("sgbmdisp.txt", disp);
	imshow("disparity", disp8U);

}

//--�������������ص�--------------------------------------------------
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
	case EVENT_LBUTTONDOWN:             //�����ť���µ��¼�
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);
		selectObject = true;
		cout << origin << "in world coordinate is: " << xyz.at<Vec3f>(origin) << endl;
		break;
	case EVENT_LBUTTONUP:               //�����ť�ͷŵ��¼�
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
			break;
	}
}

//--������---------------------------------------------------------------------------
int main()
{
	// Rodrigues�任
	Rodrigues(rec, R); //��ת��������ת�����ת����R��3*3����
	// --����У��---------------------------------------------------------------------
	// --�õ����������У�������ת����Rl/Rr��ͶӰ����Pl/Pr����ͶӰ����Q��
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

	//--��ȡͼƬ������Ҫ����������λ��4��------------------------------------------
	rgbImageL = imread("L.bmp", IMREAD_COLOR);
	rgbImageR = imread("R.bmp", IMREAD_COLOR);
	cvtColor(rgbImageL, grayImageL, CV_BGR2GRAY);
	cvtColor(rgbImageR, grayImageR, CV_BGR2GRAY);

	namedWindow("ImageL Before Rectify", WINDOW_NORMAL);  imshow("ImageL Before Rectify", grayImageL);
	namedWindow("ImageR Before Rectify", WINDOW_NORMAL);  imshow("ImageR Before Rectify", grayImageR);

	grayImageL.copyTo(rectifyImageL);
	grayImageR.copyTo(rectifyImageR);

	//--����remap֮�����������ͼ���Ѿ����沢���ж�׼----------------------------
	remap(grayImageL, rectifyImageL, mapLx, mapLy, INTER_LINEAR);
	remap(grayImageR, rectifyImageR, mapRx, mapRy, INTER_LINEAR);

	//--��У�������ʾ����---------------------------------------------------------
	Mat rgbRectifyImageL, rgbRectifyImageR;
	cvtColor(rectifyImageL, rgbRectifyImageL, CV_GRAY2BGR);  
	cvtColor(rectifyImageR, rgbRectifyImageR, CV_GRAY2BGR);

	namedWindow("ImageL After Rectify", WINDOW_NORMAL); imshow("ImageL After Rectify", rgbRectifyImageL);
	namedWindow("ImageR After Rectify", WINDOW_NORMAL); imshow("ImageR After Rectify", rgbRectifyImageR);

	//--��ʾ��ͬһ��ͼ��-----------------------------------------------------------
	Mat canvas;
	double sf;
	int w, h;
	sf = 600. / MAX(imageSize.width, imageSize.height);
	w = cvRound(imageSize.width * sf);
	h = cvRound(imageSize.height * sf);
	canvas.create(h, w * 2, CV_8UC3); //ע��ͨ��

	//--��ͼ�񻭵�������-----------------------------------------------------------
	Mat canvasPart = canvas(Rect(w * 0, 0, w, h));                                //�õ�������һ����
	resize(rgbRectifyImageL, canvasPart, canvasPart.size(), 0, 0, INTER_AREA);    //��ͼ�����ŵ���canvasPartһ����С
	Rect vroiL(cvRound(validROIL.x*sf), cvRound(validROIL.y*sf),                  //��ñ���ȡ������
		cvRound(validROIL.width*sf), cvRound(validROIL.height*sf));
	//rectangle(canvasPart, vroiL, Scalar(0, 0, 255), 3, 8);                     
	cout << "Painted ImageL" << endl;

	//--��ͼ�񻭵�������----------------------------------------------------------
	canvasPart = canvas(Rect(w, 0, w, h));                                        //��û�������һ����
	resize(rgbRectifyImageR, canvasPart, canvasPart.size(), 0, 0, INTER_LINEAR);
	Rect vroiR(cvRound(validROIR.x * sf), cvRound(validROIR.y*sf),
		cvRound(validROIR.width * sf), cvRound(validROIR.height * sf));
	//rectangle(canvasPart, vroiR, Scalar(0, 0, 255), 3, 8);
	cout << "Painted ImageR" << endl;

	//--���϶�Ӧ������-----------------------------------------------------------
	for (int i = 0; i < canvas.rows; i += 16)
		line(canvas, Point(0, i), Point(canvas.cols, i), Scalar(0, 0, 255), 1, 8);
	namedWindow("rectified", WINDOW_NORMAL); imshow("rectified", canvas);

	//--��ʾ���-----------------------------------------------------------------
	namedWindow("disparity", WINDOW_NORMAL);

	//--����SAD���� Trackbar-----------------------------------------------------
	//createTrackbar("BlockSize:\n", "disparity", &blockSize, 8, stereo_match);

	//--�����Ӳ�Ψһ�԰ٷֱȴ��� Trackbar----------------------------------------
	//createTrackbar("UniquenessRatio:\n", "disparity", &uniquenessRatio, 50, stereo_match);

	//--�����Ӳ�� Trackbar----------------------------------------------------
	//createTrackbar("NumDisparities:\n", "disparity", &numDisparities, 16, stereo_match);

	//--�����Ӧ����setMouseCallback(��������, ���ص�����, �����ص������Ĳ�����һ��ȡ0)---
	setMouseCallback("disparity", onMouse, 0);

	//--����Ҫ����������λ��5�������е���sgbm�㷨����һ�е���BM�㷨����ѡһ���о��������
	stereo_match_sgbm(0, 0);
	//stereo_match_bm(0, 0);

	waitKey(0);
	return 0;
}