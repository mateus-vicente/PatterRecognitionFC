import numpy as np
import argparse
import cv2
import Tkinter
#import Image, ImageTk

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-c", "--chip", help = "path to the chip file")
ap.add_argument("-s", "--substrate", help = "path to the substrate file")

args = vars(ap.parse_args())
 
# load the image
image_chip = cv2.imread(args["chip"])
image_sub = cv2.imread(args["substrate"])

gray_image_chip = cv2.cvtColor(image_chip, cv2.COLOR_BGR2GRAY)
gray_image_sub = cv2.cvtColor(image_sub, cv2.COLOR_BGR2GRAY)


ret, thresh = cv2.threshold(gray_image_chip,0,255,cv2.THRESH_BINARY_INV+cv2.THRESH_OTSU)


 # noise removal
kernel = np.ones((3,3),np.uint8)
opening = cv2.morphologyEx(thresh,cv2.MORPH_CLOSE,kernel, iterations = 2)

#sure_bg = cv2.dilate(opening,kernel,iterations=3)

## dist_transform = cv2.distanceTransform(opening,cv2.cv.CV_DIST_L2,5)
## ret, sure_fg = cv2.threshold(dist_transform,0.7*dist_transform.max(),255,0)

## sure_fg = np.uint8(sure_fg)
## unknown = cv2.subtract(sure_bg,sure_fg)


## # Marker labelling
## ret, markers = cv2.connectedComponents(sure_fg)
## # Add one to all labels so that sure background is not 0, but 1
## markers = markers+1
## # Now, mark the region of unknown with zero
## markers[unknown==255] = 0

## markers = cv2.watershed(img,markers)
## img[markers == -1] = [255,0,0]

params = cv2.SimpleBlobDetector_Params()
 
# Change thresholds
params.minThreshold = 40;
params.maxThreshold = 255;
 
# Filter by Area.
params.filterByArea = False
params.minArea = 15
 
# Filter by Circularity
params.filterByCircularity = False
params.minCircularity = 0.1
 
# Filter by Convexity
params.filterByConvexity = False
params.minConvexity = 0.87
 
# Filter by Inertia
params.filterByInertia = False
params.minInertiaRatio = 0.01
 
# Create a detector with the parameters
ver = (cv2.__version__).split('.')
if int(ver[0]) < 3 :
    detector = cv2.SimpleBlobDetector(params)
else : 
    detector = cv2.SimpleBlobDetector_create(params)
 
# Detect blobs.
keypoints = detector.detect(image_chip)
 
# Draw detected blobs as red circles.
# cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS ensures the size of the circle corresponds to the size of blob
im_with_keypoints = cv2.drawKeypoints(image_chip, keypoints, np.array([]), (0,0,255), cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
 
# Show keypoints
cv2.namedWindow("Keypoints",cv2.WINDOW_NORMAL)
cv2.imshow("Keypoints", im_with_keypoints)
cv2.waitKey(0)





