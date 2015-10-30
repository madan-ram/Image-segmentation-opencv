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

#ifndef SEGMENT_IMAGE
#define SEGMENT_IMAGE

#include <cstdlib>
#include <image.h>
#include <misc.h>
#include <filter.h>
#include <list>
#include <set>
#include <unordered_map>
#include <iostream>
#include "segment-graph.h"

typedef struct {
  int x, y;
} Point;

typedef struct {
  int min_x;
  int min_y;
  int max_x;
  int max_y;
} bounded_box;

// random color
rgb random_rgb(){ 
  rgb c;
  double r;
  
  c.r = (uchar)random();
  c.g = (uchar)random();
  c.b = (uchar)random();

  return c;
}

// dissimilarity measure between pixels
static inline float diff(image<float> *r, image<float> *g, image<float> *b,
			 int x1, int y1, int x2, int y2) {
  return sqrt(square(imRef(r, x1, y1)-imRef(r, x2, y2)) +
	      square(imRef(g, x1, y1)-imRef(g, x2, y2)) +
	      square(imRef(b, x1, y1)-imRef(b, x2, y2)));
}

bool compare_x(Point a, Point b) {
  return a.x<b.x; 
}
bool compare_y(Point a, Point b) { 
  return a.y<b.y; 
}

/*
 * Segment an image
 *
 * Returns a color image representing the segmentation.
 *
 * im: image to segment.
 * sigma: to smooth the image.
 * c: constant for treshold function.
 * min_size: minimum component size (enforced by post-processing stage).
 * num_ccs: number of connected components in the segmentation.
 */

 image<rgb> *segment_image(image<rgb> *im, float sigma, float c, int min_size,
        int *num_ccs) {
  int width = im->width();
  int height = im->height();

  image<float> *r = new image<float>(width, height);
  image<float> *g = new image<float>(width, height);
  image<float> *b = new image<float>(width, height);

  // smooth each color channel  
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      imRef(r, x, y) = imRef(im, x, y).r;
      imRef(g, x, y) = imRef(im, x, y).g;
      imRef(b, x, y) = imRef(im, x, y).b;
    }
  }
  image<float> *smooth_r = smooth(r, sigma);
  image<float> *smooth_g = smooth(g, sigma);
  image<float> *smooth_b = smooth(b, sigma);
  delete r;
  delete g;
  delete b;
 
  // build graph
  edge *edges = new edge[width*height*4];
  int num = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (x < width-1) {
  edges[num].a = y * width + x;
  edges[num].b = y * width + (x+1);
  edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y);
  num++;
      }

      if (y < height-1) {
  edges[num].a = y * width + x;
  edges[num].b = (y+1) * width + x;
  edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x, y+1);
  num++;
      }

      if ((x < width-1) && (y < height-1)) {
  edges[num].a = y * width + x;
  edges[num].b = (y+1) * width + (x+1);
  edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y+1);
  num++;
      }

      if ((x < width-1) && (y > 0)) {
  edges[num].a = y * width + x;
  edges[num].b = (y-1) * width + (x+1);
  edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y-1);
  num++;
      }
    }
  }
  delete smooth_r;
  delete smooth_g;
  delete smooth_b;

  // segment
  universe *u = segment_graph(width*height, num, edges, c);
  
  // post process small components
  for (int i = 0; i < num; i++) {
    int a = u->find(edges[i].a);
    int b = u->find(edges[i].b);
    if ((a != b) && ((u->size(a) < min_size) || (u->size(b) < min_size)))
      u->join(a, b);
  }
  delete [] edges;
  *num_ccs = u->num_sets();

  image<rgb> *output = new image<rgb>(width, height);

  // pick random colors for each component
  rgb *colors = new rgb[width*height];
  for (int i = 0; i < width*height; i++)
    colors[i] = random_rgb();
  
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int comp = u->find(y * width + x);
      imRef(output, x, y) = colors[comp];
    }
  }


  delete [] colors;  
  delete u;

  return output;
}

std::list<bounded_box> segment_bounded_box(image<rgb> *im, float sigma, float c, int min_size,
			  int *num_ccs) {
  int width = im->width();
  int height = im->height();

  image<float> *r = new image<float>(width, height);
  image<float> *g = new image<float>(width, height);
  image<float> *b = new image<float>(width, height);

  // smooth each color channel  
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      imRef(r, x, y) = imRef(im, x, y).r;
      imRef(g, x, y) = imRef(im, x, y).g;
      imRef(b, x, y) = imRef(im, x, y).b;
    }
  }
  image<float> *smooth_r = smooth(r, sigma);
  image<float> *smooth_g = smooth(g, sigma);
  image<float> *smooth_b = smooth(b, sigma);
  delete r;
  delete g;
  delete b;
 
  // build graph
  edge *edges = new edge[width*height*4];
  int num = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (x < width-1) {
	edges[num].a = y * width + x;
	edges[num].b = y * width + (x+1);
	edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y);
	num++;
      }

      if (y < height-1) {
	edges[num].a = y * width + x;
	edges[num].b = (y+1) * width + x;
	edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x, y+1);
	num++;
      }

      if ((x < width-1) && (y < height-1)) {
	edges[num].a = y * width + x;
	edges[num].b = (y+1) * width + (x+1);
	edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y+1);
	num++;
      }

      if ((x < width-1) && (y > 0)) {
	edges[num].a = y * width + x;
	edges[num].b = (y-1) * width + (x+1);
	edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y-1);
	num++;
      }
    }
  }
  delete smooth_r;
  delete smooth_g;
  delete smooth_b;

  // segment
  universe *u = segment_graph(width*height, num, edges, c);
  
  // post process small components
  for (int i = 0; i < num; i++) {
    int a = u->find(edges[i].a);
    int b = u->find(edges[i].b);
    if ((a != b) && ((u->size(a) < min_size) || (u->size(b) < min_size)))
      u->join(a, b);
  }
  delete [] edges;
  *num_ccs = u->num_sets();

  /*get unique components*/
  std::set<int> unique_comp;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int comp = u->find(y * width + x);
      unique_comp.insert(comp);
    }
  }

  std::unordered_map <int, int> map_comp;
  /*create map between comp and sequence_id*/
  int count = 0;
  for(std::set<int>::iterator it=unique_comp.begin(); it!=unique_comp.end();  ++it, count++) {
    map_comp[*it] = count;
  }

  /*create mapping id with component*/
  std::list<Point> data[unique_comp.size()];
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int comp = u->find(y * width + x);
      Point p;
      p.x = x;
      p.y = y;
      data[map_comp[comp]].push_back(p);
    }
  }

  std::list<bounded_box> bounded_box_list;
  for(std::set<int>::iterator it=unique_comp.begin(); it!=unique_comp.end();  ++it) {
    int max_x = (*std::max_element(data[map_comp[*it]].begin(), data[map_comp[*it]].end(), compare_x)).x;
    int max_y = (*std::max_element(data[map_comp[*it]].begin(), data[map_comp[*it]].end(), compare_y)).y;
    int min_x = (*std::min_element(data[map_comp[*it]].begin(), data[map_comp[*it]].end(), compare_x)).x;
    int min_y = (*std::min_element(data[map_comp[*it]].begin(), data[map_comp[*it]].end(), compare_y)).y;
    bounded_box b;
    b.min_x = min_x;
    b.min_y = min_y;
    b.max_x = max_x;
    b.max_y = max_y;

    bounded_box_list.push_back(b);
  }

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      data[map_comp[y * width + x]].clear();
    }
  }  
  unique_comp.clear();
  delete u;
  return bounded_box_list;
}

#endif
