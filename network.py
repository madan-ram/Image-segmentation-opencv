import os
import numpy as np

try:
	caffe_root = os.environ['CAFFE_HOME'] + '/'
except Exception as e:
	print e
	print 'please check $CAFFE_HOME is set'
	print 'default use => /home/invenzone/DIGITS/caffe/'
	caffe_root = '/home/invenzone/DIGITS/caffe/'  # this file is expected to be in {caffe_root}/examples

import sys
sys.path.insert(0, caffe_root + 'python/')
import caffe

BATCH_SIZE = 500
IMAGE_SHAPE = (227, 227, 3)

def create_network(model, weights):
	#set caffe compution mode gpu or cpu
	caffe.set_mode_gpu()

	#load model and weights for testing
	net = caffe.Net(model, weights, caffe.TEST)

	return net

def get_data(net, images):
	#change the shape of blog to accomadate data
	net.blobs['data'].reshape(*images.shape)
	#set the data
	net.blobs['data'].data[...] = images

	#fedforward network to get the layer activation
	output = net.forward()

	#get layerwise activation here from layer fc7 (fully connected layer 7 as name specified in deploy.prototxt)
	predictions = output[net.outputs[0]]
	return predictions