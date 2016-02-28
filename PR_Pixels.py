import numpy as np
import argparse
import cv2
import Tkinter
#import Image, ImageTk
from ROOT import *
from math import sqrt

pixel_to_um = 1.47

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-c", "--chip", help = "path to the chip file")
ap.add_argument("-s", "--substrate", help = "path to the substrate file")

args = vars(ap.parse_args())
 
# load the image
image_chip = cv2.imread(args["chip"])
image_sub = cv2.imread(args["substrate"])

#Transform to Grayscale
gray_image_chip = cv2.cvtColor(image_chip, cv2.COLOR_BGR2GRAY)
gray_image_sub = cv2.cvtColor(image_sub, cv2.COLOR_BGR2GRAY)

#Thresholding using adaptative gaussian algorithm, that smoothen the lighting across the image 
gray_image_sub = cv2.adaptiveThreshold(gray_image_sub,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,13,0)
gray_image_chip = cv2.adaptiveThreshold(gray_image_chip,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,13,2)

#All kind of other filtering, un-used for now
kernel = np.ones((2,2),np.uint8)
kernel = np.ones((2,2),np.float32)/4
#gray_image_sub = cv2.morphologyEx(gray_image_sub, cv2.MORPH_CLOSE, kernel)
#gray_image_sub = cv2.morphologyEx(gray_image_sub, cv2.MORPH_OPEN, kernel)

#gray_image_chip = cv2.morphologyEx(gray_image_chip, cv2.MORPH_CLOSE, kernel)
#gray_image_chip = cv2.morphologyEx(gray_image_chip, cv2.MORPH_OPEN, kernel)
#gray_image_chip = cv2.filter2D(gray_image_chip,-1,kernel)
#gray_image_sub = cv2.filter2D(gray_image_sub,-1,kernel)


#Display the filtered images
cv2.imshow("Mask_sub", gray_image_sub)
cv2.imshow("Mask_chip", gray_image_chip)

cv2.namedWindow("image_sub",cv2.WINDOW_NORMAL)


# find the contours in the chip
(cnts_chip, _) = cv2.findContours(gray_image_chip.copy(), cv2.RETR_EXTERNAL,
    cv2.CHAIN_APPROX_NONE)
print "I found %d black shapes in chip" % (len(cnts_chip))

# find the contours in the substrate
(cnts_sub, _) = cv2.findContours(gray_image_sub.copy(), cv2.RETR_EXTERNAL,
    cv2.CHAIN_APPROX_NONE)
print "I found %d black shapes in substrate" % (len(cnts_sub))





area_chip = TH1D("h3","hx",50000,0,50000)
area_sub = TH1D("h4","hy",50000,0,50000)

centers_chip = []
centers_sub = []


# filter contours to remove uninteresting stuff

#Parameter for a cut on the area of the contours 
minArea_sub = 2000
maxArea_sub = 3000
minArea_chip = 1150
maxArea_chip = 1500

#Parameters for cut on solidity
solidity_min_sub = 0
solidity_min_chip = 0.5

#Parameter for cut on aspect ratio
aspect_ratio_min_chip = 0.3
aspect_ratio_max_chip = 1.8
aspect_ratio_min_sub = 0.5
aspect_ratio_max_sub = 1.5

new_cnts_chip = []

for i,c in enumerate(cnts_chip):
	# draw the contour and show it
    #print c
    area = cv2.contourArea(c)
    #print "area = %f"%area
    area_chip.Fill(area)
    x,y,w,h = cv2.boundingRect(c)
    aspect_ratio = float(w)/h
    hull = cv2.convexHull(c)
    hull_area = cv2.contourArea(hull)
    if hull_area!=0 :
        solidity = float(area)/hull_area
    else :
        solidity =0
    if area>minArea_chip and area <maxArea_chip and (aspect_ratio<aspect_ratio_max_chip and aspect_ratio>aspect_ratio_min_chip) and solidity>solidity_min_chip : 
        new_cnts_chip.append(c)

new_cnts_sub = []

for i,c in enumerate(cnts_sub):
	# draw the contour and show it
    #print c
    area = cv2.contourArea(c)
    area_sub.Fill(area)
    x,y,w,h = cv2.boundingRect(c)
    aspect_ratio = float(w)/h
    #print "area = %f"%area
    if area>minArea_sub and area <maxArea_sub and (aspect_ratio<aspect_ratio_max_sub and aspect_ratio>aspect_ratio_min_sub) : 
        new_cnts_sub.append(c)


# Keep only good contours
cnts_sub=new_cnts_sub
cnts_chip=new_cnts_chip

# List to be filled with contour matching between substrate and chip

matched_sub = [0 for i in range(len(cnts_sub))]
matched_chip =  [0 for i in range(len(cnts_chip))]

# Maximum distance between contours circular enveloppe to be considered
dist_max = 10
        

for i,c in enumerate(cnts_chip):
    (x,y),radius = cv2.minEnclosingCircle(c)
    center = (int(x),int(y))    
    for j,c2 in enumerate(cnts_sub):
        (x2,y2),radius = cv2.minEnclosingCircle(c2)
        center2 = (int(x2),int(y2))
        dist = sqrt((x2-x)*(x2-x)+(y2-y)*(y2-y))
        #print dist
        if dist< dist_max :
            matched_chip[i]=j
            matched_sub[j] =i
            print "match! %i %i %f "%(i,j,dist)


# loop over the contours to draw the results
for i,c in enumerate(cnts_chip):
	# draw the contour and show it
    #print c
    area = cv2.contourArea(c)
    #print "area = %f"%area
    area_chip.Fill(area)
    x,y,w,h = cv2.boundingRect(c)
    aspect_ratio = float(w)/h
    hull = cv2.convexHull(c)
    hull_area = cv2.contourArea(hull)
    if hull_area!=0 :
        solidity = float(area)/hull_area
    else :
        solidity =0
    if area>minArea_chip and area <maxArea_chip and (aspect_ratio<1.8 and aspect_ratio>0.3) and solidity>0.5 : 
        (x,y),radius = cv2.minEnclosingCircle(c)
        center = (int(x),int(y))
        centers_chip.append(center)
        radius = int(radius)
        if matched_chip[i]>0 :
            cv2.circle(image_sub,center,radius,(0,255,0),1)
            cv2.drawContours(image_sub, [c], -1, (0, 255, 0), 1)
            cv2.circle(image_chip,center,radius,(0,255,0),1)
            cv2.drawContours(image_chip, [c], -1, (0, 255, 0), 1)
                         
# loop over the contours
for i,c in enumerate(cnts_sub):
	# draw the contour and show it
    #print c
    area = cv2.contourArea(c)
    area_sub.Fill(area)
    x,y,w,h = cv2.boundingRect(c)
    aspect_ratio = float(w)/h
    #print "area = %f"%area
    if area>minArea_sub and area <maxArea_sub and (aspect_ratio<1.5 and aspect_ratio>0.5) : 
        (x,y),radius = cv2.minEnclosingCircle(c)
        center = (int(x),int(y))
        centers_sub.append(center)
        radius = int(radius)
        if matched_sub[i]>0 :
            cv2.circle(image_sub,center,radius,(255,0,0),1)
            cv2.drawContours(image_sub, [c], -1, (255, 0, 0), 1)
            cv2.circle(image_chip,center,radius,(0,255,0),1)
            cv2.drawContours(image_chip, [c], -1, (0, 255, 0), 1)   

cv2.imshow("image_sub", image_sub)
cv2.waitKey(5000)
cv2.imwrite('pixel fitted_substrate.png',image_sub)

cv2.imshow("image_chip", image_chip)
cv2.waitKey(5000)
cv2.imwrite('pixel fitted_chip.png',image_chip)

#plotting of the result
can = TCanvas()
area_chip.Draw()
area_sub.Draw("same")
area_sub.SetLineColor(3)


can2 = TCanvas()
histo_dx = TH1D("h","hx",40,-10*pixel_to_um,10*pixel_to_um)
histo_dy = TH1D("h2","hy",40,-10*pixel_to_um,10*pixel_to_um)

#Calculate offsets between matched contours
for i,c in enumerate(cnts_sub) :
    for j,c2 in enumerate(cnts_chip) :
        if(matched_sub[i]==j):
            (x,y),radius = cv2.minEnclosingCircle(c)
            (x2,y2),radius = cv2.minEnclosingCircle(c2)
            dx = x2-x
            dy = y2-y
            histo_dx.Fill(dx*pixel_to_um)
            histo_dy.Fill(dy*pixel_to_um)

#Fitting and drawing
            
histo_dx.Fit('gaus')
histo_dy.Fit('gaus')

histo_dx.Draw()
histo_dy.Draw("SAMES")

histo_dy.SetLineColor(3)
        

