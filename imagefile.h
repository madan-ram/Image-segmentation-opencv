#include "image.h"
#include "misc.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>

image<rgb> *convert_to_imageRGB(const cv::Mat cv_img) {
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
	return im;
}

image<rgb> *imread(const char *name) {

	cv::Mat cv_img;
	cv_img = cv::imread(name, CV_LOAD_IMAGE_COLOR);
	image<rgb> *im = convert_to_imageRGB(cv_img);

	cv_img.release();
	return im;
}

cv::Mat convert_to_cv(const image<rgb> *im) {
	int height = im->height();
	int width = im->width();

	cv::Mat image(height, width, CV_8UC3);

	for(int y=0; y<height; y++) {
		for(int x=0; x<width; x++) {
			cv::Vec3b intensity;
			intensity.val[0] = imRef(im, x, y).b;
			intensity.val[1] = imRef(im, x, y).g;
			intensity.val[2] = imRef(im, x, y).r;
			image.at<cv::Vec3b>(y, x) = intensity;
		}
	}

	return image;
}

void imwrite(const char *name, const image<rgb> *im) {
	/* (*im). <==> img-> */
	int height = im->height();
	int width = im->width();

	cv::Mat image = convert_to_cv(im);
	cv::imwrite(name, image);
	image.release();
}

void rectange(image<rgb> **imPt, const cv::Point p1, const cv::Point p2, const cv::Scalar color) {
	cv::Mat image = convert_to_cv(*imPt);
	cv::rectangle(image, p1, p2, color);
	delete *imPt;
	*imPt = convert_to_imageRGB(image);
	image.release();
}