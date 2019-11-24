#include "readAndSave.h"
//  [8/23/2013 think]
//改为预先给定读取的图片大小
extern unsigned char *pBmpBufLeft;//读入左图像数据的指针
extern unsigned char *pBmpBufRight;//读入右图像数据的指针

extern unsigned char *pResultBuf;//结果图像数据的指针
//extern const int bmpWidth;//图像宽
//extern const int bmpHeight;//图像高
extern RGBQUAD *pColorTable;//颜色表指针
extern int biBitCount;//图像类型，每像素位数

void myProcess();
//--------------------------------------------------------------
		//以下为像素的读取函数
void xiang_su_du_qu(char* readPath1,char* readPath2)

{
    //读入指定BMP文件进内存
	getSize(readPath1);
	int lineByte=(bmpWidth*biBitCount/8+3)/4*4;

	pBmpBufLeft=new unsigned char[lineByte*bmpHeight];
	pBmpBufRight=new unsigned char[lineByte*bmpHeight];
    readBmp(readPath1,pBmpBufLeft);
	readBmp(readPath2,pBmpBufRight);
	


    //循环变量，图像的坐标
    //每行字节数
	
	myProcess();
    
    //将图像数据存盘
    
    char writePath[]="result.BMP";//图片处理后再存储

    saveBmp(writePath, pResultBuf, bmpWidth, bmpHeight, biBitCount, pColorTable);

    //清除缓冲区，pBmpBuf和pColorTable是全局变量，在文件读入时申请的空间

    delete []pBmpBufLeft;
	delete []pBmpBufRight;
	delete []pResultBuf;
    if(biBitCount==8)

        delete []pColorTable;

}
