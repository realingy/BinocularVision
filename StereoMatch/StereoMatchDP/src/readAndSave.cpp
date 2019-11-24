#include "readAndSave.h"
//extern const int bmpWidth=384;//ͼ���
//extern const int bmpHeight=288;//ͼ���


unsigned char *pBmpBufLeft;//������ͼ�����ݵ�ָ��
unsigned char *pBmpBufRight;//������ͼ�����ݵ�ָ��
unsigned char *pResultBuf;//���ͼ�����ݵ�ָ��
RGBQUAD *pColorTable;//��ɫ��ָ��
int biBitCount;//ͼ�����ͣ�ÿ����λ��
//----------------------------------------------------------------------------------------------------
bool getSize(char *bmpName)
{
	FILE *fp=fopen(bmpName,"rb");//�����ƶ���ʽ��ָ����ͼ���ļ�

	if(fp==0) return 0;

	//����λͼ�ļ�ͷ�ṹ BITMAPFILEHEADER

	fseek(fp,sizeof(BITMAPFILEHEADER),0);

	//����λͼ��Ϣͷ�ṹ��������ȡλͼ��Ϣͷ���ڴ棬����ڱ���head��
	BITMAPINFOHEADER head;

	fread(&head,sizeof(BITMAPINFOHEADER),1,fp);//��ȡͼ����ߣ�ÿλ����ռλ������Ϣ
	//bmpWidth=head.biWidth;

	//bmpHeight=head.biHeight;

	biBitCount=head.biBitCount;//�������������ͼ��ÿ��������ռ���ֽ���(������4�ı���)

	int lineByte=(bmpWidth*biBitCount/8+3)/4*4;
	return 1;
}

bool readBmp(char *bmpName,unsigned char* pBmpBuf)
{
	FILE *fp=fopen(bmpName,"rb");//�����ƶ���ʽ��ָ����ͼ���ļ�

	if(fp==0) return 0;

	//����λͼ�ļ�ͷ�ṹ BITMAPFILEHEADER

	fseek(fp,sizeof(BITMAPFILEHEADER),0);

	//����λͼ��Ϣͷ�ṹ��������ȡλͼ��Ϣͷ���ڴ棬����ڱ���head��
	BITMAPINFOHEADER head;

	fread(&head,sizeof(BITMAPINFOHEADER),1,fp);//��ȡͼ����ߣ�ÿλ����ռλ������Ϣ
	//bmpWidth=head.biWidth;

	//bmpHeight=head.biHeight;

	biBitCount=head.biBitCount;//�������������ͼ��ÿ��������ռ���ֽ���(������4�ı���)

	int lineByte=(bmpWidth*biBitCount/8+3)/4*4;
	//�Ҷ�ͼ������ɫ������ɫ�����Ϊ256

	if(biBitCount==8)
	{
		//������ɫ������Ҫ�Ŀռ䣬����ɫ����ڴ�
		pColorTable=new RGBQUAD[256];

		fread(pColorTable,sizeof(RGBQUAD),256,fp);
	}

	//����λͼ��������Ҫ�Ŀռ䣬��λͼ���ݽ��ڴ�
	//pBmpBuf=new unsigned char[lineByte*bmpHeight];

	fread(pBmpBuf,1,lineByte*bmpHeight,fp);
	//���ͼ�����Ϣ
	cout<<"width="<<bmpWidth<<" height="<<bmpHeight<<" biBitCount="<<biBitCount<<endl;


	fclose(fp);//�ر��ļ�

	return 1;//��ȡ�ļ��ɹ�

}
	//---------------------------------------------------------
	//����һ��ͼ��λͼ���ݡ����ߡ���ɫ��ָ�뼰ÿ������ռ��λ������Ϣ,����д��ָ���ļ���
bool saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height, 

		int biBitCount, RGBQUAD *pColorTable)

	{

		//���λͼ����ָ��Ϊ0����û�����ݴ��룬��������

		if(!imgBuf)

			return 0;

		//��ɫ���С�����ֽ�Ϊ��λ���Ҷ�ͼ����ɫ��Ϊ1024�ֽڣ���ɫͼ����ɫ���СΪ0

		int colorTablesize=0;

		if(biBitCount==8)

			colorTablesize=1024;

		//���洢ͼ������ÿ���ֽ���Ϊ4�ı���

		int lineByte=(width * biBitCount/8+3)/4*4;

		//�Զ�����д�ķ�ʽ���ļ�

		FILE *fp=fopen(bmpName,"wb");

		if(fp==0) return 0;

		//����λͼ�ļ�ͷ�ṹ��������д�ļ�ͷ��Ϣ

		BITMAPFILEHEADER fileHead;

		fileHead.bfType = 0x4D42;//bmp����

		//bfSize��ͼ���ļ�4����ɲ���֮��

		fileHead.bfSize= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)

			+ colorTablesize + lineByte*height;

		fileHead.bfReserved1 = 0;

		fileHead.bfReserved2 = 0;

		//bfOffBits��ͼ���ļ�ǰ3����������ռ�֮��

		fileHead.bfOffBits=54+colorTablesize;

		//д�ļ�ͷ���ļ�

		fwrite(&fileHead, sizeof(BITMAPFILEHEADER),1, fp);

		//����λͼ��Ϣͷ�ṹ��������д��Ϣͷ��Ϣ

		BITMAPINFOHEADER head; 

		head.biBitCount=biBitCount;

		head.biClrImportant=0;

		head.biClrUsed=0;

		head.biCompression=0;

		head.biHeight=height;

		head.biPlanes=1;

		head.biSize=40;

		head.biSizeImage=lineByte*height;

		head.biWidth=width;

		head.biXPelsPerMeter=0;

		head.biYPelsPerMeter=0;

		//дλͼ��Ϣͷ���ڴ�

		fwrite(&head, sizeof(BITMAPINFOHEADER),1, fp);

		//����Ҷ�ͼ������ɫ��д���ļ� 

		if(biBitCount==8)

			fwrite(pColorTable, sizeof(RGBQUAD),256, fp);

		//дλͼ���ݽ��ļ�

		fwrite(imgBuf, height*lineByte, 1, fp);

		//�ر��ļ�

		fclose(fp);

		return 1;

	}
