#ifndef CCamera_h
#define CCamera_h

#include "main.h"

//-----------------------------------------------------------------------------------------
class CCamera
//-----------------------------------------------------------------------------------------
{
public:
	char filename[128];
	long Image_counter;
	int cam_ID; // 1 = gauche , 2 = droite
	CvSize Size;

	double Intrinsic[4];
	double Distorsion[4];
	double MatRot[9];
	double VecTrans[3] ;
	double Homography[3][4];//单应矩阵

	IplImage* Courante;
	IplImage* Masque;

	// CONSTRUCTEUR:
	CCamera(void);
	CCamera(CvSize imgsize, const char* name, int ID,int); // ouverture d'une s閝uence d'images au format PGM
	void SetParameter(CvSize imgsize, const char* name, int ID);//设置摄像机参数
	~CCamera(void);

	// Lecture et 閏riture des param鑤res ds un fichier texte, 1 valeur  par ligne
	// 4 params intrinseques, 4 params distorsion, 9 params rotation, 3 params translation:
	int LoadCameraParameters();
	int SaveCameraParameters();
	// 指向定标板不同姿态下的旋转矩阵
	double* pdAllMatRot;
	// 定标所用图像的数目，即定标板的姿态的数目
	int iImage_Num;
	// 指向定标板不同姿态下的平移矩阵
	double* pdAllVecTrans;
	// 利用摄像机中已经标定出的参数计算出单应
	void CalculateHomography(void);
};

#endif
