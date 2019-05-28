/***************************************************************************
    
						Camera.cpp  -  description
                             -------------------
    
	begin                : sept 20 2004
    copyright            : (C) 2004 by Simon CONSEIL
    email                : bouldeglace@yahoo.fr

 ***************************************************************************/

#include "main.h"
#include ".\camera.h"

//------------------------------------------
CCamera::CCamera(void)
//------------------------------------------
// Constructeur par d閒aut
: pdAllMatRot(NULL)
, iImage_Num(0)
, pdAllVecTrans(NULL)
{
	Size.width	= 0;
	Size.height = 0;
	Courante	= 0;
	Masque		= 0;
	Image_counter = 0;
	cam_ID		= 0;
}

//------------------------------------------------------
CCamera::CCamera(CvSize imgsize, const char* name, int ID,int iNum)
//------------------------------------------------------
// Constructeur Principal
{
	Size.width	= imgsize.width;
	Size.height = imgsize.height;
	Courante	= cvCreateImage( Size, IPL_DEPTH_8U, 1);
	Masque		= cvCreateImage( Size, IPL_DEPTH_8U, 1);
	Image_counter = 0;
	cam_ID = ID;
	this->iImage_Num = iNum;
    pdAllMatRot = new double[iNum*9];
	this->pdAllVecTrans = new double[iNum*3];
	sprintf(filename,name);
}

//------------------------------------------------------
void CCamera::SetParameter(CvSize imgsize, const char* name, int ID)
//------------------------------------------------------
// Constructeur Principal
{
	Size.width	= imgsize.width;
	Size.height = imgsize.height;
	Courante	= cvCreateImage( Size, IPL_DEPTH_8U, 1);
	Masque		= cvCreateImage( Size, IPL_DEPTH_8U, 1);
	Image_counter = 0;
	cam_ID = ID;
	sprintf(filename,name);
}

//------------------------------------------
CCamera::~CCamera(void)
//------------------------------------------
{
	if(pdAllMatRot != NULL) delete[]pdAllMatRot;
	if(pdAllVecTrans != NULL) delete[]pdAllVecTrans;
	cvReleaseImage(&Courante);
	cvReleaseImage(&Masque);
}

//---------------------------------------------------------------------------------------------------------
int CCamera::LoadCameraParameters()
//---------------------------------------------------------------------------------------------------------
{
	char file[256];
	sprintf(file, "%s_calib.txt", filename);
    FILE *cps;
	cps = fopen(file, "r");

	if (cps == NULL)
	{
		printf("Error: Unable to open the calib text file\n");
		return 0;
	}

	fscanf(cps, "%lf\n%lf\n%lf\n%lf\n", &Intrinsic[0], &Intrinsic[1], &Intrinsic[2], &Intrinsic[3] );
	fscanf(cps, "%lf\n%lf\n%lf\n%lf\n", &Distorsion[0], &Distorsion[1], &Distorsion[2], &Distorsion[3]);

	// extrinsic params with the first image:
	int i = 0; 
	fscanf(cps, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", 
		&MatRot[i]  , &MatRot[i+1], &MatRot[i+2],
		&MatRot[i+3], &MatRot[i+4], &MatRot[i+5],
		&MatRot[i+6], &MatRot[i+7], &MatRot[i+8]);
	fscanf(cps, "%lf\n%lf\n%lf\n", &VecTrans[i], &VecTrans[i+1], &VecTrans[i+2]);

	fclose(cps);
	return 1;
}

//--------------------------------------------------------------------------------------------
int CCamera::SaveCameraParameters()
//--------------------------------------------------------------------------------------------
{
	char file[256];
	sprintf(file, "%s_calib.txt", filename);
    FILE *cps;
	cps = fopen(file, "w");

	if (cps == NULL)
	{
		printf("Error: Unable to open the calib text file\n");
		return 0;
	}

	fprintf(cps, "%lf\n%lf\n%lf\n%lf\n", Intrinsic[0],Intrinsic[1], Intrinsic[2], Intrinsic[3] );
	fprintf(cps, "%lf\n%lf\n%lf\n%lf\n", Distorsion[0],Distorsion[1], Distorsion[2],Distorsion[3]);

	// extrinsic params with the first image:
	int i = 0;
	fprintf(cps, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", 
		MatRot[i], MatRot[i+1], MatRot[i+2],
		MatRot[i+3], MatRot[i+4], MatRot[i+5],
		MatRot[i+6], MatRot[i+7], MatRot[i+8]);
	fprintf(cps, "%lf\n%lf\n%lf\n", VecTrans[i], VecTrans[i+1], VecTrans[i+2]);

	fclose(cps);
	return 1;
}

// 利用摄像机中已经标定出的参数计算出单应
//计算公式为：H = K * [R | T ]
void CCamera::CalculateHomography(void)
{
	
	int i,j,t;
	//
	double K[3][3] = {this->Intrinsic[0],0,this->Intrinsic[2],
			0, this->Intrinsic[1],this->Intrinsic[3],
			  0,0,1};
	//建立对应的矩阵
	double RT[3][4];
	//memset(*RT ,0, sizeof(double)*12);
    for( i = 0; i < 3;i++)
		for( j = 0; j< 3 ;j++)
			RT[i][j] = this->MatRot[i*3+j];
	for( i = 0 ; i< 3 ;i++)
		RT[i][3] =VecTrans[i];
		/*for ( t = 0 ;t < 3; t++)
		{
		    RT[i][3] -=RT[i][t]*Cam_gauche->VecTrans[t];
		}*/

    
	memset(*Homography,0,sizeof(double)*12);
	for( i = 0 ; i < 3 ;i++)
		for( j = 0 ;j < 4 ; j++)
			for( t = 0 ; t < 3 ;t++)
			{
				Homography[i][j] += K[i][t]*RT[t][j];
			}
	
} 
