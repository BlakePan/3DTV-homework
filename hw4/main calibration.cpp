#include	"cv.h"
#include	"highgui.h"
#include	<iostream>
#include	<fstream>
#include	<string>
#include	<vector>
#include	<math.h>
#include	<iomanip>

using	std::string;
using	std::vector;
using	std::cout;
using	std::endl;
using	std::setprecision;

#define	XO 512
#define	YO 384 

FILE*	Open_file(string name)
{
	FILE* f;
	if((f=fopen(name.c_str(),"r"))==NULL)
	{
 		printf("cannot open file\n");
 		exit(1);
	} 
	return f;
}

void	PrintMatrix(CvMat *Matrix,int Rows,int Cols)
{
    for(int i=0;i<Rows;i++){
        for(int j=0;j<Cols;j++){
			cout<<Matrix->data.db[i*Cols+j]<<" ";
        }
		cout<<endl;
    }
}

void	Save2vec(FILE* fp,vector<double>& data)
{
	while(!feof(fp)){
		float tmp = 0;
		fscanf(fp,"%f",&tmp);
		data.push_back((double)tmp);
	}
	data.pop_back();

	int cnt = 0;
	for(int i=0;i<data.size();i++){		
		if(cnt == 3){
			data[i] = -(data[i]-XO);			
		}else if(cnt == 4){
			data[i] = -(data[i]-YO);
			cnt = 0;
			continue;
		}		
		cnt++;		
	}
}

void	Conv_vec2cvMat(vector<double> src,int row,int col,CvMat* arr)
{
	double* Data = new double[src.size()];
	std::copy(src.begin(),src.end(),Data);
	*arr = cvMat(row,col,CV_64FC1,Data);
}

int		main()
{
	//Open file 
	FILE* fp = NULL;
	string file_name("Nonconplanar_pts.txt");
	fp = Open_file(file_name);

	//Read file	
	vector<double> data;
	Save2vec(fp,data);
	fclose(fp);	
	//Convert to Opencv matrix
	CvMat	Matrix_Data;
	int		data_size = data.size();
	Conv_vec2cvMat(data,data_size/5,5,&Matrix_Data);

	//Find matrix A
	vector<double> a_arr;
	for(int i=0;i<data_size/5;i++){
		double x,y;//camera
		double X,Y,Z;//world
		X = Matrix_Data.data.db[i*5];
		Y = Matrix_Data.data.db[i*5+1];
		Z = Matrix_Data.data.db[i*5+2];
		x = Matrix_Data.data.db[i*5+3];
		y = Matrix_Data.data.db[i*5+4];

		a_arr.push_back(x*X);
		a_arr.push_back(x*Y);
		a_arr.push_back(x*Z);
		a_arr.push_back(x);
		a_arr.push_back(-y*X);
		a_arr.push_back(-y*Y);
		a_arr.push_back(-y*Z);
		a_arr.push_back(-y);
	}
	//Convert to Opencv matrix 
	int a_arr_size = a_arr.size();
	CvMat Matrix_A;
	Conv_vec2cvMat(a_arr,a_arr_size/8,8,&Matrix_A);

	//Find matrix V	
	int m = Matrix_A.rows;
	int n = Matrix_A.cols;
	CvMat *W=cvCreateMat(m,n,CV_64FC1);
	CvMat *U=cvCreateMat(m,m,CV_64FC1);
    CvMat *V=cvCreateMat(n,n,CV_64FC1); 
	cvSVD(&Matrix_A,W,U,V);

	//Get vector V
	vector<double> v_vec;
	for(int i=0;i<8;i++){
		v_vec.push_back(V->data.db[i*8+7]);
	}

	//Get parameters
	double	gama=0;
	double	alpha=0;

	double	gama2 = (v_vec[0]*v_vec[0]+v_vec[1]*v_vec[1]+v_vec[2]*v_vec[2]);
	gama = sqrt(gama2);	

	double	alpha_gama = (v_vec[4]*v_vec[4]+v_vec[5]*v_vec[5]+v_vec[6]*v_vec[6]);
	alpha = sqrt(alpha_gama)/gama;

	double	r21 = v_vec[0]/gama;
	double	r22 = v_vec[1]/gama;
	double	r23 = v_vec[2]/gama;
	double	Ty	= v_vec[3]/gama;
	double	r11 = v_vec[4]/alpha/gama;
	double	r12 = v_vec[5]/alpha/gama;
	double	r13 = v_vec[6]/alpha/gama;
	double	Tx	= v_vec[7]/alpha/gama;

	double	r31 = r12*r23-r13*r22;
	double	r32	= -(r11*r23-r13*r21);
	double	r33	= r11*r22-r12*r21;

	double	Tz = 0;
	double	fx = 0;
	double	fy = 0;

	CvMat *A=cvCreateMat(m,2,CV_64FC1);
	CvMat *b=cvCreateMat(m,1,CV_64FC1);
	CvMat *A_T=cvCreateMat(2,m,CV_64FC1);
	CvMat *A_TA=cvCreateMat(2,2,CV_64FC1);
	CvMat *AAA=cvCreateMat(2,m,CV_64FC1);
	CvMat *matrix_x=cvCreateMat(2,1,CV_64FC1);
	
	for(int i=0;i<m;i++){
		double x,y;//camera
		double X,Y,Z;//world
		X = Matrix_Data.data.db[i*5];
		Y = Matrix_Data.data.db[i*5+1];
		Z = Matrix_Data.data.db[i*5+2];
		x = Matrix_Data.data.db[i*5+3];
		y = Matrix_Data.data.db[i*5+4];
		A->data.db[i*2] = x;
		A->data.db[i*2+1] = r11*X+r12*Y+r13*Z+Tx;
		b->data.db[i] = -x*(r31*X+r32*Y+r33*Z);
	}
	cvTranspose(A,A_T);
	cvmMul(A_T,A,A_TA);
	cvmInvert(A_TA,A_TA);
	cvmMul(A_TA,A_T,AAA);
	cvmMul(AAA,b,matrix_x);
	Tz = matrix_x->data.db[0];
	fx = matrix_x->data.db[1];
	fy = fx/alpha;

	system("cls");
	cout<<"Parameters"<<endl;
	cout<<"Gama: "<<setprecision(6)<<gama<<endl;
	cout<<"Alpha: "<<setprecision(6)<<alpha<<endl;	
	cout<<"Transport: "<<endl;
	cout<<setprecision(6)<<Tx<<"\t"<<setprecision(6)<<Ty<<"\t"<<setprecision(6)<<Tz<<endl;
	cout<<"Rotation: "<<endl;
	cout<<setprecision(6)<<r11<<"\t"<<setprecision(6)<<r12<<"\t"<<setprecision(6)<<r13<<endl;
	cout<<setprecision(6)<<r21<<"\t"<<setprecision(6)<<r22<<"\t"<<setprecision(6)<<r23<<endl;
	cout<<setprecision(6)<<r31<<"\t"<<setprecision(6)<<r32<<"\t"<<setprecision(6)<<r33<<endl;
	cout<<"Focal length: "<<endl;
	cout<<"fx: "<<setprecision(6)<<fx<<endl;
	cout<<"fy: "<<setprecision(6)<<fy<<endl;
	system("pause");

	return	0;
}