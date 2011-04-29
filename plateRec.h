#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

typedef struct plateCandidate {
	int sec;
	int start;
	int end;
} plateCandidate;

typedef struct areaCandidate {
	CvPoint start;
	CvPoint end;
} areaCandidate;

int minNumber(const int a, const int b)
{
	if (a <= b )
		return a;
	else
		return b;
}

int maxNumber(const int a, const int b)
{
	if (a >= b)
		return a;
	else
		return b;
}

CvRect *areaToRect(areaCandidate *area)
{
	CvRect *rect;

	rect = (CvRect *) malloc (sizeof(CvRect));
	rect->x = area->start.x;
	rect->y = area->start.y;
	rect->width = area->end.x - area->start.x;
	rect->height = area->end.y - area->start.y;

	return rect;
}

IplImage *cutImageFromRect(IplImage *img, CvRect *r, int numChannel)
{

	CvRect rect = *r;
	IplImage *x;

	// Adapts rect dimention to image dimention checking low and rigth edge.
	int endx = minNumber(rect.x + rect.width, img->width);
	int endy = minNumber(rect.y + rect.height, img->height);
	rect.width = endx - rect.x;
	rect.height = endy - rect.y;
	// Adapts rect dimention to image dimention checking high and left edge.
	if (rect.x < 0 ) {
		rect.width = rect.x + rect.width;
		rect.x = 0;
	}
	if (rect.y < 0) {
		rect.height = rect.y + rect.height;
		rect.x = 0;
	}

	x = cvCreateImage(cvSize(rect.width, rect.height), IPL_DEPTH_8U, numChannel);

	cvSetImageROI(img, rect);
	cvCopy(img, x, NULL);
	cvResetImageROI(img);
	return x;
}

int saveImage(const IplImage *img, const char *label, int step)
{
	char filename[30];

	sprintf(filename, "output/%s_step%d.jpg", label, step);
	cvSaveImage(filename, img, 0);
	return 0;
}

int matrixBinaryThreshold(int **m, int width, int height,
		    	  int threshold, int minValue, int maxValue)
{
	int i, j;

	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			if (m[i][j] >= threshold)
				m[i][j] = maxValue;
			else
				m[i][j] = minValue;
		}
	}
	return 0;

}

IplImage * scaleImage(IplImage* src , int new_width , int new_height, int numCanali) {
	IplImage* dst = cvCreateImage( cvSize( new_width , new_height ) , 8, numCanali);

	cvResize(src , dst , CV_INTER_NN);
	return dst;
}

