#include	<cv.h>
#include	<highgui.h>
#include	<vector>
//#include	<iostream>

//using	std::cout;
//using	std::endl;
using	std::vector;

IplImage*	Convolution(IplImage* src,vector<int> const mask[])
{
	vector<int> search[9];//row;col
	search[0].push_back(-1);search[0].push_back(-1);
	search[1].push_back(-1);search[1].push_back(0);
	search[2].push_back(-1);search[2].push_back(1);

	search[3].push_back(0);search[3].push_back(-1);
	search[4].push_back(0);search[4].push_back(0);
	search[5].push_back(0);search[5].push_back(1);

	search[6].push_back(1);search[6].push_back(-1);
	search[7].push_back(1);search[7].push_back(0);
	search[8].push_back(1);search[8].push_back(1);

	//output image
	IplImage* out_img = cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_8U ,1);
	//set all pixel to "0"
	for(int row=0;row<src->width;row++){
		for(int col=0;col<src->height;col++){
			out_img->imageData[row*src->width+col] = 0;
		}
	}
	//convolution
	for(int row=1;row<src->width-1;row++){
		for(int col=1;col<src->height-1;col++){
			int		conv_pixel = 0;
			size_t	cnt = 0;
			for(int i=0;i<3;i++){
				for(int j=0;j<3;j++){
					vector<int>::iterator it = search[cnt].begin();
					int	tag_row = row+(*it);it++;
					int	tag_col = col+(*it);
					int tag_pixel = tag_row*(src->width)+tag_col;
					conv_pixel+=mask[i][j]*src->imageData[tag_pixel];
					cnt++;
				}
			}
			out_img->imageData[row*src->width+col] = (uchar)conv_pixel;
		}
	}
	return out_img;
}
void		main()
{
	//Read image
	IplImage *Image		= cvLoadImage("Baboon.jpg", CV_LOAD_IMAGE_COLOR);
	IplImage *Gray		= cvCreateImage(cvSize(Image->width,Image->height),IPL_DEPTH_8U ,1);
	IplImage *Binary	= cvCreateImage(cvSize(Image->width,Image->height),IPL_DEPTH_8U ,1);
	IplImage *X_Edge	= cvCreateImage(cvSize(Image->width,Image->height),IPL_DEPTH_8U ,1);
	IplImage *Y_Edge	= cvCreateImage(cvSize(Image->width,Image->height),IPL_DEPTH_8U ,1);
	IplImage *Edge		= cvCreateImage(cvSize(Image->width,Image->height),IPL_DEPTH_8U ,1);	

	//Step1: 將輸入圖像轉成灰階圖像並二值化
	cvCvtColor(Image, Gray, CV_RGB2GRAY);

	//Step2: 將灰階圖像二值化
	cvThreshold(Gray, Binary, 120, 255, CV_THRESH_BINARY);

	//Step3: 定義 Mask
	//int Mask_X[3][3]={{-1,0,1},{-2,0,2},{-1,0,1}};
	//int Mask_Y[3][3]={{-1,-2,-1},{0,0,0},{1,2,1}};
	vector<int>	Mask_X[3];
	Mask_X[0].push_back(-1);	Mask_X[0].push_back(0);		Mask_X[0].push_back(1);
	Mask_X[1].push_back(-2);	Mask_X[1].push_back(0);		Mask_X[1].push_back(2);
	Mask_X[2].push_back(-1);	Mask_X[2].push_back(0);		Mask_X[2].push_back(1);
	vector<int>	Mask_Y[3];
	Mask_Y[0].push_back(-1);	Mask_Y[0].push_back(-2);	Mask_Y[0].push_back(-1);
	Mask_Y[1].push_back(0);		Mask_Y[1].push_back(0);		Mask_Y[1].push_back(0);
	Mask_Y[2].push_back(1);		Mask_Y[2].push_back(2);		Mask_Y[2].push_back(1);

	//Step4: 與 Mask 做Convolution的動作已找出 X 方向的邊緣
	X_Edge = Convolution(Binary,Mask_X);	

	//Step5: 與 Mask 做Convolution的動作已找出 Y 方向的邊緣
	Y_Edge = Convolution(Binary,Mask_Y);	


	//Step6: 將 X 方向與 Y 方向的邊緣資訊相加
	cvAdd(X_Edge, Y_Edge, Edge);

	//Step7: 防止 Pixel 的灰階值超過 255
	cvThreshold(Edge, Edge, 254, 255, CV_THRESH_BINARY);

	//show original image
	cvNamedWindow(" Load Image", CV_WINDOW_AUTOSIZE);
	cvShowImage(" Load Image", Image);
	//show Gray image
	cvNamedWindow(" Gray Image", CV_WINDOW_AUTOSIZE);
	cvShowImage(" Gray Image", Gray);
	//show Binary image
	cvNamedWindow(" Binary Image", CV_WINDOW_AUTOSIZE);
	cvShowImage(" Binary Image", Binary);
	//show X_Edge image
	cvNamedWindow(" X_Edge Image", CV_WINDOW_AUTOSIZE);
	cvShowImage(" X_Edge Image", X_Edge);
	//show Y_Edge image
	cvNamedWindow(" Y_Edge Image", CV_WINDOW_AUTOSIZE);
	cvShowImage(" Y_Edge Image", Y_Edge);
	//show Edge image
	cvNamedWindow(" Edge Image", CV_WINDOW_AUTOSIZE);
	cvShowImage(" Edge Image", Edge);

	//wait for key
	cvWaitKey(0);

	//release
	cvDestroyWindow(" Load Imag");	
	cvDestroyWindow(" Gray Imag");	
	cvDestroyWindow(" Binary Imag");	
	cvDestroyWindow(" X_Edge Imag");	
	cvDestroyWindow(" Y_Edge Imag");	
	cvDestroyWindow(" Edge Imag");	

	cvReleaseImage(&Image);
	cvReleaseImage(&Gray);
	cvReleaseImage(&Binary);
	cvReleaseImage(&X_Edge);
	cvReleaseImage(&Y_Edge);
	cvReleaseImage(&Edge);
}