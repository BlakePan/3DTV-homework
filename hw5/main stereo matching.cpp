#include <stdlib.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

int main (int argc, char* const argv[])
{
    //Load image
	IplImage* leftImage = cvLoadImage("cones2.png");
	IplImage* rightImage = cvLoadImage("cones6.png");
	IplImage* groundTruth = cvLoadImage("conesdisp2.png");

	//RGB to Gray
	IplImage* grayLeftImage = cvCreateImage(cvSize(leftImage->width, leftImage->height), IPL_DEPTH_8U, 1);
	IplImage* grayRightImage = cvCreateImage(cvSize(rightImage->width, rightImage->height), IPL_DEPTH_8U, 1);
	cvCvtColor(leftImage, grayLeftImage, CV_RGB2GRAY);
	cvCvtColor(rightImage, grayRightImage, CV_RGB2GRAY);

	//Define disparity map
	IplImage* disparityImage = cvCreateImage(cvSize(leftImage->width, leftImage->height), IPL_DEPTH_8U, 1);
	cvSetZero(disparityImage);

	//Define parameters
	int windowSize = 9;
	int w = (int)(windowSize-1)/2;
	int disparityRange = 64;
	int disparity = 0; 
	float scale = 4;

	//Scaning image
	int Height = grayLeftImage->height;
	int Width = grayLeftImage->width;
	bool det_flag = false;

	for(int y=0; y<Height; y++){
		for(int x=0; x<Width; x++){			
			int ssd = 255*255*9*9;//max value
			//Disparity Range
			for(int d=-disparityRange; d<=0; d++){
				//scaning window
				int tmp_ssd = 0;
				det_flag = false;
				for(int j=-w;j<=w;j++){
					for(int i=-w;i<=w;i++){
						if((x+i+d)>=0 && (x+i+d)<Width && (y+j)>=0 && (y+j)<Height){
							det_flag = true;							
							int left_pixel = ((uchar*)(grayLeftImage->imageData+grayLeftImage->widthStep*(y+j)))[x+i];
							int right_pixel = ((uchar*)(grayRightImage->imageData+grayRightImage->widthStep*(y+j)))[x+i+d];
							tmp_ssd+=(left_pixel-right_pixel)*(left_pixel-right_pixel);
						}
					}
				}
				if(tmp_ssd<ssd && det_flag){
					ssd = tmp_ssd;
					disparity = -d;
				}
			}
			//build diparity map
			disparityImage->imageData[y*disparityImage->widthStep+x] = (int)disparity*scale;
		}		
	}					

	//Compare diparity map with ground truth
	//Calculate error rate	
	int sumOfSub = 0;
	int sumOfGT = 0;
	float ErrorRate = 0;
	for(int y=0; y<disparityImage->height; y++)
	{
		 for(int x=0; x<disparityImage->width; x++)
		 {
			  sumOfSub += abs(((uchar*)(groundTruth->imageData+groundTruth->widthStep*y))[x] - ((uchar*)(disparityImage->imageData+disparityImage->widthStep*y))[x]);
			  sumOfGT += ((uchar*)(groundTruth->imageData+groundTruth->widthStep*y))[x];
		 }
	}
	ErrorRate = (float)sumOfSub/sumOfGT;
	printf("¿ù»~²v¬°%f", ErrorRate);

	//Show image
	cvNamedWindow("Right Image", CV_WINDOW_AUTOSIZE);	 
	cvShowImage("Right Image", rightImage);			
	cvNamedWindow("Left Image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Left Image", leftImage);			
	cvNamedWindow("Ground Truth", CV_WINDOW_AUTOSIZE);			
	cvShowImage("Ground Truth", groundTruth);	                
	cvNamedWindow("Disparity Image", CV_WINDOW_AUTOSIZE);	
	cvShowImage("Disparity Image", disparityImage);			

	//Release
	cvWaitKey(0);
 	cvReleaseImage(&grayRightImage); 
	cvReleaseImage(&grayLeftImage);
	cvReleaseImage(&groundTruth);
	cvReleaseImage(&disparityImage); 
	cvDestroyWindow("Left Image"); 
	cvDestroyWindow("Right Image"); 
	cvDestroyWindow("Ground Truth"); 
	cvDestroyWindow("Dispairty Image"); 

    return 0;
}
