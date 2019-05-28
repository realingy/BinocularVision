/***************************************************************************
    
						         main.cpp  -  description
                             -------------------
    
    begin                : aout 01 2004
    copyright            : (C) 2004 by Simon CONSEIL
    email                : bouldeglace@yahoo.fr

 ***************************************************************************/
#include "main.h"

using namespace std;

void Reconstruct3DPoint(CCamera* Cam_gauche,CCamera* Cam_droite,CvSize CBCornersSize,char *pcLeftImage,char *pcRightImage);
double Dis(CvPoint3D64d pA,CvPoint3D64d pB)
{
	double Temp = (pA.x-pB.x)*(pA.x-pB.x)+(pA.y-pB.y)*(pA.y-pB.y)+(pA.z-pB.z)*(pA.z-pB.z);
	double d = sqrt(Temp);

	static int i = 0 ;
	static  double  ave = 0;
	
	//FILE *cps = fopen("E:\\re.txt","a");
	//CString str;
	if (i == 0 )
	{
		//str = "\n\n\n\n";
		//fseek(cps,0,SEEK_END);
		//fprintf(cps,str);
		//file.Write(str,str.GetLength());
	}
	if (0 == i%4)
	{
		//str = "\n";
		//file.SeekToEnd();
		//file.Write(str,str.GetLength());
	}
	//str.Format("%f  ",d);
	//file.SeekToEnd();
	//file.Write(str,str.GetLength());
	ave += abs(d-30)/30;
	if (i == 95 )
	{
		ave /= 96;
		ave *= 100;
	//	str.Format("\n误差为 %f  ",ave);
	//	file.SeekToEnd();
	//	file.Write(str,str.GetLength());
	}
	//file.Close();
	i++;

	return sqrt(Temp);
}


///////////////////////
int main(void)
///////////////////////
{
	CvSize Image_size = {2048,1536};


	/*******************  CALIBRATION     *****************************/

// 	char* names_L[] = { "./calib/Stereo_Left_2.JPG", "./calib/Stereo_Left_2.JPG", "./calib/Stereo_Left_3.JPG",
//                         "./calib/Stereo_Left_4.JPG",  "./calib/Stereo_Left_6.JPG",
// 						"./calib/Stereo_Left_7.JPG", "./calib/Stereo_Left_8.JPG", "./calib/Stereo_Left_9.JPG",
// 						"./calib/Stereo_Left_10.JPG", "./calib/Stereo_Left_11.JPG", "./calib/Stereo_Left_12.JPG",
// 						"./calib/Stereo_Left_13.JPG", "./calib/Stereo_Left_14.JPG", "./calib/Stereo_Left_15.JPG",
// 						"./calib/Stereo_Left_16.JPG", "./calib/Stereo_Left_17.JPG", "./calib/Stereo_Left_18.JPG",
// 						"./calib/Stereo_Left_19.JPG", "./calib/Stereo_Left_20.JPG", "./calib/Stereo_Left_21.JPG",
// 						"./calib/Stereo_Left_22.JPG", "./calib/Stereo_Left_23.JPG", "./calib/Stereo_Left_24.JPG",
// 						"./calib/Stereo_Left_25.JPG", "./calib/Stereo_Left_26.JPG", "./calib/Stereo_Left_27.JPG"};
// 						
// 
// 
// 
//     char* names_R[] = { "./calib/Stereo_Right_2.JPG", "./calib/Stereo_Right_2.JPG", "./calib/Stereo_Right_3.JPG",
//                         "./calib/Stereo_Right_4.JPG", "./calib/Stereo_Right_6.JPG",
// 						"./calib/Stereo_Right_7.JPG", "./calib/Stereo_Right_8.JPG", "./calib/Stereo_Right_9.JPG",
// 						"./calib/Stereo_Right_10.JPG", "./calib/Stereo_Right_11.JPG", "./calib/Stereo_Right_12.JPG",
// 						"./calib/Stereo_Right_13.JPG", "./calib/Stereo_Right_14.JPG", "./calib/Stereo_Right_15.JPG",
// 						"./calib/Stereo_Right_16.JPG", "./calib/Stereo_Right_17.JPG", "./calib/Stereo_Right_18.JPG",
// 						"./calib/Stereo_Right_19.JPG", "./calib/Stereo_Right_20.JPG", "./calib/Stereo_Right_21.JPG",
// 						"./calib/Stereo_Right_22.JPG", "./calib/Stereo_Right_23.JPG", "./calib/Stereo_Right_24.JPG",
// 						"./calib/Stereo_Right_25.JPG", "./calib/Stereo_Right_26.JPG", "./calib/Stereo_Right_27.JPG"};

	//一下是生成文件名字数
#define  File_Num 36
    const int nbImages = File_Num;					// nb d'images a traiter
     char* names_L[File_Num];
	 char* names_R[File_Num];
	 char Left_File_Names[5*9][256];
     char Right_File_Names[5*9][256];
	 //初始化指针数组
	 for ( int i = 0 ; i < File_Num ; i++)
	 {
		 names_L[i] = Left_File_Names[i+6];
		 names_R[i] = Right_File_Names[i+6];
	 }
	 //生成文件名
	 int iRotX[5] = {0,10,20,30,40};
	 int iRotY[9] = {-40,-30,-20,-10,0,10,20,30,40};

    

	 int j;
	 int k = 0;

	 FILE* f = fopen("E:\\实验图片\\Left\\left.txt","r" );
	 for( i = 0 ;i < 5 ; i++)
	 {
		 for ( j = 0 ;j < 9 ; j++)
		 {
			 fscanf(f,"%s",Left_File_Names[i*9+j]);

		 }
	 }
	 fclose(f);



	 for( i = 0 ;i < 5 ; i++)
	 {
		 for ( j = 0 ;j < 9 ; j++)
		 {
			 sprintf(Left_File_Names[i*9+j],"G:/12.24/Calibration/Left/Image_RotX_%d_RotY_%d.bmp",iRotX[i],iRotY[j]);
			 sprintf(Right_File_Names[i*9+j],"G:/12.24/Calibration/Right/Image_RotX_%d_RotY_%d.bmp",iRotX[i],iRotY[j]);
			
		 }
	 }



  
	CvSize CBCornersSize = {8,6};		// nb de points interieurs de la mire

/*	CvSize Image_size = {640,480};
	char* names_L[] = { "./stereo_example/left01.jpg", "./stereo_example/left03.jpg", "./stereo_example/left03.jpg",
                        "./stereo_example/left04.jpg", "./stereo_example/left09.jpg", "./stereo_example/left10.jpg"};

    char* names_R[] = { "./stereo_example/right01.jpg", "./stereo_example/right03.jpg", "./stereo_example/right03.jpg",
                        "./stereo_example/right04.jpg", "./stereo_example/right09.jpg", "./stereo_example/right10.jpg"};
    CvSize CBCornersSize = {9,6};*/
	int squareSize = 30 ;				// taille des carres de la mire
   
	

	
	/* TESTS AVEC IMAGES DE CALIB ( 640 , 480 )*/
	CCamera Cam_gauche(Image_size, "C://LeftCamerParameter.txt", 1,nbImages);
	Calibration(names_L, nbImages, CBCornersSize, squareSize, &Cam_gauche);


	CCamera Cam_droite(Image_size, "C://RightCamerParameter.txt", 2,nbImages);

	cout << "Loading cameras ... " << endl;

	Calibration(names_R, nbImages, CBCornersSize, squareSize, &Cam_droite);

	
	cvDestroyAllWindows();
	SolveCameraPose(&Cam_gauche,&Cam_droite,nbImages);
	Cam_droite.SaveCameraParameters();
	Cam_gauche.SaveCameraParameters();

	
	for ( i = 0 ; i < 45 ; i++)
	{
		Reconstruct3DPoint(&Cam_gauche,&Cam_droite, CBCornersSize,Left_File_Names[i],Right_File_Names[i]);

	}
	system("pause");
	return 1;
}
void Reconstruct3DPoint(CCamera* Cam_gauche,CCamera* Cam_droite,CvSize CBCornersSize,char *pcLeftImage,char *pcRightImage)
{
	int i=0, j=0, t=0, x=0, y=0;
			// nb de points interieurs trouves
	int nbLines = CBCornersSize.height ;// nb de lignes ds la mire
	int nbCols	= CBCornersSize.width ;	// nb de colonnes ds la mire
	int nbPoints = nbLines * nbCols;


	

	// Matrices et Vecteurs des parametres:
	//-------------------------------------
	
	
	CvPoint2D64d* uvL		= new CvPoint2D64d [nbPoints]; 
	CvPoint2D64d* uvR		= new CvPoint2D64d [nbPoints]; 
	CvPoint3D64d* XYZ		= new CvPoint3D64d [nbPoints]; 

 
   
	char* names[] = {"./calib/Stereo_Left_2.bmp","./calib/Stereo_Right_2.bmp"};
    //char* names[] = {"./stereo_example/left10.jpg","./stereo_example/right10.jpg"};

	//分别获得左右视图的角点坐标
    FindCorners(pcLeftImage,uvL,CBCornersSize,NULL);//Cam_gauche);
    FindCorners(pcRightImage,uvR,CBCornersSize,NULL);//Cam_droite);


    double KL[3][3] = { Cam_gauche->Intrinsic[0], 0, Cam_gauche->Intrinsic[2], 
                   0, Cam_gauche->Intrinsic[1], Cam_gauche->Intrinsic[3], 
                   0, 0, 1};
    double KR[3][3] = { Cam_droite->Intrinsic[0], 0, Cam_droite->Intrinsic[2], 
                   0, Cam_droite->Intrinsic[1], Cam_droite->Intrinsic[3], 
                   0, 0, 1};
	CvMat MLintrinsic_matrix, MLdistortion_coeffs,MRintrinsic_matrix, MRdistortion_coeffs;
    cvInitMatHeader( &MLintrinsic_matrix, 3, 3, CV_64FC1, KL);
	cvInitMatHeader( &MRintrinsic_matrix, 3, 3, CV_64FC1, KR);
    cvInitMatHeader( &MLdistortion_coeffs, 1, 4, CV_64FC1, Cam_gauche->Distorsion);
	cvInitMatHeader( &MRdistortion_coeffs, 1, 4, CV_64FC1, Cam_droite->Distorsion);

	//分别对获取得到的点进行校正
    for(i = 0; i <  nbPoints ; i++)
	{
		UndistortPoint(uvL+i,uvL+i,&MLintrinsic_matrix,&MLdistortion_coeffs);
		UndistortPoint(uvR+i,uvR+i,&MRintrinsic_matrix,&MRdistortion_coeffs);
	}
	Reconstruct3DPoint(uvL,Cam_gauche,uvR,Cam_droite,XYZ,nbPoints);
	//比较计算出来的数据的准确性，将计算出来的结果显示出来
	/*
	for( i = 0 ; i < CBCornersSize.height ; i++)
	{
		for( j = 0; j < CBCornersSize.width ; j++)
		{
			std::cout<<i<<j<<" ("<<XYZ[CBCornersSize.width*i+j].x<<" "<<XYZ[CBCornersSize.width*i+j].y<<" "<<XYZ[CBCornersSize.width*i+j].z<<")   ";
		}
		std::cout<<endl;
	}

	for( i = 1 ; i < CBCornersSize.height -1 ; i++)
	{
		for( j = 1; j < CBCornersSize.width - 1 ; j++)
		{
			std::cout<<i<<j<<" ("
			<<Dis(XYZ[CBCornersSize.width*i+j],XYZ[CBCornersSize.width*(i-1)+j])<<" "
			<<Dis(XYZ[CBCornersSize.width*i+j],XYZ[CBCornersSize.width*i+j-1])<<" "
			<<Dis(XYZ[CBCornersSize.width*i+j],XYZ[CBCornersSize.width*i+j+1])<<" "
			<<Dis(XYZ[CBCornersSize.width*i+j],XYZ[CBCornersSize.width*(i+1)+j])<<")  ";
		}
		std::cout<<endl;
	}
	*/
	static int iCount = 0;
	iCount++;
	char str[256];
	sprintf(str,"G:/12.24/Calibration/%d.txt",iCount);
	if(iCount == 6)
	{
		return;
	}
	FILE *cps;
	
	cps = fopen(str, "a+" );
	if (cps == NULL)
	{
		printf("Error: Unable to open the calib text file\n");
		return ;
	}
    fseek(cps,0,SEEK_END);

//  	std::cout<<pcLeftImage<<endl;
	static double dErrAll = 0,dMaxAll = 0;
	double dMax = 0;
	double dAve1 = 0,dAve2 = 0,d;
	for (i = 0 ; i < CBCornersSize.height  ; i++)
	{
	    d = Dis(XYZ[CBCornersSize.width*i],XYZ[CBCornersSize.width*i+7]);
	//	std::cout<<"Real distance: 210   Reconstruct dis  " <<d<<" Err: " << 210-d<<" Err rate "<<abs(210-d)*1000/210<<endl;
		dMax = abs(210-d)>dMax?abs(210-d):dMax;
		dAve1 += abs(210-d);

		
	}
	for (i = 0 ; i < CBCornersSize.width  ; i++)
	{
		d = Dis(XYZ[i],XYZ[CBCornersSize.width*5+i]);
	//	std::cout<<"Real distance: 150   Reconstruct dis  " <<d<<" Err: " << 150-d<<" Err rate "<<abs(150-d)*1000/150<<endl;
		dMax = abs(150-d)>dMax?abs(150-d):dMax;
		dAve2 += abs(150-d);
	
	}

	for( i = 0 ; i < CBCornersSize.height ; i++)
	{
		for( j = 0 ; j < CBCornersSize.width ; j++)
		{
			fprintf(cps,"%f  %f  %f  ",XYZ[CBCornersSize.width*i+j].x,XYZ[CBCornersSize.width*i+j].y,XYZ[CBCornersSize.width*i+j].z);
			d = sqrt(i*i+j*j*1.0)*30;
			d = d - Dis(XYZ[CBCornersSize.width*i+j],XYZ[0]);
			if (d<-3)
			{
				std::cout<<iCount<<"  " << i <<"  "<<j<<endl;
			}
			if (0==i&&0==j)
			{
				continue;
			}
			//fprintf(cps,"%f  ",d);
			
		}
	}
	fclose(cps);
	dAve1 /= 6;
	dAve2 /= 8;
//	std::cout<<"Max Err "<<dMax<<" dAve1: " <<dAve1<<"  "<<dAve1*1000/210
//				<< " dAve2: " <<dAve2<<"  "<<dAve2*1000/150<<endl;
	d = Dis(XYZ[0],XYZ[47]);
//	std::cout<<"Accurate Dis "<<30*sqrt(74.0)<<"   Cal Result "<<d<<"    Err " <<abs(30*sqrt(74.0)-d)<<"  "<<abs(30*sqrt(74.0)-d)*1000/(30*sqrt(74.0))
//		<<endl;
  
	
	dErrAll += (dAve1 + dAve2)/2;
	dMaxAll = dMaxAll>dMax?dMaxAll:dMax;
	
//	std::cout<<endl;
	if (iCount == 45)
	{
		dErrAll /= 44;
		std::cout<< " Total Err is："<< dErrAll << endl;
		std::cout<< " Total Max is："<< dMaxAll << endl;
	}
//	std::cout<<endl;
//	std::cout<<endl;
   	delete []uvL;
	delete []uvR;
	delete []XYZ;
}