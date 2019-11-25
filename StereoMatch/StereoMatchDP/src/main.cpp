// #include "readAndSave.h"
// #include "pixelToMat.h"

#include <math.h>
#include <iomanip>
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

const int bmpWidth = 384;	//图像宽
const int bmpHeight = 288;	//图像高
const int deta = 20;//搜索范围
const int dataThre = 30;//数据项阈值
const int smoothThre = 30;//平滑项阈值
const double smoothDataRatio = 3;//平滑项与数据项的比值

bool readBmp(char *, unsigned char*);
bool saveBmp(char *, unsigned char *, int, int, int, RGBQUAD *);
bool getSize(char*);
void xiang_su_du_qu(char*,char*);
void myProcess();

unsigned char *pBmpBufLeft;	//读入左图像数据的指针
unsigned char *pBmpBufRight;//读入右图像数据的指针
unsigned char *pResultBuf;	//结果图像数据的指针
RGBQUAD *pColorTable;		//颜色表指针
int biBitCount;				//图像类型，每像素位数

int main()
{
	char* filePathLeft="tsuR.bmp";
	char* filePathRight="tsuL.bmp";
	xiang_su_du_qu(filePathLeft,filePathRight);
	return 0;
}

//以下为像素的读取函数
void xiang_su_du_qu(char* path1,char* path2)
{
	//读入指定BMP文件进内存
	getSize(path1);
	int lineByte=(bmpWidth*biBitCount/8+3)/4*4;

	pBmpBufLeft=new unsigned char[lineByte*bmpHeight];
	pBmpBufRight=new unsigned char[lineByte*bmpHeight];
	readBmp(path1,pBmpBufLeft);
	readBmp(path2,pBmpBufRight);

	myProcess();

	char writePath[]="result.BMP";
	saveBmp(writePath, pResultBuf, bmpWidth, bmpHeight, biBitCount, pColorTable);

	//清除缓冲区，pBmpBuf和pColorTable是全局变量，在文件读入时申请的空间
	delete [] pBmpBufLeft;
	delete [] pBmpBufRight;
	delete [] pResultBuf;

	if (biBitCount == 8) {
		delete [] pColorTable;
	}
}

bool getSize(char *bmpName)
{
	FILE *fp = fopen(bmpName, "rb");//二进制读方式打开指定的图像文件

	if (fp == 0) return 0;

	//跳过位图文件头结构 BITMAPFILEHEADER

	fseek(fp, sizeof(BITMAPFILEHEADER), 0);

	//定义位图信息头结构变量，读取位图信息头进内存，存放在变量head中
	BITMAPINFOHEADER head;

	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);//获取图像宽，高，每位像素占位数等信息
	//bmpWidth=head.biWidth;

	//bmpHeight=head.biHeight;

	biBitCount = head.biBitCount;//定义变量，计算图像每行像素所占的字节数(必须是4的倍数)

	int lineByte = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;
	return 1;
}

bool readBmp(char *bmpName, unsigned char* pBmpBuf)
{
	FILE *fp = fopen(bmpName, "rb");//二进制读方式打开指定的图像文件

	if (fp == 0)
		return 0;

	//跳过位图文件头结构 BITMAPFILEHEADER
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);

	//定义位图信息头结构变量，读取位图信息头进内存，存放在变量head中
	BITMAPINFOHEADER head;

	//获取图像宽，高，每位像素占位数等信息
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);
	
	//bmpWidth=head.biWidth;
	//bmpHeight=head.biHeight;

	biBitCount = head.biBitCount; //定义变量，计算图像每行像素所占的字节数(必须是4的倍数)

	int lineByte = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;

	//灰度图像有颜色表，且颜色表表项为256
	if (biBitCount == 8)
	{
		//申请颜色表所需要的空间，读颜色表进内存
		pColorTable = new RGBQUAD[256];
		fread(pColorTable, sizeof(RGBQUAD), 256, fp);
	}

	//申请位图数据所需要的空间，读位图数据进内存
	//pBmpBuf=new unsigned char[lineByte*bmpHeight];
	fread(pBmpBuf, 1, lineByte*bmpHeight, fp);

	//输出图像的信息
	cout << "width=" << bmpWidth << " height=" << bmpHeight << " biBitCount=" << biBitCount << endl;

	fclose(fp);//关闭文件

	return 1;//读取文件成功

}

//给定一个图像位图数据、宽、高、颜色表指针及每像素所占的位数等信息,将其写到指定文件中
bool saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height, int biBitCount, RGBQUAD *pColorTable)
{
	//如果位图数据指针为0，则没有数据传入，函数返回
	if (!imgBuf)
		return 0;

	//颜色表大小，以字节为单位，灰度图像颜色表为1024字节，彩色图像颜色表大小为0
	int colorTablesize = 0;

	if (biBitCount == 8)
		colorTablesize = 1024;

	//待存储图像数据每行字节数为4的倍数
	int lineByte = (width * biBitCount / 8 + 3) / 4 * 4;

	//以二进制写的方式打开文件
	FILE *fp = fopen(bmpName, "wb");

	if (fp == 0)
		return 0;

	//申请位图文件头结构变量，填写文件头信息
	BITMAPFILEHEADER fileHead;

	fileHead.bfType = 0x4D42;//bmp类型

	//bfSize是图像文件4个组成部分之和
	fileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte * height;

	fileHead.bfReserved1 = 0;
	fileHead.bfReserved2 = 0;
	//bfOffBits是图像文件前3个部分所需空间之和
	fileHead.bfOffBits = 54 + colorTablesize;

	//写文件头进文件
	fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);

	//申请位图信息头结构变量，填写信息头信息
	BITMAPINFOHEADER head;

	head.biBitCount = biBitCount;
	head.biClrImportant = 0;
	head.biClrUsed = 0;
	head.biCompression = 0;
	head.biHeight = height;
	head.biPlanes = 1;
	head.biSize = 40;
	head.biSizeImage = lineByte * height;
	head.biWidth = width;
	head.biXPelsPerMeter = 0;
	head.biYPelsPerMeter = 0;

	//写位图信息头进内存
	fwrite(&head, sizeof(BITMAPINFOHEADER), 1, fp);

	//如果灰度图像，有颜色表，写入文件 
	if (biBitCount == 8) {
		fwrite(pColorTable, sizeof(RGBQUAD), 256, fp);
	}

	//写位图数据进文件
	fwrite(imgBuf, height*lineByte, 1, fp);

	//关闭文件
	fclose(fp);

	return 1;
}

void myProcess()
{
	int lineByte = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;
	pResultBuf = new unsigned char[lineByte*bmpHeight];
	if (biBitCount == 8) //对于灰度图像
	{
	}
	else if (biBitCount == 24)//彩色图像
	{
		static int Matleft[bmpHeight][bmpWidth];//左图
		static int MatRight[bmpHeight][bmpWidth];//右图
		static int D[bmpHeight][bmpWidth];//记录视差值

		static int dif[bmpWidth][deta + 1];//数据项
		static int dif2[bmpHeight][deta + 1];//数据项

		static int Emat[deta + 1][bmpWidth];//记录能量值
		static int Emat2[deta + 1][bmpHeight];//记录能量值

		static int Dmat[deta + 1][bmpWidth];//记录中间视差
		static int Dmat2[deta + 1][bmpHeight];//记录中间视差


		//注意C读取图像时坐标原点为左下角
		double gray;
		for (int i = 0; i < bmpHeight; ++i)
		{
			for (int j = 0; j < bmpWidth; ++j)
			{
				gray = 0.5 + *(pBmpBufLeft + i * lineByte + j * 3 + 0)*0.114 + *(pBmpBufLeft + i * lineByte + j * 3 + 1)*0.587 + *(pBmpBufLeft + i * lineByte + j * 3 + 2)*0.299;
				Matleft[bmpHeight - i - 1][j] = (int)gray;
				gray = 0.5 + *(pBmpBufRight + i * lineByte + j * 3 + 0)*0.114 + *(pBmpBufRight + i * lineByte + j * 3 + 1)*0.587 + *(pBmpBufRight + i * lineByte + j * 3 + 2)*0.299;
				MatRight[bmpHeight - i - 1][j] = (int)gray;
			}
		}
		DWORD start_time = GetTickCount();
		for (int i = 0; i < bmpHeight; ++i)//循环每一行
		{
			//初始化Emat
			for (int ind_r = 0; ind_r < deta + 1; ++ind_r)
				for (int ind_c = 0; ind_c < bmpWidth; ++ind_c)
					Emat[ind_r][ind_c] = (ind_c == 0 ? 0 : 32767);

			//初始化Dmat
			for (int ind_r = 0; ind_r < deta + 1; ++ind_r)
				for (int ind_c = 0; ind_c < bmpWidth; ++ind_c)
					Dmat[ind_r][ind_c] = 0;

			//初始化dif

			for (int ind_c = 0; ind_c < bmpWidth; ++ind_c)
				for (int ind_r = 0; ind_r < deta + 1; ++ind_r)
					dif[ind_c][ind_r] = abs(Matleft[i][ind_c] - (((ind_c + ind_r) <= bmpWidth) ? MatRight[i][ind_c + ind_r] : 0));


			for (int j = 1; j < bmpWidth; ++j)
			{
				for (int ind_match = 0; ind_match <= deta; ++ind_match)
				{
					for (int ind_pre = (ind_match - 3 >= 0 ? (ind_match - 3) : 0); ind_pre <= (ind_match + 3 <= deta ? ind_match + 3 : deta); ++ind_pre)
					{
						int Edata = (dif[j][ind_match] < dataThre ? dif[j][ind_match] : dataThre);
						int Esmooth = abs(ind_match - ind_pre) < smoothThre ? abs(ind_match - ind_pre) : smoothThre;
						int E = Edata + smoothDataRatio * Esmooth + Emat[ind_pre][j - 1];
						if (E < Emat[ind_match][j])
						{
							Emat[ind_match][j] = E;
							Dmat[ind_match][j] = ind_pre;
						}
					}
				}
			}
			//[Emin,n]=min(Emat(:,width));
			int minInd = 0;
			int max = Emat[0][bmpWidth - 1];
			for (int i = 0; i <= deta; ++i)
			{

				if (Emat[i][bmpWidth - 1] < max)
				{
					minInd = i;
					max = Emat[i][bmpWidth - 1];
				}
			}
			//取得视差
			for (int c = bmpWidth - 1; c >= 0; --c)
			{
				int d = Dmat[minInd][c];
				minInd = d;
				D[i][c] = d;//*8增加对比度而已
			}
		}

		//继续求取列方向匹配，并对之前行方向的匹配结果中的点的代价进行修改

		for (int j = 0; j < bmpWidth; ++j)//循环每一列
		{
			//初始化Emat
			for (int ind_r = 0; ind_r < deta + 1; ++ind_r)
				for (int ind_c = 0; ind_c < bmpHeight; ++ind_c)
					Emat2[ind_r][ind_c] = (ind_c == 0 ? 0 : 32767);

			//初始化Dmat
			for (int ind_r = 0; ind_r < deta + 1; ++ind_r)
				for (int ind_c = 0; ind_c < bmpHeight; ++ind_c)
					Dmat2[ind_r][ind_c] = 0;

			//初始化dif

			for (int ind_c = 0; ind_c < bmpHeight; ++ind_c)
				for (int ind_r = 0; ind_r < deta + 1; ++ind_r)
					dif2[ind_c][ind_r] = (D[ind_c][j] == ind_r ? -2 : abs(Matleft[ind_c][j] - (((j + ind_r) <= bmpWidth) ? MatRight[ind_c][j + ind_r] : 0)));


			for (int i = 1; i < bmpHeight; ++i)
			{
				for (int ind_match = 0; ind_match <= deta; ++ind_match)
				{
					for (int ind_pre = (ind_match - 3 >= 0 ? (ind_match - 3) : 0); ind_pre <= (ind_match + 3 <= deta ? ind_match + 3 : deta); ++ind_pre)
					{
						int Edata = (dif2[i][ind_match] < dataThre ? dif2[i][ind_match] : dataThre);
						int Esmooth = abs(ind_match - ind_pre) < smoothThre ? abs(ind_match - ind_pre) : smoothThre;
						int E = Edata + smoothDataRatio * Esmooth + Emat2[ind_pre][i - 1];
						if (E < Emat2[ind_match][i])
						{
							Emat2[ind_match][i] = E;
							Dmat2[ind_match][i] = ind_pre;
						}
					}
				}
			}
			//[Emin,n]=min(Emat(:,width));
			int minInd2 = 0;
			int max2 = Emat2[0][bmpHeight - 1];
			for (int i = 0; i <= deta; ++i)
			{

				if (Emat2[i][bmpHeight - 1] < max2)
				{
					minInd2 = i;
					max2 = Emat2[i][bmpHeight - 1];
				}
			}
			//取得视差
			for (int c = bmpHeight - 1; c >= 0; --c)
			{
				int d = Dmat2[minInd2][c];
				minInd2 = d;
				D[c][j] = d * 8;//*8增加对比度而已
			}
		}
		//按图片存储格式重新排序
		for (int i = 0; i < bmpHeight; ++i)
		{
			for (int j = 0; j < bmpWidth; ++j)
			{
				*(pResultBuf + (bmpHeight - i - 1)*lineByte + j * 3 + 0) = D[i][j];
				*(pResultBuf + (bmpHeight - i - 1)*lineByte + j * 3 + 1) = D[i][j];
				*(pResultBuf + (bmpHeight - i - 1)*lineByte + j * 3 + 2) = D[i][j];

			}
		}
		DWORD end_time = GetTickCount();
		std::cout << end_time - start_time << "ms";
	}
}

