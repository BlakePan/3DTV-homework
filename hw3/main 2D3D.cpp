#include	<iostream>
#include	<cv.h>
#include	<highgui.h>
#include	<vector>
#include	<math.h>

using	std::vector;
using	std::string;
using	std::cout;
using	std::endl;

#define		WINDOW_WIDTH	600
#define		WINDOW_HEIGHT	450
#define		WINDOW_X		400
#define		WINDOW_Y		200
#define		WINDOW_X_SHIFT	15
#define		WINDOW_Y_SHIFT	30
#define		ANGLE_RESU		0.1
#define		LINE_TH			30
#define		PI				3.14159265

bool	click_flag = false;
int		center_x = 0;
int		center_y = 0;

void	onMouse(int Event,int x,int y,int flags,void* param)
{
	if(Event==1){
		center_x = x;
		center_y = y;
		click_flag = true;
	}
}
void	ShowImageOnWindow(IplImage* img,string window_name,
						  size_t window_width = WINDOW_WIDTH,
						  size_t window_height = WINDOW_HEIGHT,
						  size_t x = WINDOW_X,size_t y = WINDOW_Y)
{
	//create window
	cvNamedWindow(window_name.c_str(), 0);
	//move window
	cvMoveWindow(window_name.c_str(),x,y);
	//resize window
	cvResizeWindow(window_name.c_str(),window_width,window_height);	
	//set mouse call back function
	cvSetMouseCallback(window_name.c_str(),onMouse,NULL);
	//show image 
	cvShowImage(window_name.c_str(),img);
	//wait for key
	while(cvWaitKey(10)!=27 && !click_flag);
	click_flag = false;
	cvDestroyAllWindows();
}

int		main()
{
	//LOAD IMAGE
	cout<<"原圖"<<endl;
	cout<<"(滑鼠點擊圖片或按ESC鍵離開)"<<endl;
	IplImage *Image = cvLoadImage("2.bmp");
	ShowImageOnWindow(Image,"Load Image ");
	system("cls");

	//CONVERT TO GRAY IMAGE
	cout<<"灰階圖"<<endl;
	cout<<"(滑鼠點擊圖片或按ESC鍵離開)"<<endl;
	IplImage *gary_image = cvCreateImage(cvGetSize(Image),Image->depth,1);
	cvCvtColor(Image, gary_image, CV_BGR2GRAY);
	ShowImageOnWindow(gary_image,"Gary Image ");
	system("cls");

	//DETECT EDGE USING CANNY
	cout<<"Canny 邊緣偵測"<<endl;
	cout<<"(滑鼠點擊圖片或按ESC鍵離開)"<<endl;
	IplImage *edge_image = cvCreateImage(cvGetSize(Image),Image->depth,1);
	cvCanny(gary_image,edge_image, 60 , 20, 3 );
	ShowImageOnWindow(edge_image,"Edge Image ");
	system("cls");

	//SMOOTH
	cvSmooth(edge_image,edge_image);
	//ShowImageOnWindow(edge_image,"Smooth Edge Image ");

	//ENFORCE EDGE
	IplImage	*X_Edge		=cvCreateImage(cvGetSize(Image),IPL_DEPTH_16S,1);
	IplImage	*Y_Edge		=cvCreateImage(cvGetSize(Image),IPL_DEPTH_16S,1);		
	IplImage	*S_Edge		=cvCreateImage(cvGetSize(Image),IPL_DEPTH_32F,1);
	IplImage	*S_Edge8u	=cvCreateImage(cvGetSize(X_Edge),IPL_DEPTH_8U,1);

	cvSobel(edge_image, X_Edge, 1, 0, 3);
	cvSobel(edge_image, Y_Edge, 0, 1, 3);

	cvPow(X_Edge, X_Edge, 2);
	cvPow(Y_Edge, Y_Edge, 2);
	cvAdd(X_Edge, Y_Edge, Y_Edge);

	cvConvertScale(Y_Edge,S_Edge,1,0);
	cvPow(S_Edge, S_Edge, 0.5);

	cvConvertScaleAbs(S_Edge,S_Edge8u,1,0);
	cvThreshold(edge_image, S_Edge8u, 20, 255,CV_THRESH_BINARY);
	cout<<"強化邊緣"<<endl;
	cout<<"(滑鼠點擊圖片或按ESC鍵離開)"<<endl;
	ShowImageOnWindow(S_Edge8u,"Enforce Edge Image ");
	system("cls");
	
	//SEARCH LINES	
	IplImage	*tar_img = edge_image;
	cout<<"請點選圓心"<<endl;
	ShowImageOnWindow(S_Edge8u,"Target Image");
	//center_x = tar_img->width/2;
	//center_y = tar_img->height/2;
	cout<<"Center: ("<<center_x<<","<<center_y<<")"<<endl;

	vector<vector<int>> Lines;//{start_x,start_y,end_x,end_y}...


	for(double a=0;a<360;a+=ANGLE_RESU){
		int count = 0;
		int	start_x = center_x;
		int	start_y = center_y;

		for(int x=0;x<tar_img->width/2;x++){
			int y = 0;
			int x_shift = 0;
			int y_shift = 0;

			if(a>0 && a<90){
				y = x*tan(a*PI/180);
				x_shift = x;
				y_shift = y;
			}else if(a>90 && a<=180){
				y = x*tan((180-a)*PI/180);
				x_shift = -x;
				y_shift = y;
			}else if(a>180 && a<270){
				y = x*tan((a-180)*PI/180);
				x_shift = -x;
				y_shift = -y;
			}else if(a>270 && a<=360){
				y = x*tan((360-a)*PI/180);
				x_shift = x;
				y_shift = -y;
			}

			int pixel_x = center_x+x_shift;
			int pixel_y = center_y+y_shift;

			if(pixel_x<=tar_img->width && pixel_x>=0 && pixel_y<=tar_img->height && pixel_y>=0){
				int	pixel_value = tar_img->imageData[pixel_x+pixel_y*tar_img->width];				

				if(pixel_value>0){
					count++;
					if(count>=LINE_TH){
						vector<int> points;
						points.push_back(start_x);
						points.push_back(start_y);
						points.push_back(pixel_x);//end_x
						points.push_back(pixel_y);//end_y
						Lines.push_back(points);
					}
				}else{
					count = 0;
					start_x = pixel_x;
					start_y = pixel_y;
				}
			}			
		}		
	}
	
	for(int i=0;i<Lines.size();i++){
		CvPoint from = cvPoint(Lines[i][0],Lines[i][1]);
		CvPoint end = cvPoint(Lines[i][2],Lines[i][3]);
		cvLine( Image, from, end, CV_RGB( 255, 0, 0 ) ,1);
	}
	cout<<"偵測垂直於地面之直線"<<endl;
	cout<<"(滑鼠點擊圖片或按ESC鍵離開)"<<endl;
	ShowImageOnWindow(Image,"Final Result");	

	//release
	cvReleaseImage(&Image);
	cvReleaseImage(&gary_image);
	cvReleaseImage(&edge_image);
	cvReleaseImage(&X_Edge);
	cvReleaseImage(&Y_Edge);
	cvReleaseImage(&S_Edge);
	cvReleaseImage(&S_Edge8u);
	cvDestroyAllWindows();

	return	0;
}