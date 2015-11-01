import cv2
import segment
img = cv2.imread('input/0.jpg')
print (segment.get_bounded_box(img, 0.5, 1000, 10))