import cv2
import sys
import time
import math
import numpy
import network
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

def read_label(path):
	map_id_word = {}
	for i, d in enumerate(open(path)):
		d = d.strip()
		d = ' '.join(d.split()[1:])
		d = d.split(',')[0]
		map_id_word[i] = d

	return map_id_word

def map_max_label_bb(map_id_word, bounded_box_list, max_label, max_label_prob):
	result = []
	for i, bb in enumerate(bounded_box_list):
		result.append({'label': map_id_word[max_label[i]], 'prob':max_label_prob[i],  'bounded_box':bb})
	return result

def generate_random_color():
	return (np.random.randint(0, 255), np.random.randint(0, 255), np.random.randint(0, 255))

model = 'model/deploy.prototxt'
weights = 'model/bvlc_alexnet.caffemodel'
batch_size = network.BATCH_SIZE
map_id_word = read_label('model/label.txt')
net = network.create_network(model, weights)

if __name__ == '__main__':
	img = cv2.imread(sys.argv[1])
	img = reshape_image(img, frame_size=(1024, 1024, 3), mode='fit')
	
	print 'creating eastimated possible bounded box'
	start_time = time.time()
	bounded_box_list = segment.get_bounded_box(img, 0.5, 500, 20)
	end_time = time.time()
	print end_time - start_time, 'time spent'

	bounded_box_list = filter_by_boundray_ratio(filter_by_size(bounded_box_list))
	print "predicting labels for bounded box"

	images = []
	for bb in bounded_box_list:
		sub_img = img[bb[1]:bb[3], bb[0]:bb[2]]
		images.append(create_fixed_image_shape(sub_img, frame_size=network.IMAGE_SHAPE, mode='fit'))
	images = np.asarray(images)

	images = np.transpose(images, (0, 3, 1, 2))

	prob = network.get_data(net, images)
	max_label = np.argmax(prob, axis=1)
	max_label_prob = np.max(prob, axis=1)
	result = map_max_label_bb(map_id_word, bounded_box_list, max_label, max_label_prob)

	for d in result:
		bb = d['bounded_box']
		label = d['label']
		if d['prob'] >= 0.2:
			color = generate_random_color()
			cv2.putText(img, label, (bb[0], bb[1]), cv2.FONT_HERSHEY_SIMPLEX, 1, color, 2)
			cv2.rectangle(img, (bb[0], bb[1]), (bb[2], bb[3]), color, 2)
	cv2.imwrite(sys.argv[2], img)