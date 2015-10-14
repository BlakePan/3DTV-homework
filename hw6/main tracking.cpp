#include <visp/vpConfig.h>
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpMbEdgeTracker.h>
#include <visp/vpOpenCVGrabber.h>
#include <visp/vpDisplayOpenCV.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpDisplayX.h>


int	main()
{
	int number=0;
	vpImage<unsigned char> I;
	char inputimage[300];
	sprintf(inputimage,"video/box_0.jpg");
	vpImageIo::readJPEG(I,inputimage);

    #if defined VISP_HAVE_X11
    vpDisplayX displaywindow;
    #elif defined VISP_HAVE_GDI
    vpDisplayGDI displaywindow;
    #elif defined VISP_HAVE_OPEN_CV
    vpDisplayOpenCV displaywindow;
    #endif
    displaywindow.init(I, 0, 0, "OUTPUT") ; 

	vpMbEdgeTracker tracker;
	vpHomogeneousMatrix cMo;
	tracker.loadConfigFile("box.xml");
	vpCameraParameters camera;
	tracker.getCameraParameters(camera);
	tracker.loadModel("box.wrl");
	
	tracker.initClick(I,"box",0);
	tracker.track(I);
	tracker.getPose(cMo);
	tracker.display(I,cMo,camera,vpColor::red,1);
	vpDisplay::flush(I);

	while(number<318){
		sprintf(inputimage,"video/box_%d.jpg",number);
		vpImageIo::readJPEG(I,inputimage);
		vpDisplay::display(I);
		tracker.track(I);
		tracker.getPose(cMo);
		tracker.display(I,cMo,camera,vpColor::red,1);
		vpDisplay::displayFrame(I,cMo,camera,0.05,vpColor::blue);
		vpDisplay::flush(I);
		//system("pause");
		number++;
	}


	return 0;
}