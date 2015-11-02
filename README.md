
Implementation of the segmentation algorithm described in:

Efficient Graph-Based Image Segmentation
Pedro F. Felzenszwalb and Daniel P. Huttenlocher
International Journal of Computer Vision, 59(2) September 2004.

The program takes a color image and produces a segmentation
with a random color assigned to each region and 
return bounded_box list "min_x, min_y, max_x, max_y".

1) Type "make" to compile "segment".

```python
    import cv2
	import segment
	img = cv2.imread('input/0.jpg')
	bd_l = segment.get_bounded_box(img, 0.5, 1000, 10)
	print bd_l
	>> [[130, 0, 176, 18], [166, 0, 178, 18], [89, 0, 137, 18] ....]
```

The parameters are: (see the paper for details)

sigma: Used to smooth the input image before segmenting it.
k: Value for the threshold function.
min: Minimum component size enforced by post-processing.
input: Input image.
output: Output image.

Typical parameters are sigma = 0.5, k = 500, min = 20.
Larger values for k result in larger components in the result.

