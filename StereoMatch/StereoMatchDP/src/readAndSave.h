#ifndef HEADER_READANDSAVE
#define HEADER_READANDSAVE

#if 0
#include <math.h>
#include <iomanip>
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

const int bmpWidth=384;//ͼ���
const int bmpHeight=288;//ͼ���
const int deta=20;//������Χ
const int dataThre=30;//��������ֵ
const int smoothThre=30;//ƽ������ֵ
const double smoothDataRatio=3;//ƽ������������ı�ֵ

using namespace std;

bool readBmp(char *,unsigned char*);
bool saveBmp(char *, unsigned char *, int, int,int, RGBQUAD *);
bool getSize(char*);
#endif

#endif