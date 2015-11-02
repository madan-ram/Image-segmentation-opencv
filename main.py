import cv2
import sys
import time
import math
import numpy
import segment
from utils import *

def filter_by_size(bounded_box_list):
	new_bounded_box_list = []
	for i, bb in enumerate(bounded_box_list):
		dist = math.sqrt((bb[0]-bb[2])**2+(bb[1] - bb[3])**2)
		if  dist >= 100:
			new_bounded_box_list.append(bounded_box_list[i])
	return new_bounded_box_list

def filter_by_boundray_ratio(bounded_box_list):
	new_bounded_box_list = []
	for i, bb in enumerate(bounded_box_list):
		try:
			ratio = float(abs(bb[0]-bb[2]))/float(abs(bb[1]-bb[3]))
			if  ratio <= 3.0 and 1/ratio <= 3.0:
				new_bounded_box_list.append(bounded_box_list[i])
		except ZeroDivisionError:
			pass

	return new_bounded_box_list


if __name__ == '__main__':
	img = cv2.imread(sys.argv[1])
	img = reshape_image(img, frame_size=(1024, 1024, 3), mode='fit')
	
	print 'creating eastimated possible bounded box'
	start_time = time.time()
	bounded_box_list = segment.get_bounded_box(img, 0.5, 500, 20)
	end_time = time.time()
	print end_time - start_time, 'time spent'
		
	bounded_box_list = filter_by_boundray_ratio(filter_by_size(bounded_box_list))

	# for bb in bounded_box_list:
	# 	temp = img.copy()
	# 	cv2.rectangle(img, (bb[0], bb[1]) , (bb[2] ,bb[3]), (0,255,0), 2)
	# cv2.imwrite(sys.argv[2], img)