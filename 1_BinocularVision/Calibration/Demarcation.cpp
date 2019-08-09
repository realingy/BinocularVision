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

	//�������ı궨���ͼƬ·���Լ��궨����ı����ļ���
	string infilename = "left.txt";
	string outfilename = "left_calib.txt";

	//�궨����ͼƬ���ļ�·����ÿһ�б���һ���궨ͼƬ��·��
	ifstream fin(infilename);
	ofstream fout(outfilename);

	//��ȡÿһ��ͼ�񣬴�����ȡ�ǵ㣬Ȼ��Խǵ���������ؾ�ϸ������ȡÿ���ǵ�����������ϵ�е�����
	cout << "��ʼ��ȡ�ǵ�.....\n"<< endl;
	int imagecount = 0;//ͼ�������
	Size imagesize;//ͼ��ĳߴ�
	Size boardSize = Size(11, 8);//�궨����ÿ��ÿ�е��ڽǵ���Ŀ
	vector<Point2f>imagePointsBuf;//����ÿ��ͼ���ϼ�⵽�Ľǵ�����
	vector<vector<Point2f>>imagePointsSeq;//��������ͼ���ϼ�⵽�Ľǵ������
	char filename[100];

	if (fin.is_open())
	{
		while (!fin.eof())//�Ƿ��ȡ���
		{
			fin.getline(filename, sizeof(filename) / sizeof(char));//һ�ζ�ȡһ��
			filenames.push_back(filename);//�����ļ���
			Mat imageInput = imread(filename);
			if (imagecount == 0)//�����һ��ͼƬʱ����ȡͼ��Ŀ�͸���Ϣ��
			{
				imagesize.width = imageInput.cols;
				imagesize.height = imageInput.rows;
				cout << "imagewidth=" << imagesize.width;
				cout << "\nimageheight=" << imagesize.height;
			}
			imagecount++;
			/*
			��ʼ��ȡÿһ��ͼ���ϵĽǵ�
			*/
			if (findChessboardCorners(imageInput, boardSize, imagePointsBuf) == 0)
			{
				cout << "�Ҳ����ǵ�";
				exit(1);
			}
			else
			{
				Mat viewGray;
				cvtColor(imageInput, viewGray, COLOR_BGR2GRAY);//��ͼƬת��Ϊ�Ҷ�ͼ��ת��Ϊ8λͼ���Ч�ʸ�
				find4QuadCornerSubpix(viewGray, imagePointsBuf, Size(3, 3));//�����ؾ�ϸ�����Դ���ȡ�Ľǵ���о�ȷ�� ,size()Ϊ�ǵ��������ڵĳߴ�
				imagePointsSeq.push_back(imagePointsBuf);//���������ػ���Ľǵ�����
				drawChessboardCorners(viewGray, boardSize, imagePointsBuf, true);//��ͼ������ʾ�ǵ��λ��
				imshow("Camera Calibration ", viewGray);
				imwrite("test.jpg", viewGray);
				waitKey(500);
			}
		}

		cout << "\nimagecount=" << imagecount; //�궨ͼƬ������

		//����ÿ��ͼƬ�ϵĽǵ�����
		int cornerNum = boardSize.width*boardSize.height;
		//�������б궨ͼƬ�Ľǵ�����Ŀ
		int total = imagePointsSeq.size()*cornerNum;
		cout << "���б궨ͼƬ�ܵĽǵ���Ŀ=��\n" << total;
		for (int i = 0; i < total; i++)
		{
			int num = i / cornerNum;//cornerNum��ÿ��ͼƬ�ϵĽǵ����
			int p = i % cornerNum;
			if ((p==0))//Ϊ�˽��е��ԣ���ȡͼƬ������
			{
				cout << "\n��" << num + 1 << "��ͼƬ������-->:\n";
			}
			
			/*���ÿ��ͼƬ�ϸ����ǵ��λ����Ϣ*/
			cout << p + 1 << ":(" << imagePointsSeq[num][p].x;
			cout << imagePointsSeq[num][p].y << ")\n";
			if ((p+1)%3==0)
			{
				cout << endl;
			}
		}
		cout << "�ǵ���ȡ���";
		/*��ʼ�궨<������궨ʱ����������ϵԭ������λ�ڱ궨�����Ͻǣ���һ����������Ͻǣ�*/

		cout << "��ʼ�궨";
		//������ά��Ϣ��������������������ϵ������
		Size squareSize = Size(5,5);//������ÿ�����̸��ʵ�ʴ�С
		vector<int>pointCounts;//ÿ��ͼ��Ľǵ�����
		vector<vector<Point3f>>objectPoints;//���б궨ͼ��Ľǵ�����������ϵ�е���ʵ����
		Mat cameraMatrix = Mat(3, 3, CV_64F, Scalar::all(0));//������ڲξ���[fx,fy,r,u0,0,v0,0,0,1]
		Mat distCoeffs = Mat(1, 5, CV_64F, Scalar::all(0));//5�����������k1,k2,p1,p2,k3
		vector<Mat>rvecsMat;//��ת����
		vector<Mat>tvecsMat;//ƽ������

		/*��ʼ���궨���Ͻǵ����ά����*/
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
					realPoint.z= 0;//�������ϱ궨ʱ���������������ϵԭ�����ڵ�ƽ����ͼ��ƽ���غϣ����Ա궨���z���������Ϊ0
					temPointSet.push_back(realPoint);
				}
			}
			objectPoints.push_back(temPointSet);//�������нǵ����������
		}
		/*��¼ÿ��ͼ���еĽǵ��������ٶ�ÿ��ͼ�񶼿��Կ��������ı궨��*/
		for ( i = 0; i < imagecount; i++)
		{
			pointCounts.push_back(boardSize.width*boardSize.height);
		}
		/*��ʼ�궨*/
		int flag = 0; //| cv::CALIB_FIX_K3
		calibrateCamera(objectPoints, imagePointsSeq, imagesize, 
			cameraMatrix, distCoeffs, rvecsMat, tvecsMat,flag);
		cout << "�궨���\n";
		/*�Ա궨�����������*/
		cout << "��ʼ���۱궨���\n"; 
		double totalErr=0;//����ͼ���ƽ������ܺ�
		double err = 0;//ÿ��ͼ���ƽ�����
		vector<Point2f>imagePoints2;//������ݱ궨������¼���ĵ���ͶӰ�������

		cout << "ÿ��ͼ��ı궨���\n";
		for ( i = 0; i < imagecount; i++)
		{
			vector<Point3f>tempPointset = objectPoints[i];//�õ�ÿ�ű궨��ͼ���ԭ��������
			/*ͨ���õ��������������������Կռ����ά�������ͶӰ���㣬�õ��µ�ͶӰ��ImagePoints2(����������ϵ�µĵ�����꣩*/
			projectPoints(tempPointset, rvecsMat[i], tvecsMat[i], cameraMatrix, distCoeffs, imagePoints2);
			//�����µ�ͶӰ��;ɵ�ͶӰ��֮������
			vector<Point2f>tempImagePoint = imagePointsSeq[i];//�õ�ÿ�ű궨���ϵĽǵ�ԭ��������
			Mat tempImagePointMat = Mat(1, tempImagePoint.size(), CV_32FC2);//���궨���ϵĽǵ���ֱ�Ų�
			Mat imagePoints2Mat = Mat(1, imagePoints2.size(), CV_32FC2);//���궨���ϵĽǵ���ֱ�Ų�
			for (int j = 0; j < tempImagePoint.size(); j++)//�ֱ�õ��궨��ԭ���ĸ����ǵ����������
			{
				//�õ�������������ݱ궨�����ͶӰ�õ�����������
				imagePoints2Mat.at<Vec2f>(0, j) = Vec2f(imagePoints2[j].x, imagePoints2[j].y);
				//�õ��궨��ԭ���ĸ����������������
				tempImagePointMat.at<Vec2f>(0, j) = Vec2f(tempImagePoint[j].x, tempImagePoint[j].y);
			}
			/*����ÿ��ͼ���ϱ궨�����*/
			err = norm(imagePoints2Mat, tempImagePointMat, NORM_L2);
			totalErr += err /= pointCounts[i];//��ȡ���б궨ͼ���ƽ�����
			cout << "��" << i + 1 << "��ͼ���ƽ����" << err << "����\n";
			fout << "��" << i + 1 << "��ͼ���ƽ����" << err << "����\n";
		}
		/*ÿ��ͼ���ƽ�������*/
		cout << "����ƽ�����" << totalErr / imagecount << "����\n";
		fout << "����ƽ�����" << totalErr / imagecount << "����\n";
		cout << "�������\n";
		cout << "����궨�����������\n";
		Mat rotationMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
		fout << "����ڲ�������\n"  ;
		fout << cameraMatrix;
		fout << "����ϵ��\n";
		fout << distCoeffs;
		for (int i = 0; i < imagecount; i++)
		{
			fout << "��" << i + 1 << "��ͼ�����ת����:\n";
			fout << rvecsMat[i];
			Rodrigues(rvecsMat[i], rotationMatrix);
			fout << "��" << i + 1 << "��ͼ�����ת����:\n";
			fout << rotationMatrix;
			fout << "��" << i + 1 << "��ͼ���ƽ������:\n";
			fout << tvecsMat[i];
		}
		cout << "�������\n";
		/*��ʾ�궨���*/
		Mat mapx = Mat(imagesize, CV_32FC1);
		Mat mapy = Mat(imagesize, CV_32FC1);
		Mat R = Mat::eye(3, 3, CV_32F);
		cout << "��ʾ����ͼ��\n";
		for (int i = 0; i !=imagecount; i++)
		{
			cout << "Frame#" << i + 1;
			/*����͹�Ի���У����ӳ�����mapx,mapy(�������������������У����Ҫʹ��˫��*/
			initUndistortRectifyMap(cameraMatrix, distCoeffs, R, cameraMatrix, imagesize, CV_32FC1, mapx, mapy);
			Mat imageSource = imread(filenames[i]);
			Mat newimage = imageSource.clone();
			/*����һ�ֲ���Ҫת������ķ�ʽ
			udistort(imageSource,newiamge,cameraMatrix,distCoeffs);
			*/
			remap(imageSource, newimage, mapx, mapy, INTER_LINEAR);
			imshow("ԭʼͼ��",imageSource );
			imshow("�������ͼ��", newimage);
			waitKey();
		}
     }
}

