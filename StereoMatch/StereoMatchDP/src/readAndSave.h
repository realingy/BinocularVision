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

const int bmpWidth=384;//图像宽
const int bmpHeight=288;//图像高
const int deta=20;//搜索范围
const int dataThre=30;//数据项阈值
const int smoothThre=30;//平滑项阈值
const double smoothDataRatio=3;//平滑项与数据项的比值

using namespace std;

bool readBmp(char *,unsigned char*);
bool saveBmp(char *, unsigned char *, int, int,int, RGBQUAD *);
bool getSize(char*);
#endif

#endif