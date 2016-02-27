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

## # find all the 'black' shapes in the image
lower = 62
upper = 255
shapeMask_chip = cv2.inRange(gray_image_chip, lower, upper)
shapeMask_sub = cv2.inRange(gray_image_sub, lower, upper)


## cv2.namedWindow("image_chip",cv2.WINDOW_NORMAL)
## cv2.imshow('image',shapeMask_chip)
## cv2.resizeWindow("image_chip",1000,1000)
## cv2.waitKey(10)

cv2.namedWindow("image_sub",cv2.WINDOW_NORMAL)
## cv2.imshow('image_sub',shapeMask_sub)
## cv2.resizeWindow("image_sub",1000,1000)
## cv2.waitKey(10)

## edges = cv2.Canny(shapeMask_chip,50,150,apertureSize = 3)
## lines = cv2.HoughLinesP(edges,1,np.pi/180,100,minLineLength=100,maxLineGap=10)
## for line in lines:
##     x1,y1,x2,y2 = line[0]
##     cv2.line(image_sub,(x1,y1),(x2,y2),(0,255,0),2)


#a = raw_input()

# find the contours in the chip
(cnts_chip, _) = cv2.findContours(shapeMask_chip.copy(), cv2.RETR_EXTERNAL,
    cv2.CHAIN_APPROX_NONE)
print "I found %d black shapes in chip" % (len(cnts_chip))
cv2.imshow("Mask_chip", shapeMask_chip)

# find the contours in the substrate
(cnts_sub, _) = cv2.findContours(shapeMask_sub.copy(), cv2.RETR_EXTERNAL,
    cv2.CHAIN_APPROX_NONE)
print "I found %d black shapes in substrate" % (len(cnts_sub))
cv2.imshow("Mask_sub", shapeMask_sub)

minArea = 400
maxArea = 22500
# loop over the contours
for c in cnts_chip:
	# draw the contour and show it
    #print c
    area = cv2.contourArea(c)
    print "area = %f"%area
    if area>minArea and area<maxArea : 
        cv2.drawContours(image_sub, [c], -1, (0, 255, 0), 2)
        (x,y),radius = cv2.minEnclosingCircle(c)
        center = (int(x),int(y))
        radius = int(radius)
        cv2.circle(image_sub,center,radius,(0,255,0),2)
                         
# loop over the contours
for c in cnts_sub:
	# draw the contour and show it
    #print c
    area = cv2.contourArea(c)
    print "area = %f"%area
    if area>minArea and area <maxArea : 
        cv2.drawContours(image_sub, [c], -1, (0, 255, 0), 2)
        (x,y),radius = cv2.minEnclosingCircle(c)
        center = (int(x),int(y))
        radius = int(radius)
        cv2.circle(image_sub,center,radius,(0,255,0),2)
    
cv2.imshow("image_sub", image_sub)
cv2.waitKey(00)
