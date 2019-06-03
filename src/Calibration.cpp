/***************************************************************************
	begin                : aout 15 2004
    copyright            : (C) 2004 by Simon CONSEIL
 ***************************************************************************/

#include "main.h"

using namespace std;

int Calibration(char** names, int nbImages, CvSize CBCornersSize, int squareSize, CCamera* camera)
{
    int i=0, j=0, t=0, x=0, y=0;
	int nbCornersFound;					// nb de points interieurs trouves
	int nbLines = CBCornersSize.height ;// nb de lignes ds la mire
	int nbCols	= CBCornersSize.width ;	// nb de colonnes ds la mire
	int nbPoints = nbLines * nbCols;
	int* pointnumbers = new int [nbImages] ;
	int currPoint;
	int useIntrinsicGuess = 0; 
	char savenames[128] ;

	IplImage* img = 0;
	IplImage* img0 = 0;
	IplImage* img1 = 0;
	IplImage* greyimg = 0;
	CvMemStorage* storage = 0;

	// Matrices et Vecteurs des parametres:
	//-------------------------------------
	CvMatr64d cameraMatrix	= new double [3*3];				// parametres intrinseques
	CvMatr64d rotMatrixes	= new double [3*3 * nbImages];	// matrices de rotation 3x3
	CvVect64d rotVect       = new double [3 * nbImages];	// vecteur rotation
	CvVect64d transVects	= new double [3 * nbImages];	// vecteur de translation
	CvVect64d distortion	= new double [4];				// distortion
	
	CvPoint2D64f* uv		= new CvPoint2D64f [nbPoints * nbImages]; 
	CvPoint3D64f* XYZ		= new CvPoint3D64f [nbPoints * nbImages]; 
    CvPoint2D32f* corners	= new CvPoint2D32f [nbPoints + 1];
	CvPoint onecorner;

	// initialisation des variables et tableaux:
	for( j = 0 ; j<nbImages ; j++ )
		pointnumbers[j] = nbPoints ;

	//������ʾ�����ڲ��Խǵ���ȡ����ȷ��
	//cvNamedWindow( "image", 1 );  
	CvFont dfont;
	float hscale = 0.3f;
	float vscale = 0.3f;
	float italicscale = 0.0f;
	int thickness =1;
	cvInitFont (&dfont, CV_FONT_VECTOR0, hscale, vscale, italicscale, thickness);

	// Coordonnees reelles 3D des points de la mire:
	//-------------------------------------------
	for( int currImage = 0; currImage < nbImages; currImage++ )
	{
		for( x = 0; x < nbLines; x++ )
		{
			for( y = 0; y < nbCols ; y++ )
			{
				XYZ[ currImage*nbPoints + x*nbCols + y ].x = double(squareSize * (nbLines - x-1) ); //(nbCols - i )) ;
				XYZ[ currImage*nbPoints + x*nbCols + y ].y = double(squareSize * y ); //(nbLines - j));
				XYZ[ currImage*nbPoints + x*nbCols + y ].z = 0; 
			}
		}
	}
   for( currPoint=0; currPoint < nbPoints; currPoint++ ) 
		{ 
			uv[  currPoint].x = XYZ[currPoint].x;
			uv[  currPoint].y = XYZ[currPoint].y;
		}
     SaveResultToFile("11.txt",uv,nbPoints);
	//for visual number effect to see the corners that were found
	char* numbers[] = 
	{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", 
	"11", "12", "13", "14", "15", "16", "17", "18", "19", "20", 
	"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", 
	"31", "32", "33", "34", "35", "36", "37", "38", "39", "40", 
	"41", "42", "43", "44", "45", "46", "47", "48", "49", "50", 
	"51", "52", "53", "54", "55", "56", "57", "58", "59", "60", 
	"61", "62", "63", "64", "65", "66", "67", "68", "69", "70", 
	"71", "72", "73", "74", "75", "76", "77", "78", "79", "80" };

	// BOUCLE
	for ( i = 0; i < nbImages; i++ )
	{
		nbCornersFound = nbPoints ;		// reset nbCornersFound
		img	  = cvLoadImage(names[i]);	// load image
		img1  = cvCloneImage(img);

		// convert color image img to gray image greyimg:
		greyimg = cvCreateImage(camera->Size, IPL_DEPTH_8U, 1); 
		cvCvtColor(img, greyimg, CV_RGB2GRAY);
		img0 = cvCloneImage(greyimg); 

		// Find Corners:
		cvFindChessBoardCornerGuesses(greyimg, img0, storage, CBCornersSize, corners, &nbCornersFound);
		
		if ( nbCornersFound != 0 )
		{
			std::cout << "I FOUND: " << nbCornersFound << endl;
			if (nbCornersFound != 48)
			{
				int i = 0;
			}
		}
		else
		{
			std::cout << " NO CORNERS .... " << endl;
			for( currPoint=0; currPoint < nbPoints; currPoint++ ) 
			{ 
				uv[ i*nbPoints + currPoint].x = 0;
				uv[ i*nbPoints + currPoint].y = 0;
			}
			continue;
		}

		// Find sub-corners:
		cvFindCornerSubPix(greyimg, corners, nbCornersFound, cvSize(5,5), cvSize(-1, -1), cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.1));

/*		for( t = 0; t < nbCornersFound; t++ ) 
		{printf("Point: %i = ", t+1);printf("x: %f ", corners[t].x);	printf("y: %f\n", corners[t].y);} */

		//draw a circle and put the corner number at each subcorner found
		for( t = 0; t < nbCornersFound; t++ ) 
		{
			onecorner.x = int(corners[t].x);
			onecorner.y = int(corners[t].y);
			cvCircle(img1, onecorner, 3, CV_RGB(255,0,0),1);
			cvPutText(img1,numbers[t],cvPoint(onecorner.x,onecorner.y+10),&dfont,CV_RGB(0,255,0)); 
		}

		// enregistrement des images :
		sprintf(savenames, "%s%s", names[i], "_calib.jpg");
		cvSaveImage( savenames, img1 );

		// recopie des coordonn�es :
		for( currPoint=0; currPoint < nbPoints; currPoint++ ) 
		{ 
			uv[ i*nbPoints + currPoint].x = double(corners[currPoint].x);
			uv[ i*nbPoints + currPoint].y = double(corners[currPoint].y);
		}
		//cvShowImage("image",img1);
		//cvWaitKey(0);
        //���һ�����Ժ���������⵽�������¼����
		char filenames[256];
        static int cout = 0;
		cout++;
	//	sprintf(filenames, "data%d%s", cout,".txt");
		sprintf(filenames, "%s%s", names[i], "_subpixel.txt");
      //  SaveResultToFile(filenames,uv+i*nbPoints,nbPoints);
		sprintf(filenames, "%s%s", names[i], "_subpixel_cal2.txt");
		ReadResultFromFile(filenames,uv+i*nbPoints,nbPoints);
    
		// release images
		cvReleaseImage( &img );
		cvReleaseImage( &img0 );
		cvReleaseImage( &img1 );
		cvReleaseImage( &greyimg );
	}
	//////////////////////////////////////////////////////////////////////
	free (corners);

	// Calibration:
	static int flag = 0;
// 	if (0 == flag)
// 	{
// 		cameraMatrix[0] = cameraMatrix[4] = 3746.76;
// 		cameraMatrix[2] = 976.19;
// 		cameraMatrix[5] = 805.90;
// 		cameraMatrix[8] = 1;
// 		cameraMatrix[1] = cameraMatrix[3] = cameraMatrix[6] = cameraMatrix[7] = 0;
// 		distortion[0] = distortion[1] = distortion[2] = distortion[3] = 0;
// 		flag = 1;
// 	}
// 	else
// 	{
// 		cameraMatrix[0] = cameraMatrix[4] = 3763.15;
// 		cameraMatrix[2] = 1088.76;
// 		cameraMatrix[5] = 806.27;
// 		cameraMatrix[8] = 1;
// 		cameraMatrix[1] = cameraMatrix[3] = cameraMatrix[6] = cameraMatrix[7] = 0;
// 		distortion[0] = distortion[1] = distortion[2] = distortion[3] = 0;
// 	}
	cvCalibrateCamera_64d( nbImages, 
    					  pointnumbers, 
    					  camera->Size, 
    					  uv, 
    					  XYZ, 
    					  distortion, 
    					  cameraMatrix, 
    					  transVects, 
    					  rotMatrixes, 
    					  useIntrinsicGuess);

	CvVect64d focal = new double[2];
	  focal[0] = cameraMatrix[0];
	  focal[1] = cameraMatrix[4];

	CvPoint2D64f principal;
	  principal.x = cameraMatrix[2];
	  principal.y = cameraMatrix[5];

	// Affichage des resultats :
	printf("Matrice intrinseque:\n%f %f %f\n%f %f %f\n%f %f %f\n\nDistortion:\n%f %f %f %f\n\n", 
	cameraMatrix[0],cameraMatrix[1],cameraMatrix[2], 
	cameraMatrix[3],cameraMatrix[4],cameraMatrix[5], 
	cameraMatrix[6],cameraMatrix[7],cameraMatrix[8],
	distortion[0],distortion[1],distortion[2],distortion[3]);

/*	printf("Matrice extrinseque image 0:\n%f %f %f\n%f %f %f\n%f %f %f\n\n Translation:\n%f %f %f\n", 
	rotMatrixes[0],rotMatrixes[1],rotMatrixes[2], 
	rotMatrixes[3],rotMatrixes[4],rotMatrixes[5], 
	rotMatrixes[6],rotMatrixes[7],rotMatrixes[8],
	transVects[0],transVects[1],transVects[2]);
*/
	// ���������:
	CvMat rmat = cvMat(3, 3, CV_64FC1, rotMatrixes) ;
	CvMat rvec = cvMat(3, 1, CV_64FC1, rotVect) ;	
	for (int iImageCout = 0; iImageCout < nbImages; iImageCout++ )
	{   //int iImageCout = 0;
		cvFindExtrinsicCameraParams_64d( nbPoints,
										camera->Size,
										uv+nbPoints*iImageCout,
										XYZ,
										focal,
										principal,
										distortion,
										rotVect,
										transVects );

		printf("\nVecteur rotation image %d:\n%f %f %f\n \n Translation:\n%f %f %f\n", iImageCout,
		rotVect[0],rotVect[1],rotVect[2], transVects[0],transVects[1],transVects[2]);

		// Matrices Rotation: conversion par Rodrigues
		//--------------------------------------------
	  
		cvRodrigues( &rmat, &rvec, 0, CV_RODRIGUES_V2M );
		CvScalar value;

		for( i=0; i<3; i++)
			for( j=0; j<3; j++)
			{
			value = cvGetAt( &rmat, i, j);
			rotMatrixes[i*3+j] = (double)value.val[0] ;
			} 

		printf("\nMatrice extrinseque :\n%f %f %f\n%f %f %f\n%f %f %f\n\n", 
		rotMatrixes[0],rotMatrixes[1],rotMatrixes[2], 
		rotMatrixes[3],rotMatrixes[4],rotMatrixes[5], 
		rotMatrixes[6],rotMatrixes[7],rotMatrixes[8] );

		
		//����ͬ��̬�µ���ת��ƽ�ƾ��������¼���������
		for ( j=0 ; j<9 ; j++)
			camera->pdAllMatRot[9*iImageCout+j] = rotMatrixes[j];
		for ( j=0 ; j<3 ; j++)
			camera->pdAllVecTrans[3*iImageCout+j] = transVects[j];
	}
    //��������ͷ����ת�����ƽ������,��ʼ��Ϊ��һ�鶨���ó��Ľ��
    
	for ( j=0 ; j<9 ; j++)
		camera->MatRot[j] = camera->pdAllMatRot[j];
	for ( j=0 ; j<3 ; j++)
		camera->VecTrans[j] = camera->pdAllVecTrans[j];

	// enregistrement des parametres:

	camera->Intrinsic[0] = focal[0];
	camera->Intrinsic[1] = focal[1];
	camera->Intrinsic[2] = principal.x;
	camera->Intrinsic[3] = principal.y;
	camera->Distorsion[0] = distortion[0];
	camera->Distorsion[1] = distortion[1];
	camera->Distorsion[2] = distortion[2];
	camera->Distorsion[3] = distortion[3];

	camera->SaveCameraParameters();
	camera->CalculateHomography();

	if(pointnumbers)delete[] pointnumbers;
	return 1;
}

//��������������λ�ù�ϵ
void  SolveCameraPose(CCamera* pCameraLeft,CCamera* pCameraRight,const int nbImages)
{
	double* pdR = new double[nbImages*9];//�洢���������֮�����ת����
	double* pdT = new double[nbImages*9];//�洢���������֮���ƽ������

	//���������֮��Ĺ�ϵ���㹫ʽ����,�Ե�����������λ��Ϊ��������ԭ��
	//R = R2*inverse(R1)
	//T = T2 - R2inverse(R1)T1
	double R1[9],R2[9],T1[3],T2[3],Temp[9],R[9],T[3],Temp2[3];
	CvMat MR1, MR2, MT1,MT2,MTemp,MTemp2,MR,MT ;
    cvInitMatHeader( &MR1, 3, 3, CV_64FC1, R1);
    cvInitMatHeader( &MR2, 3, 3, CV_64FC1, R2);
    cvInitMatHeader( &MT1, 3, 1, CV_64FC1, T1);
	cvInitMatHeader( &MT2, 3, 1, CV_64FC1, T2);
	cvInitMatHeader( &MTemp, 3, 3, CV_64FC1, Temp);
	cvInitMatHeader( &MR, 3, 3, CV_64FC1, R);
	cvInitMatHeader( &MT, 3, 1, CV_64FC1, T);
	cvInitMatHeader( &MTemp2, 3, 1, CV_64FC1, Temp2);
	int i,j;
	for( i = 0 ; i < nbImages ; i++ )
	{
		memcpy(R1,pCameraLeft->pdAllMatRot+i*9,sizeof(double)*9);
		memcpy(R2,pCameraRight->pdAllMatRot+i*9,sizeof(double)*9);
		memcpy(T1,pCameraLeft->pdAllVecTrans+i*3,sizeof(double)*3);
		memcpy(T2,pCameraRight->pdAllVecTrans+i*3,sizeof(double)*3);
		cvInvert(&MR1,&MTemp);//����
		cvGEMM(&MR2,&MTemp,1,NULL,0,&MR);//R = R2*inverse(R1)
        cvGEMM(&MR,&MT1,1,NULL,0,&MTemp2);//T = T2 - R2*inverse(R1)*T1
		cvSub(&MT2,&MTemp2,&MT);
		//�����������浽pdR,pdT��
		memcpy(pdR+i*9,R,sizeof(double)*9);
		memcpy(pdT+i*3,T,sizeof(double)*3);
	}
	//�����������R,T��ֵȡƽ��
    memset(R,0,sizeof(double)*9);
	for( i = 0 ; i < nbImages ; i++ )
	{
		for(j = 0 ; j < 9 ; j++)
			R[j] += pdR[i*9+j];
	}
    memset(T,0,sizeof(double)*3);
	for( i = 0 ; i < nbImages ; i++ )
	{
		for(j = 0 ; j < 3 ; j++)
			T[j] += pdT[i*3+j];
	}
    for( i = 0 ; i < 9 ; i++ )
	{
		R[i] /= nbImages;
	}
	for( i = 0 ; i < 3 ; i++ )
	{
		T[i] /= nbImages;
	}
	//�������е���̬�µ�˫����ͷ�Ĺ�ϵ,���ǿ����������������������������ϵΪ[I|0],�ұ����������������ϵΪ[R|T]
    for( i = 0 ; i < 9 ; i++ )
	{
		pCameraRight->MatRot[i] = R[i] ;
	}
	for( i = 0 ; i < 3 ; i++ )
	{
		pCameraRight->VecTrans[i] = T[i] ;
	}

	memset(pCameraLeft->MatRot,0,sizeof(double)*9);
    pCameraLeft->MatRot[0] = 1;
	pCameraLeft->MatRot[4] = 1;
	pCameraLeft->MatRot[8] = 1;
	memset(pCameraLeft->VecTrans,0,sizeof(double)*3);

	//
	pCameraLeft->CalculateHomography();
	pCameraRight->CalculateHomography();
}

//ͼ���е�������������ͷ����ı���
/*
ptSrc
ԭʼͼ���Ѿ����ε�ͼ���еĵ㡣 
ptDst
���ͼ���Ѿ�У����ͼ���ж�Ӧ������ꡣ 
intrinsic_matrix 
����ڲ������󣬸�ʽΪ �� 
distortion_coeffs 
�ĸ�����ϵ����ɵ���������СΪ4x1����1x4����ʽΪ[k1,k2,p1,p2]�� 
*/

void  UndistortPoint(const CvPoint2D64f* ptSrc,  CvPoint2D64f* ptDst, const CvMat* intrinsic_matrix, const CvMat* distortion_coeffs )
{
	CvPoint2D64f ptDistort,ptUndistort;//�����ı��κ�ͽ�������ʵ����

    //������Ľ��������
	const double fx = cvmGet(intrinsic_matrix,0,0);
    const double fy = cvmGet(intrinsic_matrix,1,1);
	const double cx = cvmGet(intrinsic_matrix,0,2);
	const double cy = cvmGet(intrinsic_matrix,1,2);
    //������ı��β���
    const double k1 = cvmGet(distortion_coeffs,0,0);
    const double k2 = cvmGet(distortion_coeffs,0,1);
	const double p1 = cvmGet(distortion_coeffs,0,2);
	const double p2 = cvmGet(distortion_coeffs,0,3);

	//����С�׳���ʽ�õ�
	//u = fx*x" + cx
    //v = fy*y" + cy

	ptDistort.x = (ptSrc->x - cx)/fx;
    ptDistort.y = (ptSrc->y - cy)/fy;
    ptUndistort = ptDistort;
     
	//һ��������newton��������ȡ�����Է�����Ľ�
	// x" = x'*(1 + k1r2 + k2r4) + 2*p1x'*y' + p2(r2+2*x'2)
    //y" = y'*(1 + k1r2 + k2r4) + p1(r2+2*y'2) + 2*p2*x'*y'
    //���㷽���ο����ġ������Է�������������Newton���Ƚϡ�
	double  deltaX,deltaY; //��������
	int iCount = 0; //ѭ������ͳ��
	double fun1,fun2;//f1(x),f2(x)
	double f1x,f1y,f2x,f2y;//f1,f2��ƫ΢��
	double x = ptDistort.x;
	double y =  ptDistort.y;
	double x1 = x  ;
	double y1 = y  ;
	double r2;
	r2 = x1*x1+y1*y1;
	fun1 = x1*(1+k1*r2+k2*r2*r2) + 2*p1*x1*y1 + p2*(r2+2*x1*x1) - x;
	fun2 = y1*(1+k1*r2+k2*r2*r2) + p1*(r2+2*y1*y1) + 2*p2*x1*y1 - y;
	
    f1x = 1+k1*r2+k2*r2*r2 + 2*p1*y1+4*p2*x1;
    f1y = 2*p1*x1;
	f2x = 2*p2*y1;
	f2y = 1+k1*r2+k2*r2*r2 + 4*p1*y1 +2*p2*x1;
	
	CvPoint2D64f ptDelta;
	do
	{
		iCount ++ ;
		r2 = x1*x1+y1*y1;
		
       //�����õ�������
		deltaX = (f1y*fun2-f2y*fun1)/(f1x*f2y-f1y*f2x);
		deltaY = (f1x*fun2-f2x*fun1)/(f1y*f2x-f1x*f2y);

		x1 = x1 + deltaX;
		y1 = y1 + deltaY;
		ptDelta.x = deltaX*fx;
		ptDelta.y = deltaY*fy;
        
		f1x = 1+k1*r2+k2*r2*r2 + 2*p1*y1+4*p2*x1;
        f1y = 2*p1*x1;
	    f2x = 2*p2*y1;
	    f2y = 1+k1*r2+k2*r2*r2 + 4*p1*y1 +2*p2*x1;
		fun1 = x1*(1+k1*r2+k2*r2*r2) + 2*p1*x1*y1 + p2*(r2+2*x1*x1) - x;
		fun2 = y1*(1+k1*r2+k2*r2*r2) + p1*(r2+2*y1*y1) + 2*p2*x1*y1 - y;
		if(fun1>=0.0000005||fun2>=0.0000005)  //���Ŀ�꺯��ֵ����ֱ������ѭ��
			continue;   
	}
	while((abs(ptDelta.x)>=0.05||abs(ptDelta.y)>=0.05)&&iCount<100);  //�ж�����ѭ��
	 
    //������ϣ��ó��˷����Է�����Ľ�
	//�ٴ����ù�ʽ����õ����ս��
	//u = fx*x' + cx
    //v = fy*y' + cy
	ptUndistort.x = x1;
	ptUndistort.y = y1;
	ptDst->x = fx*ptUndistort.x + cx;
    ptDst->y = fx*ptUndistort.y + cy;
}
//��������ͼ��Ӧ���ͼ�����꣬�������Ƿ����������ά����
//ptuvL:����ͼͼ������
//pCameraLeft:�������
//ptuvR:����ͼͼ������
//pCameraRight:�������
//ptXYZ:��ά��������
//nbPoints:�ؽ������Ŀ

void  Reconstruct3DPoint(const CvPoint2D64f * ptuvL, CCamera * pCameraLeft,const CvPoint2D64f * ptuvR, CCamera * pCameraRight, CvPoint3D64f * ptXYZ, const int nbPoints)
{
	//�ؽ���ÿ�������ά����
	int i,t;
	double M[4][3];
	double b[4];
	double MTM[3][3],MTb[3],X[3];
		
	CvMat MatM,Matb,MatX,MatTemp,MatTemp2;
	for( t = 0 ;t < nbPoints ;t++)
	{
		//������sx=HX��ȥs��õ�һ������ΪMX = b 
		//����ͼ�õ��͵�ǰ����,����ͼ�õ�M�ĺ�����
		for( i = 0 ; i < 3 ;i++)
		{
			M[0][i] = ptuvL[t].x*pCameraLeft->Homography[2][i] - pCameraLeft->Homography[0][i];
			M[1][i] = ptuvL[t].y*pCameraLeft->Homography[2][i] - pCameraLeft->Homography[1][i];
			M[2][i] = ptuvR[t].x*pCameraRight->Homography[2][i] - pCameraRight->Homography[0][i];
			M[3][i] = ptuvR[t].y*pCameraRight->Homography[2][i] - pCameraRight->Homography[1][i];
		}
		b[0] = pCameraLeft->Homography[0][3] - ptuvL[t].x*pCameraLeft->Homography[2][3];
		b[1] = pCameraLeft->Homography[1][3] - ptuvL[t].y*pCameraLeft->Homography[2][3];
		b[2] = pCameraRight->Homography[0][3] - ptuvR[t].x*pCameraRight->Homography[2][3];
		b[3] = pCameraRight->Homography[1][3] - ptuvR[t].y*pCameraRight->Homography[2][3];
		//�ⷽ��MX = b
     
		cvInitMatHeader( &MatTemp, 4, 3, CV_64FC1, M );
		cvInitMatHeader( &MatM, 3, 3, CV_64FC1, MTM );
        cvGEMM(&MatTemp,&MatTemp,1,NULL,0,&MatM,CV_GEMM_A_T);
	
		cvInitMatHeader( &MatTemp2, 4, 1, CV_64FC1, b );
        cvInitMatHeader(&Matb, 3, 1, CV_64FC1, MTb );
		cvInitMatHeader( &MatTemp, 4, 3, CV_64FC1, M );
        cvGEMM(&MatTemp,&MatTemp2,1,NULL,0,&Matb,CV_GEMM_A_T);
		cvInitMatHeader( &MatX, 3, 1, CV_64FC1, X );
		
		
		cvSolve(&MatM,&Matb,&MatX,CV_SVD);
		ptXYZ[t].x = cvmGet(&MatX,0,0);
		ptXYZ[t].y = cvmGet(&MatX,1,0);
		ptXYZ[t].z = cvmGet(&MatX,2,0);
		
	}
  /*  //�ͷž���
	cvReleaseMat(&MatM);
	cvReleaseMat(&Matb);
	cvReleaseMat(&MatX);
	cvReleaseMat(&MatTemp);
	cvReleaseMat(&MatTemp2);
	*/
	
}
//�ҵ�ͼ���еĽǵ㣬���������
//strFile��ͼ���ļ���
//ptuv��ָ���Žǵ����ݵ��ڴ��ָ��
//CBCornersSize�����̸�Ĵ�С
//pCamera��ָ������ͼ�������������ô�����������Խǵ��λ�ý���У������ΪNULL��У��
void  FindCorners(char* strFile, CvPoint2D64f * ptuv, const CvSize CBCornersSize, CCamera* pCamera = NULL)
{
	int nbPoints = CBCornersSize.height*CBCornersSize.width;//���ͼ����һ���еĽǵ����Ŀ
	//�����ݽ��м��
	if(NULL == strFile)
	{
		printf("no file input");
		return ;
	}
	if(NULL == ptuv)
	{
       ptuv = new CvPoint2D64f[nbPoints];
	}
	//���ó�ʼ������
    int i=0, j=0, t=0, x=0, y=0;
	int nbCornersFound;					// nb de points interieurs trouves
	CvSize ImageSize;
	
	int currPoint;
	int useIntrinsicGuess = 0; 
	char savenames[128] ;

	IplImage* img = 0;
	IplImage* img0 = 0;
	IplImage* img1 = 0;
	IplImage* greyimg = 0;
	CvMemStorage* storage = 0;
   

	nbCornersFound = nbPoints ;		// reset nbCornersFound
	img	  = cvLoadImage(strFile);	// load image
	img1  = cvCloneImage(img);
    ImageSize = cvGetSize(img);
    CvPoint onecorner;
	CvPoint2D32f* corners	= new CvPoint2D32f [nbPoints];

	//������ʾ�����ڲ��Խǵ���ȡ����ȷ��
	//cvNamedWindow( "image", 1 );  
	CvFont dfont;
	float hscale = 0.3f;
	float vscale = 0.3f;
	float italicscale = 0.0f;
	int thickness =1;
	cvInitFont (&dfont, CV_FONT_VECTOR0, hscale, vscale, italicscale, thickness);
	//for visual number effect to see the corners that were found
	char* numbers[] = 
	{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", 
	"11", "12", "13", "14", "15", "16", "17", "18", "19", "20", 
	"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", 
	"31", "32", "33", "34", "35", "36", "37", "38", "39", "40", 
	"41", "42", "43", "44", "45", "46", "47", "48", "49", "50", 
	"51", "52", "53", "54", "55", "56", "57", "58", "59", "60", 
	"61", "62", "63", "64", "65", "66", "67", "68", "69", "70", 
	"71", "72", "73", "74", "75", "76", "77", "78", "79", "80" };

	// convert color image img to gray image greyimg:
	greyimg = cvCreateImage(ImageSize, IPL_DEPTH_8U, 1); 
	cvCvtColor(img, greyimg, CV_RGB2GRAY);
	img0 = cvCloneImage(greyimg); 
    //ͼ�����У������
	if(pCamera !=NULL)
	{
		double K[3][3] = {pCamera->Intrinsic[0], 0, pCamera->Intrinsic[2], 
					0, pCamera->Intrinsic[1],pCamera->Intrinsic[3], 
					0, 0, 1};
		CvMat Mintrinsic_matrix, Mdistortion_coeffs;
		cvInitMatHeader( &Mintrinsic_matrix, 3, 3, CV_64FC1, K);
		cvInitMatHeader( &Mdistortion_coeffs, 1, 4, CV_64FC1, pCamera->Distorsion);
		cvUndistort2(greyimg,img0,&Mintrinsic_matrix,&Mdistortion_coeffs);
		cvReleaseImage( &greyimg);
		greyimg = cvCloneImage(img0); 
	}
		// Find Corners:
	cvFindChessBoardCornerGuesses(greyimg, img0, storage, CBCornersSize, corners, &nbCornersFound);
		
	if ( nbCornersFound != 0 );
			//std::cout << "I FOUND: " << nbCornersFound << endl;
	else
	{
		std::cout << " NO CORNERS .... " << endl;
		for( currPoint=0; currPoint < nbPoints; currPoint++ ) 
		{ 
			ptuv[currPoint].x = 0;
			ptuv[currPoint].y = 0;
		}
		return;
	}

	// Find sub-corners:
	cvFindCornerSubPix(greyimg, corners, nbCornersFound, cvSize(5,5), cvSize(-1, -1), cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.1));

	
	for( t = 0; t < nbCornersFound; t++ ) 
	{
		onecorner.x = int(ptuv[t].x);
		onecorner.y = int(ptuv[t].y);
		cvCircle(img1, onecorner, 3, CV_RGB(255,0,0),1);
		cvPutText(img1,numbers[t],cvPoint(onecorner.x,onecorner.y+10),&dfont,CV_RGB(0,255,0)); 
	}
    for( currPoint=0; currPoint < nbPoints; currPoint++ ) 
	{ 
			ptuv[ i*nbPoints + currPoint].x = double(corners[currPoint].x);
			ptuv[ i*nbPoints + currPoint].y = double(corners[currPoint].y);
	} 
	//���ı��ж�ȡ����
	char filenames[256];
	sprintf(filenames, "%s%s", strFile, "_subpixel_cal2.txt");
	ReadResultFromFile(filenames,ptuv,nbPoints);

		// enregistrement des images :
	sprintf(savenames, "%s%s", strFile, "_calib111.jpg");
	cvSaveImage( savenames, img1 );
	
	//�����ݴ����ָ����TXT�ļ���ȥ
    
	// release images
	cvReleaseImage( &img );
	cvReleaseImage( &img0 );
	cvReleaseImage( &img1 );
	cvReleaseImage( &greyimg );
	if(NULL != corners) delete []corners;
}
void  SaveResultToFile(char* strFile,CvPoint2D64f * ptuv,const int nbCornersFound)
{
    FILE *cps;
	cps = fopen(strFile, "w");

	if (cps == NULL)
	{
		printf("Error: Unable to open the calib text file\n");
		return ;
	}
    for(int i = 0 ; i < nbCornersFound ;i +=8)
	{
		fprintf(cps, "%f  %f  ", ptuv[i].x,ptuv[i].y);
		fprintf(cps, "%f  %f  ", ptuv[i+1].x,ptuv[i+1].y);
		fprintf(cps, "%f  %f  ", ptuv[i+2].x,ptuv[i+2].y);
		fprintf(cps, "%f  %f  ", ptuv[i+3].x,ptuv[i+3].y);
		fprintf(cps, "%f  %f  ", ptuv[i+4].x,ptuv[i+4].y);
		fprintf(cps, "%f  %f  ", ptuv[i+5].x,ptuv[i+5].y);
		fprintf(cps, "%f  %f  ", ptuv[i+6].x,ptuv[i+6].y);
		fprintf(cps, "%f  %f  \n", ptuv[i+7].x,ptuv[i+7].y);
	}
	fclose(cps);
}
void  ReadResultFromFile(char* strFile,CvPoint2D64f * ptuv,const int nbCornersFound)
{
	FILE *cps;
	cps = fopen(strFile, "r+");

	if (cps == NULL)
	{
		printf("Error: Unable to open the calib text file\n");
		return ;
	}
	for(int i = 0 ; i < nbCornersFound ;i +=8)
	{
		fscanf(cps, "%lf  %lf  ", &ptuv[i].x,&ptuv[i].y);
		fscanf(cps, "%lf  %lf  ", &ptuv[i+1].x,&ptuv[i+1].y);
		fscanf(cps, "%lf  %lf  ", &ptuv[i+2].x,&ptuv[i+2].y);
		fscanf(cps, "%lf  %lf  ", &ptuv[i+3].x,&ptuv[i+3].y);
		fscanf(cps, "%lf  %lf  ", &ptuv[i+4].x,&ptuv[i+4].y);
		fscanf(cps, "%lf  %lf  ", &ptuv[i+5].x,&ptuv[i+5].y);
		fscanf(cps, "%lf  %lf  ", &ptuv[i+6].x,&ptuv[i+6].y);
		fscanf(cps, "%lf  %lf  \n", &ptuv[i+7].x,&ptuv[i+7].y);
	}
	fclose(cps);
}