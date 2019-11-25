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

const int bmpWidth = 384;	//ͼ���
const int bmpHeight = 288;	//ͼ���
const int deta = 20;//������Χ
const int dataThre = 30;//��������ֵ
const int smoothThre = 30;//ƽ������ֵ
const double smoothDataRatio = 3;//ƽ������������ı�ֵ

bool readBmp(char *, unsigned char*);
bool saveBmp(char *, unsigned char *, int, int, int, RGBQUAD *);
bool getSize(char*);
void xiang_su_du_qu(char*,char*);
void myProcess();

unsigned char *pBmpBufLeft;	//������ͼ�����ݵ�ָ��
unsigned char *pBmpBufRight;//������ͼ�����ݵ�ָ��
unsigned char *pResultBuf;	//���ͼ�����ݵ�ָ��
RGBQUAD *pColorTable;		//��ɫ��ָ��
int biBitCount;				//ͼ�����ͣ�ÿ����λ��

int main()
{
	char* filePathLeft="tsuR.bmp";
	char* filePathRight="tsuL.bmp";
	xiang_su_du_qu(filePathLeft,filePathRight);
	return 0;
}

//����Ϊ���صĶ�ȡ����
void xiang_su_du_qu(char* path1,char* path2)
{
	//����ָ��BMP�ļ����ڴ�
	getSize(path1);
	int lineByte=(bmpWidth*biBitCount/8+3)/4*4;

	pBmpBufLeft=new unsigned char[lineByte*bmpHeight];
	pBmpBufRight=new unsigned char[lineByte*bmpHeight];
	readBmp(path1,pBmpBufLeft);
	readBmp(path2,pBmpBufRight);

	myProcess();

	char writePath[]="result.BMP";
	saveBmp(writePath, pResultBuf, bmpWidth, bmpHeight, biBitCount, pColorTable);

	//�����������pBmpBuf��pColorTable��ȫ�ֱ��������ļ�����ʱ����Ŀռ�
	delete [] pBmpBufLeft;
	delete [] pBmpBufRight;
	delete [] pResultBuf;

	if (biBitCount == 8) {
		delete [] pColorTable;
	}
}

bool getSize(char *bmpName)
{
	FILE *fp = fopen(bmpName, "rb");//�����ƶ���ʽ��ָ����ͼ���ļ�

	if (fp == 0) return 0;

	//����λͼ�ļ�ͷ�ṹ BITMAPFILEHEADER

	fseek(fp, sizeof(BITMAPFILEHEADER), 0);

	//����λͼ��Ϣͷ�ṹ��������ȡλͼ��Ϣͷ���ڴ棬����ڱ���head��
	BITMAPINFOHEADER head;

	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);//��ȡͼ����ߣ�ÿλ����ռλ������Ϣ
	//bmpWidth=head.biWidth;

	//bmpHeight=head.biHeight;

	biBitCount = head.biBitCount;//�������������ͼ��ÿ��������ռ���ֽ���(������4�ı���)

	int lineByte = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;
	return 1;
}

bool readBmp(char *bmpName, unsigned char* pBmpBuf)
{
	FILE *fp = fopen(bmpName, "rb");//�����ƶ���ʽ��ָ����ͼ���ļ�

	if (fp == 0)
		return 0;

	//����λͼ�ļ�ͷ�ṹ BITMAPFILEHEADER
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);

	//����λͼ��Ϣͷ�ṹ��������ȡλͼ��Ϣͷ���ڴ棬����ڱ���head��
	BITMAPINFOHEADER head;

	//��ȡͼ����ߣ�ÿλ����ռλ������Ϣ
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);
	
	//bmpWidth=head.biWidth;
	//bmpHeight=head.biHeight;

	biBitCount = head.biBitCount; //�������������ͼ��ÿ��������ռ���ֽ���(������4�ı���)

	int lineByte = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;

	//�Ҷ�ͼ������ɫ������ɫ�����Ϊ256
	if (biBitCount == 8)
	{
		//������ɫ������Ҫ�Ŀռ䣬����ɫ����ڴ�
		pColorTable = new RGBQUAD[256];
		fread(pColorTable, sizeof(RGBQUAD), 256, fp);
	}

	//����λͼ��������Ҫ�Ŀռ䣬��λͼ���ݽ��ڴ�
	//pBmpBuf=new unsigned char[lineByte*bmpHeight];
	fread(pBmpBuf, 1, lineByte*bmpHeight, fp);

	//���ͼ�����Ϣ
	cout << "width=" << bmpWidth << " height=" << bmpHeight << " biBitCount=" << biBitCount << endl;

	fclose(fp);//�ر��ļ�

	return 1;//��ȡ�ļ��ɹ�

}

//����һ��ͼ��λͼ���ݡ����ߡ���ɫ��ָ�뼰ÿ������ռ��λ������Ϣ,����д��ָ���ļ���
bool saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height, int biBitCount, RGBQUAD *pColorTable)
{
	//���λͼ����ָ��Ϊ0����û�����ݴ��룬��������
	if (!imgBuf)
		return 0;

	//��ɫ���С�����ֽ�Ϊ��λ���Ҷ�ͼ����ɫ��Ϊ1024�ֽڣ���ɫͼ����ɫ���СΪ0
	int colorTablesize = 0;

	if (biBitCount == 8)
		colorTablesize = 1024;

	//���洢ͼ������ÿ���ֽ���Ϊ4�ı���
	int lineByte = (width * biBitCount / 8 + 3) / 4 * 4;

	//�Զ�����д�ķ�ʽ���ļ�
	FILE *fp = fopen(bmpName, "wb");

	if (fp == 0)
		return 0;

	//����λͼ�ļ�ͷ�ṹ��������д�ļ�ͷ��Ϣ
	BITMAPFILEHEADER fileHead;

	fileHead.bfType = 0x4D42;//bmp����

	//bfSize��ͼ���ļ�4����ɲ���֮��
	fileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte * height;

	fileHead.bfReserved1 = 0;
	fileHead.bfReserved2 = 0;
	//bfOffBits��ͼ���ļ�ǰ3����������ռ�֮��
	fileHead.bfOffBits = 54 + colorTablesize;

	//д�ļ�ͷ���ļ�
	fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);

	//����λͼ��Ϣͷ�ṹ��������д��Ϣͷ��Ϣ
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

	//дλͼ��Ϣͷ���ڴ�
	fwrite(&head, sizeof(BITMAPINFOHEADER), 1, fp);

	//����Ҷ�ͼ������ɫ��д���ļ� 
	if (biBitCount == 8) {
		fwrite(pColorTable, sizeof(RGBQUAD), 256, fp);
	}

	//дλͼ���ݽ��ļ�
	fwrite(imgBuf, height*lineByte, 1, fp);

	//�ر��ļ�
	fclose(fp);

	return 1;
}

void myProcess()
{
	int lineByte = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;
	pResultBuf = new unsigned char[lineByte*bmpHeight];
	if (biBitCount == 8) //���ڻҶ�ͼ��
	{
	}
	else if (biBitCount == 24)//��ɫͼ��
	{
		static int Matleft[bmpHeight][bmpWidth];//��ͼ
		static int MatRight[bmpHeight][bmpWidth];//��ͼ
		static int D[bmpHeight][bmpWidth];//��¼�Ӳ�ֵ

		static int dif[bmpWidth][deta + 1];//������
		static int dif2[bmpHeight][deta + 1];//������

		static int Emat[deta + 1][bmpWidth];//��¼����ֵ
		static int Emat2[deta + 1][bmpHeight];//��¼����ֵ

		static int Dmat[deta + 1][bmpWidth];//��¼�м��Ӳ�
		static int Dmat2[deta + 1][bmpHeight];//��¼�м��Ӳ�


		//ע��C��ȡͼ��ʱ����ԭ��Ϊ���½�
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
		for (int i = 0; i < bmpHeight; ++i)//ѭ��ÿһ��
		{
			//��ʼ��Emat
			for (int ind_r = 0; ind_r < deta + 1; ++ind_r)
				for (int ind_c = 0; ind_c < bmpWidth; ++ind_c)
					Emat[ind_r][ind_c] = (ind_c == 0 ? 0 : 32767);

			//��ʼ��Dmat
			for (int ind_r = 0; ind_r < deta + 1; ++ind_r)
				for (int ind_c = 0; ind_c < bmpWidth; ++ind_c)
					Dmat[ind_r][ind_c] = 0;

			//��ʼ��dif

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
			//ȡ���Ӳ�
			for (int c = bmpWidth - 1; c >= 0; --c)
			{
				int d = Dmat[minInd][c];
				minInd = d;
				D[i][c] = d;//*8���ӶԱȶȶ���
			}
		}

		//������ȡ�з���ƥ�䣬����֮ǰ�з����ƥ�����еĵ�Ĵ��۽����޸�

		for (int j = 0; j < bmpWidth; ++j)//ѭ��ÿһ��
		{
			//��ʼ��Emat
			for (int ind_r = 0; ind_r < deta + 1; ++ind_r)
				for (int ind_c = 0; ind_c < bmpHeight; ++ind_c)
					Emat2[ind_r][ind_c] = (ind_c == 0 ? 0 : 32767);

			//��ʼ��Dmat
			for (int ind_r = 0; ind_r < deta + 1; ++ind_r)
				for (int ind_c = 0; ind_c < bmpHeight; ++ind_c)
					Dmat2[ind_r][ind_c] = 0;

			//��ʼ��dif

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
			//ȡ���Ӳ�
			for (int c = bmpHeight - 1; c >= 0; --c)
			{
				int d = Dmat2[minInd2][c];
				minInd2 = d;
				D[c][j] = d * 8;//*8���ӶԱȶȶ���
			}
		}
		//��ͼƬ�洢��ʽ��������
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

