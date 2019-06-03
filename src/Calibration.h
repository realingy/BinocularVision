#ifndef H_SC_CAMCALIB
#define H_SC_CAMCALIB

class CCamera;

#if 0
typedef Point_<int> Point2i;
typedef Point_<int64> Point2l;
typedef Point_<float> Point2f;
typedef Point_<double> Point2d;
typedef Point2i Point;
#endif

int Calibration(char** , int, CvSize, int, CCamera*);
void  SolveCameraPose(CCamera*, CCamera*,int);//求解两个摄像机的位置关系
#if 0
void  UndistortPoint(const CvPoint2D64d* ptSrc,  CvPoint2D64d* ptDst, const CvMat* intrinsic_matrix, const CvMat* distortion_coeffs );
void  Reconstruct3DPoint(const CvPoint2D64d* ptuvL,CCamera* pCameraLeft,const CvPoint2D64d* ptuvR,CCamera* pCameraRight,CvPoint3D64f* ptXYZ,const int nbPoints);
void  FindCorners(char* strFile,CvPoint2D64d* ptuv,const CvSize CBCornersSize, CCamera* pCamera );
void  SaveResultToFile(char* strFile,CvPoint2D64d* ptuv,const int);
void  ReadResultFromFile(char* strFile,CvPoint2D64d* ptuv,const int nbCornersFound);
#endif

void  UndistortPoint(const CvPoint2D64f* ptSrc,  CvPoint2D64f* ptDst, const CvMat* intrinsic_matrix, const CvMat* distortion_coeffs );
void  Reconstruct3DPoint(const CvPoint2D64f* ptuvL,CCamera* pCameraLeft,const CvPoint2D64f* ptuvR,CCamera* pCameraRight,CvPoint3D64f* ptXYZ,const int nbPoints);
void  FindCorners(char* strFile, CvPoint2D64f* ptuv,const CvSize CBCornersSize, CCamera* pCamera );
void  SaveResultToFile(char* strFile,CvPoint2D64f* ptuv,const int);
void  ReadResultFromFile(char* strFile,CvPoint2D64f* ptuv,const int nbCornersFound);

#endif

