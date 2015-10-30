#include "image.h"
#include "misc.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>

image<rgb> *imread(const char *name) {

	cv::Mat cv_img;
	cv_img = cv::imread(name, CV_LOAD_IMAGE_COLOR);
	cv::Size s = cv_img.size();
	int height = s.height;
	int width = s.width;
	
	image<rgb> *im = new image<rgb>(width, height);
	for(int y=0; y<height; y++) {
		for(int x=0; x<width; x++) {
			cv::Vec3b intensity = cv_img.at<cv::Vec3b>(y, x);
			rgb color_pix;
			color_pix.b = intensity.val[0];
			color_pix.g = intensity.val[1];
			color_pix.r = intensity.val[2];
			imRef(im, x, y) = color_pix;
		}
	}

	cv_img.release();
	return im;
}