#include "readAndSave.h"

//��ΪԤ�ȸ�����ȡ��ͼƬ��С

extern unsigned char *pBmpBufLeft;//������ͼ�����ݵ�ָ��
extern unsigned char *pBmpBufRight;//������ͼ�����ݵ�ָ��
extern unsigned char *pResultBuf;//���ͼ�����ݵ�ָ��

//extern const int bmpWidth;//ͼ���
//extern const int bmpHeight;//ͼ���

extern RGBQUAD *pColorTable;//��ɫ��ָ��
extern int biBitCount;//ͼ�����ͣ�ÿ����λ��

void myProcess();

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

