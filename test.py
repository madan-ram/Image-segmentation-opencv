import cv2
import time
import numpy
import segment

img = cv2.imread('input/1.jpg')
start_time = time.time()
print len(segment.segment.get_bounded_box(img, 0.5, 500, 20))
end_time = time.time()
print end_time - start_time, 'time spent'