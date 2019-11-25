#include "stereoMatchDP.h"

int main()
{
	char* filePathLeft = "tsuR.bmp";
	char* filePathRight = "tsuL.bmp";

	StereoMatchDP dp;
	dp.process(filePathLeft, filePathRight);

	return 0;
}

