#include "readAndSave.h"
//extern const int bmpWidth=384;//图像宽
//extern const int bmpHeight=288;//图像高


unsigned char *pBmpBufLeft;//读入左图像数据的指针
unsigned char *pBmpBufRight;//读入右图像数据的指针
unsigned char *pResultBuf;//结果图像数据的指针
RGBQUAD *pColorTable;//颜色表指针
int biBitCount;//图像类型，每像素位数
//----------------------------------------------------------------------------------------------------
bool getSize(char *bmpName)
{
	FILE *fp=fopen(bmpName,"rb");//二进制读方式打开指定的图像文件

	if(fp==0) return 0;

	//跳过位图文件头结构 BITMAPFILEHEADER

	fseek(fp,sizeof(BITMAPFILEHEADER),0);

	//定义位图信息头结构变量，读取位图信息头进内存，存放在变量head中
	BITMAPINFOHEADER head;

	fread(&head,sizeof(BITMAPINFOHEADER),1,fp);//获取图像宽，高，每位像素占位数等信息
	//bmpWidth=head.biWidth;

	//bmpHeight=head.biHeight;

	biBitCount=head.biBitCount;//定义变量，计算图像每行像素所占的字节数(必须是4的倍数)

	int lineByte=(bmpWidth*biBitCount/8+3)/4*4;
	return 1;
}

bool readBmp(char *bmpName,unsigned char* pBmpBuf)
{
	FILE *fp=fopen(bmpName,"rb");//二进制读方式打开指定的图像文件

	if(fp==0) return 0;

	//跳过位图文件头结构 BITMAPFILEHEADER

	fseek(fp,sizeof(BITMAPFILEHEADER),0);

	//定义位图信息头结构变量，读取位图信息头进内存，存放在变量head中
	BITMAPINFOHEADER head;

	fread(&head,sizeof(BITMAPINFOHEADER),1,fp);//获取图像宽，高，每位像素占位数等信息
	//bmpWidth=head.biWidth;

	//bmpHeight=head.biHeight;

	biBitCount=head.biBitCount;//定义变量，计算图像每行像素所占的字节数(必须是4的倍数)

	int lineByte=(bmpWidth*biBitCount/8+3)/4*4;
	//灰度图像有颜色表，且颜色表表项为256

	if(biBitCount==8)
	{
		//申请颜色表所需要的空间，读颜色表进内存
		pColorTable=new RGBQUAD[256];

		fread(pColorTable,sizeof(RGBQUAD),256,fp);
	}

	//申请位图数据所需要的空间，读位图数据进内存
	//pBmpBuf=new unsigned char[lineByte*bmpHeight];

	fread(pBmpBuf,1,lineByte*bmpHeight,fp);
	//输出图像的信息
	cout<<"width="<<bmpWidth<<" height="<<bmpHeight<<" biBitCount="<<biBitCount<<endl;


	fclose(fp);//关闭文件

	return 1;//读取文件成功

}
	//---------------------------------------------------------
	//给定一个图像位图数据、宽、高、颜色表指针及每像素所占的位数等信息,将其写到指定文件中
bool saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height, 

		int biBitCount, RGBQUAD *pColorTable)

	{

		//如果位图数据指针为0，则没有数据传入，函数返回

		if(!imgBuf)

			return 0;

		//颜色表大小，以字节为单位，灰度图像颜色表为1024字节，彩色图像颜色表大小为0

		int colorTablesize=0;

		if(biBitCount==8)

			colorTablesize=1024;

		//待存储图像数据每行字节数为4的倍数

		int lineByte=(width * biBitCount/8+3)/4*4;

		//以二进制写的方式打开文件

		FILE *fp=fopen(bmpName,"wb");

		if(fp==0) return 0;

		//申请位图文件头结构变量，填写文件头信息

		BITMAPFILEHEADER fileHead;

		fileHead.bfType = 0x4D42;//bmp类型

		//bfSize是图像文件4个组成部分之和

		fileHead.bfSize= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)

			+ colorTablesize + lineByte*height;

		fileHead.bfReserved1 = 0;

		fileHead.bfReserved2 = 0;

		//bfOffBits是图像文件前3个部分所需空间之和

		fileHead.bfOffBits=54+colorTablesize;

		//写文件头进文件

		fwrite(&fileHead, sizeof(BITMAPFILEHEADER),1, fp);

		//申请位图信息头结构变量，填写信息头信息

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

		//写位图信息头进内存

		fwrite(&head, sizeof(BITMAPINFOHEADER),1, fp);

		//如果灰度图像，有颜色表，写入文件 

		if(biBitCount==8)

			fwrite(pColorTable, sizeof(RGBQUAD),256, fp);

		//写位图数据进文件

		fwrite(imgBuf, height*lineByte, 1, fp);

		//关闭文件

		fclose(fp);

		return 1;

	}
