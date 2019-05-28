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
	double Homography[3][4];//��Ӧ����

	IplImage* Courante;
	IplImage* Masque;

	// CONSTRUCTEUR:
	CCamera(void);
	CCamera(CvSize imgsize, const char* name, int ID,int); // ouverture d'une s�quence d'images au format PGM
	void SetParameter(CvSize imgsize, const char* name, int ID);//�������������
	~CCamera(void);

	// Lecture et �criture des param�tres ds un fichier texte, 1 valeur  par ligne
	// 4 params intrinseques, 4 params distorsion, 9 params rotation, 3 params translation:
	int LoadCameraParameters();
	int SaveCameraParameters();
	// ָ�򶨱�岻ͬ��̬�µ���ת����
	double* pdAllMatRot;
	// ��������ͼ�����Ŀ������������̬����Ŀ
	int iImage_Num;
	// ָ�򶨱�岻ͬ��̬�µ�ƽ�ƾ���
	double* pdAllVecTrans;
	// ������������Ѿ��궨���Ĳ����������Ӧ
	void CalculateHomography(void);
};

#endif
