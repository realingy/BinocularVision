#include "readAndSave.h"
//  [8/23/2013 think]
//��ΪԤ�ȸ�����ȡ��ͼƬ��С
extern unsigned char *pBmpBufLeft;//������ͼ�����ݵ�ָ��
extern unsigned char *pBmpBufRight;//������ͼ�����ݵ�ָ��

extern unsigned char *pResultBuf;//���ͼ�����ݵ�ָ��
//extern const int bmpWidth;//ͼ���
//extern const int bmpHeight;//ͼ���
extern RGBQUAD *pColorTable;//��ɫ��ָ��
extern int biBitCount;//ͼ�����ͣ�ÿ����λ��

void myProcess();
//--------------------------------------------------------------
		//����Ϊ���صĶ�ȡ����
void xiang_su_du_qu(char* readPath1,char* readPath2)

{
    //����ָ��BMP�ļ����ڴ�
	getSize(readPath1);
	int lineByte=(bmpWidth*biBitCount/8+3)/4*4;

	pBmpBufLeft=new unsigned char[lineByte*bmpHeight];
	pBmpBufRight=new unsigned char[lineByte*bmpHeight];
    readBmp(readPath1,pBmpBufLeft);
	readBmp(readPath2,pBmpBufRight);
	


    //ѭ��������ͼ�������
    //ÿ���ֽ���
	
	myProcess();
    
    //��ͼ�����ݴ���
    
    char writePath[]="result.BMP";//ͼƬ������ٴ洢

    saveBmp(writePath, pResultBuf, bmpWidth, bmpHeight, biBitCount, pColorTable);

    //�����������pBmpBuf��pColorTable��ȫ�ֱ��������ļ�����ʱ����Ŀռ�

    delete []pBmpBufLeft;
	delete []pBmpBufRight;
	delete []pResultBuf;
    if(biBitCount==8)

        delete []pColorTable;

}
