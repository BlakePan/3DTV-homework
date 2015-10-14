#include	<iostream>
#include	<cv.h>
#include	<highgui.h>
#include	<vector>
#include	<math.h>
#include	<iomanip>  

#define		NUM_POINTS	4
#define		DIMENSION	2
#define		IMG_WINDOW_WIDTH	600
#define		IMG_WINDOW_HEIGHT	450
#define		WINDOW_X			400
#define		WINDOW_Y			150
#define		WINDOW_X2			200
#define		WINDOW_Y2			50
#define		WINDOW_X_SHIFT		15
#define		WINDOW_Y_SHIFT		30
#define		WINDOW_SCALE		0.6

void	onMouse(int event,int x,int y,int flags,void* param);

bool		flag = 0;
double		a_arr[NUM_POINTS*DIMENSION]={};//match point array
double		b_arr[NUM_POINTS*DIMENSION]={};

int		main()
{
	//matrix
	CvMat src_point;
	CvMat dst_point;

	//==========================get pixel from a.jpg==============================//
	//load image
	IplImage *Imagea = cvLoadImage("a.jpg", CV_LOAD_IMAGE_COLOR);	
	//creat window
	cvNamedWindow("a.jpg", 0);	
	//move window
	cvMoveWindow("a.jpg",WINDOW_X,WINDOW_Y);
	//resize window
	cvResizeWindow("a.jpg",IMG_WINDOW_WIDTH,IMG_WINDOW_HEIGHT);	
	//set mouse call back function
	cvSetMouseCallback("a.jpg",onMouse,NULL);	
	//show image a
	cvShowImage("a.jpg",Imagea);
	//wait key "Esc" or get enough information
	std::cout<<"a圖套合到b圖"<<std::endl;
	std::cout<<"請在圖中以滑鼠點選四個匹配點"<<std::endl;
	flag = 0;
	while(true){		
		if(cvWaitKey(10)==27 || flag)
            break;
    }
	//set matrix
	src_point =	cvMat( NUM_POINTS, DIMENSION, CV_64FC1, a_arr);		
	//release
	cvDestroyWindow("a.jpg");

	//==========================get pixel from b.jpg==============================//
	//load image
	IplImage *Imageb = cvLoadImage("b.jpg", CV_LOAD_IMAGE_COLOR);
	//creat window
	cvNamedWindow("b.jpg", 0);
	//move window
	cvMoveWindow("b.jpg",WINDOW_X,WINDOW_Y);
	//resize window
	cvResizeWindow("b.jpg",IMG_WINDOW_WIDTH,IMG_WINDOW_HEIGHT);
	//set mouse call back function
	cvSetMouseCallback("b.jpg",onMouse,NULL);	
	//show image b
	cvShowImage("b.jpg",Imageb);
	//wait key "Esc" or get enough information
	std::cout<<"請依上圖點選順序在此圖中選出四個匹配點"<<std::endl;
	flag = 0;	
	while(true){		
		if(cvWaitKey(10)==27 || flag)//exit
            break;
    }
	//set matrix
	dst_point =	cvMat( NUM_POINTS, DIMENSION, CV_64FC1, b_arr);
	//release
	cvDestroyWindow("b.jpg");
	

	//==========================find Homography matrix==============================//
	double	h_arr[3*3]={};
	CvMat	H= cvMat( 3, 3, CV_64FC1, h_arr);	

	cvFindHomography(&src_point,&dst_point,&H);

	//output homograghy matrix   
    std::cout<<"The homograghy matrix is:"<<std::endl;       
    for ( int i = 0; i < 3; i++ ) {   
        for ( int j = 0; j < 3; j++ ) 
			std::cout <<std::setw(12)<<cvmGet(&H, i, j);    
        std::cout<<std::endl;   
    }
	//==========================output image==============================//
	//Sourse image
	//creat window
	cvNamedWindow("Sourse image", 0);
	//move window
	cvMoveWindow("Sourse image",WINDOW_X2,WINDOW_Y2);
	//resize window
	cvResizeWindow("Sourse image",IMG_WINDOW_WIDTH*WINDOW_SCALE,IMG_WINDOW_HEIGHT*WINDOW_SCALE);
	//show image a
	cvShowImage("Sourse image",Imagea);

	//Destination image
	//creat window
	cvNamedWindow("Destination image", 0);
	//move window
	cvMoveWindow("Destination image",WINDOW_X2+IMG_WINDOW_WIDTH*WINDOW_SCALE+WINDOW_X_SHIFT,WINDOW_Y2);
	//resize window
	cvResizeWindow("Destination image",IMG_WINDOW_WIDTH*WINDOW_SCALE,IMG_WINDOW_HEIGHT*WINDOW_SCALE);
	//show image a
	cvShowImage("Destination image",Imageb);

	//Perspective image
	IplImage *Imagep = cvCreateImage(cvSize(Imagea->width,Imagea->height), IPL_DEPTH_8U, 3);
	//Perspective operation
	cvWarpPerspective(Imagea,Imagep,&H,CV_INTER_LINEAR+(CV_WARP_FILL_OUTLIERS),cvScalarAll(0));
	//creat window
	cvNamedWindow("Perspective image", 0);
	//move window
	cvMoveWindow("Perspective image",WINDOW_X2,WINDOW_Y2+IMG_WINDOW_HEIGHT*WINDOW_SCALE+WINDOW_Y_SHIFT);
	//resize window
	cvResizeWindow("Perspective image",IMG_WINDOW_WIDTH*WINDOW_SCALE,IMG_WINDOW_HEIGHT*WINDOW_SCALE);
	//show image a
	cvShowImage("Perspective image",Imagep);

	//Add destination and perspective image together
	//create image
	IplImage *Image_add = cvCreateImage(cvSize(Imagea->width,Imagea->height), IPL_DEPTH_8U, 3);
	//add image
	cvAddWeighted(Imageb,0.5,Imagep,0.5,0,Image_add);
	//creat window
	cvNamedWindow("ADD image", 0);
	//move window
	cvMoveWindow("ADD image",WINDOW_X2+IMG_WINDOW_WIDTH*WINDOW_SCALE+WINDOW_X_SHIFT,WINDOW_Y2+IMG_WINDOW_HEIGHT*WINDOW_SCALE+WINDOW_Y_SHIFT);
	//resize window
	cvResizeWindow("ADD image",IMG_WINDOW_WIDTH*WINDOW_SCALE,IMG_WINDOW_HEIGHT*WINDOW_SCALE);
	//show add image 
	cvShowImage("ADD image",Image_add);


	//wait for key
	while(cvWaitKey(10)!=27);
	cvDestroyAllWindows();
	system("cls");

	//==========================get pixel from b.jpg==============================//
	//creat window
	cvNamedWindow("b.jpg", 0);
	//move window
	cvMoveWindow("b.jpg",WINDOW_X,WINDOW_Y);
	//resize window
	cvResizeWindow("b.jpg",IMG_WINDOW_WIDTH,IMG_WINDOW_HEIGHT);
	//set mouse call back function
	cvSetMouseCallback("b.jpg",onMouse,NULL);	
	//show image b
	cvShowImage("b.jpg",Imageb);
	//wait key "Esc" or get enough information
	std::cout<<"b圖套合到a圖"<<std::endl;
	std::cout<<"請在圖中以滑鼠點選四個匹配點"<<std::endl;
	
	flag = 0;	
	while(true){		
		if(cvWaitKey(10)==27 || flag)//exit
            break;
    }
	//set matrix
	src_point=	cvMat( NUM_POINTS, DIMENSION, CV_64FC1, a_arr);
	//release
	cvDestroyWindow("b.jpg");

	//==========================get pixel from a.jpg==============================//
	//creat window
	cvNamedWindow("a.jpg", 0);	
	//move window
	cvMoveWindow("a.jpg",WINDOW_X,WINDOW_Y);
	//resize window
	cvResizeWindow("a.jpg",IMG_WINDOW_WIDTH,IMG_WINDOW_HEIGHT);	
	//set mouse call back function
	cvSetMouseCallback("a.jpg",onMouse,NULL);	
	//show image a
	cvShowImage("a.jpg",Imagea);
	//wait key "Esc" or get enough information
	std::cout<<"請依上圖點選順序在此圖中選出四個匹配點"<<std::endl;
	flag = 0;
	while(true){		
		if(cvWaitKey(10)==27 || flag)
            break;
    }
	//set matrix
	dst_point =	cvMat( NUM_POINTS, DIMENSION, CV_64FC1, b_arr);		
	//release
	cvDestroyWindow("a.jpg");

	
	

	//==========================find Homography matrix==============================//
	//double	h_arr[3*3]={};
	//CvMat	H= cvMat( 3, 3, CV_64FC1, h_arr);	

	cvFindHomography(&src_point,&dst_point,&H);

	//output homograghy matrix   
    std::cout<<"The homograghy matrix is:"<<std::endl;       
    for ( int i = 0; i < 3; i++ ) {   
        for ( int j = 0; j < 3; j++ ) 
			std::cout <<std::setw(12)<<cvmGet(&H, i, j);    
        std::cout<<std::endl;   
    }
	//==========================output image==============================//
	//Sourse image
	//creat window
	cvNamedWindow("Sourse image", 0);
	//move window
	cvMoveWindow("Sourse image",WINDOW_X2,WINDOW_Y2);
	//resize window
	cvResizeWindow("Sourse image",IMG_WINDOW_WIDTH*WINDOW_SCALE,IMG_WINDOW_HEIGHT*WINDOW_SCALE);
	//show image a
	cvShowImage("Sourse image",Imageb);

	//Destination image
	//creat window
	cvNamedWindow("Destination image", 0);
	//move window
	cvMoveWindow("Destination image",WINDOW_X2+IMG_WINDOW_WIDTH*WINDOW_SCALE+WINDOW_X_SHIFT,WINDOW_Y2);
	//resize window
	cvResizeWindow("Destination image",IMG_WINDOW_WIDTH*WINDOW_SCALE,IMG_WINDOW_HEIGHT*WINDOW_SCALE);
	//show image a
	cvShowImage("Destination image",Imagea);

	//Perspective image
	//IplImage *Imagep = cvCreateImage(cvSize(Imagea->width,Imagea->height), IPL_DEPTH_8U, 3);
	//Perspective operation
	cvWarpPerspective(Imageb,Imagep,&H,CV_INTER_LINEAR+(CV_WARP_FILL_OUTLIERS),cvScalarAll(0));
	//creat window
	cvNamedWindow("Perspective image", 0);
	//move window
	cvMoveWindow("Perspective image",WINDOW_X2,WINDOW_Y2+IMG_WINDOW_HEIGHT*WINDOW_SCALE+WINDOW_Y_SHIFT);
	//resize window
	cvResizeWindow("Perspective image",IMG_WINDOW_WIDTH*WINDOW_SCALE,IMG_WINDOW_HEIGHT*WINDOW_SCALE);
	//show image a
	cvShowImage("Perspective image",Imagep);

	//Add destination and perspective image together
	//create image
	//IplImage *Image_add = cvCreateImage(cvSize(Imagea->width,Imagea->height), IPL_DEPTH_8U, 3);
	//add image
	cvAddWeighted(Imagea,0.5,Imagep,0.5,0,Image_add);
	//creat window
	cvNamedWindow("ADD image", 0);
	//move window
	cvMoveWindow("ADD image",WINDOW_X2+IMG_WINDOW_WIDTH*WINDOW_SCALE+WINDOW_X_SHIFT,WINDOW_Y2+IMG_WINDOW_HEIGHT*WINDOW_SCALE+WINDOW_Y_SHIFT);
	//resize window
	cvResizeWindow("ADD image",IMG_WINDOW_WIDTH*WINDOW_SCALE,IMG_WINDOW_HEIGHT*WINDOW_SCALE);
	//show add image 
	cvShowImage("ADD image",Image_add);


	//wait for key
	while(cvWaitKey(10)!=27);
	//release
	cvReleaseImage(&Imagea);
	cvReleaseImage(&Imageb);
	cvReleaseImage(&Imagep);
	cvReleaseImage(&Image_add);

	cvDestroyAllWindows();
	//system("pause");
}
void	onMouse(int Event,int x,int y,int flags,void* param)
{
	static	int point_count = 0;
	static	int click_count = 0;
	static	int x_index = 0;

	if(Event==1){//mouse lefrt click
		point_count++;	
		click_count++;	

		printf("%d( %d, %d) \n",point_count,x,y);	

		if(click_count<=NUM_POINTS){			
			a_arr[x_index] = x;
			a_arr[x_index+1] = y;
			x_index+=2;
		}else{						
			b_arr[x_index] = x;
			b_arr[x_index+1] = y;
			x_index+=2;
		}

		if(point_count == NUM_POINTS){//get enough points		
			flag = 1;
			point_count = 0;
			x_index = 0;
		}else
			flag = 0;
		
	}
	//printf("( %d, %d) ",x,y);
    //printf("The Event is : %d ",Event);
    //printf("The flags is : %d \n",flags);
   // printf("The param is : %d\n",param);
}