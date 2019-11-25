#include <iostream>
#include <math.h>
#include "readAndSave.h"

using namespace std;

extern unsigned char *pBmpBufLeft;//������ͼ�����ݵ�ָ��
extern unsigned char *pBmpBufRight;//������ͼ�����ݵ�ָ��
extern unsigned char *pResultBuf;//���ͼ�����ݵ�ָ��
extern RGBQUAD *pColorTable;//��ɫ��ָ��
extern int biBitCount;//ͼ�����ͣ�ÿ����λ��

void rgbToGray(unsigned char*);

void myProcess()
{
	int lineByte=(bmpWidth*biBitCount/8+3)/4*4;
	pResultBuf=new unsigned char[lineByte*bmpHeight];
    if(biBitCount==8) //���ڻҶ�ͼ��
	{              
	}	
    else if(biBitCount==24)//��ɫͼ��
	{
		static int Matleft[bmpHeight][bmpWidth];//��ͼ
		static int MatRight[bmpHeight][bmpWidth];//��ͼ
		static int D[bmpHeight][bmpWidth];//��¼�Ӳ�ֵ

		static int dif[bmpWidth][deta+1];//������
		static int dif2[bmpHeight][deta+1];//������

		static int Emat[deta+1][bmpWidth];//��¼����ֵ
		static int Emat2[deta+1][bmpHeight];//��¼����ֵ

		static int Dmat[deta+1][bmpWidth];//��¼�м��Ӳ�
		static int Dmat2[deta+1][bmpHeight];//��¼�м��Ӳ�


		//ע��C��ȡͼ��ʱ����ԭ��Ϊ���½�
		double gray;
		for(int i=0;i<bmpHeight;++i)
		{
			for(int j=0;j<bmpWidth;++j)
			{
				gray=0.5+*(pBmpBufLeft+i*lineByte+j*3+0)*0.114+*(pBmpBufLeft+i*lineByte+j*3+1)*0.587+*(pBmpBufLeft+i*lineByte+j*3+2)*0.299;
				Matleft[bmpHeight-i-1][j]=(int)gray;
				gray=0.5+*(pBmpBufRight+i*lineByte+j*3+0)*0.114+*(pBmpBufRight+i*lineByte+j*3+1)*0.587+*(pBmpBufRight+i*lineByte+j*3+2)*0.299;
				MatRight[bmpHeight-i-1][j]=(int)gray;
			}
		}
		DWORD start_time=GetTickCount();
		for (int i=0;i<bmpHeight;++i)//ѭ��ÿһ��
		{
			//��ʼ��Emat
			for(int ind_r=0;ind_r<deta+1;++ind_r)
				for(int ind_c=0;ind_c<bmpWidth;++ind_c)
					Emat[ind_r][ind_c]=(ind_c==0?0:32767);

			//��ʼ��Dmat
			for(int ind_r=0;ind_r<deta+1;++ind_r)
				for(int ind_c=0;ind_c<bmpWidth;++ind_c)
					Dmat[ind_r][ind_c]=0;

			//��ʼ��dif
			
			for(int ind_c=0;ind_c<bmpWidth;++ind_c)
				for(int ind_r=0;ind_r<deta+1;++ind_r)
					dif[ind_c][ind_r]=abs(Matleft[i][ind_c]-(((ind_c+ind_r)<=bmpWidth)?MatRight[i][ind_c+ind_r]:0));
		
			
			for (int j=1;j<bmpWidth;++j)
			{
				for (int ind_match=0;ind_match<=deta;++ind_match)
				{
					for(int ind_pre=(ind_match-3>=0?(ind_match-3):0);ind_pre<=(ind_match+3<=deta?ind_match+3:deta);++ind_pre)
					{
						int Edata=(dif[j][ind_match]<dataThre?dif[j][ind_match]:dataThre);
						int Esmooth=abs(ind_match-ind_pre)<smoothThre?abs(ind_match-ind_pre):smoothThre;
						int E=Edata+smoothDataRatio*Esmooth+Emat[ind_pre][j-1];
						if (E<Emat[ind_match][j])
						{
							Emat[ind_match][j]=E;
							Dmat[ind_match][j]=ind_pre;
						}
					}
				}
			}
			//[Emin,n]=min(Emat(:,width));
			int minInd=0;
			int max=Emat[0][bmpWidth-1];
			for(int i=0;i<=deta;++i)
			{
			
				if(Emat[i][bmpWidth-1]<max)
				{
					minInd=i;
					max=Emat[i][bmpWidth-1];
				}
			}
			//ȡ���Ӳ�
			for(int c=bmpWidth-1;c>=0;--c)
			{
				int d=Dmat[minInd][c];
				minInd=d;
				D[i][c]=d;//*8���ӶԱȶȶ���
			}
		}

		//������ȡ�з���ƥ�䣬����֮ǰ�з����ƥ�����еĵ�Ĵ��۽����޸�

		for (int j=0;j<bmpWidth;++j)//ѭ��ÿһ��
		{
			//��ʼ��Emat
			for(int ind_r=0;ind_r<deta+1;++ind_r)
				for(int ind_c=0;ind_c<bmpHeight;++ind_c)
					Emat2[ind_r][ind_c]=(ind_c==0?0:32767);

			//��ʼ��Dmat
			for(int ind_r=0;ind_r<deta+1;++ind_r)
				for(int ind_c=0;ind_c<bmpHeight;++ind_c)
					Dmat2[ind_r][ind_c]=0;

			//��ʼ��dif
			
			for(int ind_c=0;ind_c<bmpHeight;++ind_c)
				for(int ind_r=0;ind_r<deta+1;++ind_r)
					dif2[ind_c][ind_r]=(D[ind_c][j]==ind_r?-2:abs(Matleft[ind_c][j]-(((j+ind_r)<=bmpWidth)?MatRight[ind_c][j+ind_r]:0)));
		
			
			for (int i=1;i<bmpHeight;++i)
			{
				for (int ind_match=0;ind_match<=deta;++ind_match)
				{
					for(int ind_pre=(ind_match-3>=0?(ind_match-3):0);ind_pre<=(ind_match+3<=deta?ind_match+3:deta);++ind_pre)
					{
						int Edata=(dif2[i][ind_match]<dataThre?dif2[i][ind_match]:dataThre);
						int Esmooth=abs(ind_match-ind_pre)<smoothThre?abs(ind_match-ind_pre):smoothThre;
						int E=Edata+smoothDataRatio*Esmooth+Emat2[ind_pre][i-1];
						if (E<Emat2[ind_match][i])
						{
							Emat2[ind_match][i]=E;
							Dmat2[ind_match][i]=ind_pre;
						}
					}
				}
			}
			//[Emin,n]=min(Emat(:,width));
			int minInd2=0;
			int max2=Emat2[0][bmpHeight-1];
			for(int i=0;i<=deta;++i)
			{
			
				if(Emat2[i][bmpHeight-1]<max2)
				{
					minInd2=i;
					max2=Emat2[i][bmpHeight-1];
				}
			}
			//ȡ���Ӳ�
			for(int c=bmpHeight-1;c>=0;--c)
			{
				int d=Dmat2[minInd2][c];
				minInd2=d;
				D[c][j]=d*8;//*8���ӶԱȶȶ���
			}
		}
		//��ͼƬ�洢��ʽ��������
		for(int i=0;i<bmpHeight;++i)
		{
			for(int j=0;j<bmpWidth;++j)
			{
				*(pResultBuf+(bmpHeight-i-1)*lineByte+j*3+0)=D[i][j];
				*(pResultBuf+(bmpHeight-i-1)*lineByte+j*3+1)=D[i][j];
				*(pResultBuf+(bmpHeight-i-1)*lineByte+j*3+2)=D[i][j];

			}
		}
	DWORD end_time=GetTickCount();
	std::cout<<end_time-start_time<<"ms";
	}
}
