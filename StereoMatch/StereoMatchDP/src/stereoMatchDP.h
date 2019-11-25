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

	unsigned char * pBmpBufLeft;	//读入左图像数据的指针
	unsigned char * pBmpBufRight;	//读入右图像数据的指针
	unsigned char * pResultBuf;		//结果图像数据的指针
	RGBQUAD * pColorTable;			//颜色表指针
	int biBitCount;					//图像类型，每像素位数

};


#endif //__STEREO_MATCH_DP__