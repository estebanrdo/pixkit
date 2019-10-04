// opencv related headers
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

// system related headers
#include <iostream>

// pixkit-image
#include "../../../modules/pixkit-image/include/pixkit-image.hpp"

int main(int argc, char** argv){

	cv::Mat	src,dst;

	// load image
	src	=	cv::imread("../../../data/lena.bmp",cv::IMREAD_GRAYSCALE);
	if(!src.empty()){
		// process
		if(pixkit::halftoning::dotdiffusion::GuoLiu2009(src,dst,8)){
			// write output
			cv::imwrite("output.bmp",dst);

			// show image
			cv::imshow("src",src);
			cv::imshow("dst",dst);			
			cv::waitKey(0);
		}
	}
        return 0;
}
