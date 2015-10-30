/*
Copyright (C) 2006 Pedro Felzenszwalb

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#include <cstdio>
#include <cstdlib>
#include <image.h>
#include <misc.h>
#include <imagefile.h>
#include "segment-image.h"

int main(int argc, char **argv) {
  if (argc != 6) {
    fprintf(stderr, "usage: %s sigma k min input(ppm) output(ppm)\n", argv[0]);
    return 1;
  }
  
  float sigma = atof(argv[1]);
  float k = atof(argv[2]);
  int min_size = atoi(argv[3]);

	image<rgb> *input = imread(argv[4]);

  int num_ccs;
  std::list<bounded_box> bounded_box_list = segment_bounded_box(input, sigma, k, min_size, &num_ccs); 
  image<rgb> *seg = segment_image(input, sigma, k, min_size, &num_ccs); 

  for(std::list<bounded_box>::iterator it=bounded_box_list.begin(); it!=bounded_box_list.end();  ++it) {
    printf("%d, %d, %d, %d\n", (*it).min_x, (*it).min_y, (*it).max_x, (*it).max_y);
    rectange(&input, cv::Point((*it).min_x, (*it).min_y), cv::Point((*it).max_x, (*it).max_y), cv::Scalar(0, 0, 255));
  }

  imwrite(argv[5], input);

  return 0;
}

