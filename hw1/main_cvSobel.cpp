#include	<cv.h>
#include	<highgui.h>

void	main()
{	
	IplImage *Image		= cvLoadImage("Baboon.jpg");
	IplImage *Gray		= cvCreateImage(cvGetSize(Image),Image->depth,1);
	IplImage *Edge		= cvCreateImage(cvGetSize(Image),Image->depth,1);
	
	//Because after cvSobel operating, there would be negtive value(s) in the image.
	//We have to use the parameter,IPL_DEPTH_16S(16-bit signed),to store the data.
	IplImage *X_Edge=cvCreateImage(cvGetSize(Image),IPL_DEPTH_16S,1);
	IplImage *Y_Edge=cvCreateImage(cvGetSize(Image),IPL_DEPTH_16S,1);	

	//covert Image to Gray
	cvCvtColor(Image, Gray, CV_RGB2GRAY);	
	
	//use cvSobel
	cvSobel(Gray, X_Edge, 1, 0, 3);
	cvSobel(Gray, Y_Edge, 0, 1, 3);	

	//cvShowImage have to use 8-bit unsigned type data to show image.
	//So we can use cvConvertScaleAbs to convert the image.
	IplImage *X_Edge8u=cvCreateImage(cvGetSize(X_Edge),IPL_DEPTH_8U,1);
	IplImage *Y_Edge8u=cvCreateImage(cvGetSize(Y_Edge),IPL_DEPTH_8U,1);
	
	cvConvertScaleAbs(X_Edge,X_Edge8u,1,0);
	cvConvertScaleAbs(Y_Edge,Y_Edge8u,1,0);

	//Add two image
	cvAdd(X_Edge8u, Y_Edge8u, Edge);
	cvThreshold(Edge, Edge, 254, 255, CV_THRESH_BINARY);

	//show original image
	cvNamedWindow(" Load Image", CV_WINDOW_AUTOSIZE);
	cvShowImage(" Load Image", Image);
	//show X_Edge image
	cvNamedWindow(" X_Edge Image", CV_WINDOW_AUTOSIZE);
	cvShowImage(" X_Edge Image", X_Edge8u);
	//show Y_Edge image
	cvNamedWindow(" Y_Edge Image", CV_WINDOW_AUTOSIZE);
	cvShowImage(" Y_Edge Image", Y_Edge8u);
	//show Edge image
	cvNamedWindow(" Edge Image", CV_WINDOW_AUTOSIZE);
	cvShowImage(" Edge Image", Edge);


	//wait for key
	cvWaitKey(0);

	//release
	cvDestroyWindow(" Load Imag");		
	cvDestroyWindow(" X_Edge Imag");	
	cvDestroyWindow(" Y_Edge Imag");	
	cvDestroyWindow(" Edge Imag");	

	cvReleaseImage(&Image);	
	cvReleaseImage(&X_Edge);
	cvReleaseImage(&Y_Edge);
	cvReleaseImage(&Edge);
}