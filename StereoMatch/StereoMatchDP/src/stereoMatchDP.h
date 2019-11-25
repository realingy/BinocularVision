#ifndef __STEREO_MATCH_DP__
#define __STEREO_MATCH_DP__

#include <math.h>
#include <iomanip>
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

class StereoMatchDP
{
public:
	StereoMatchDP();
	~StereoMatchDP();
	void process(char*, char*);
	void match();

private:
	bool readBmp(char *, unsigned char *);
	bool saveBmp(char *, unsigned char *, int, int, int, RGBQUAD *);
	bool getSize(char* );

private:

	unsigned char * pBmpBufLeft;	//������ͼ�����ݵ�ָ��
	unsigned char * pBmpBufRight;	//������ͼ�����ݵ�ָ��
	unsigned char * pResultBuf;		//���ͼ�����ݵ�ָ��
	RGBQUAD * pColorTable;			//��ɫ��ָ��
	int biBitCount;					//ͼ�����ͣ�ÿ����λ��

};


#endif //__STEREO_MATCH_DP__