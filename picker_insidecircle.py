#! /usr/bin/env python2
 
import cv2
import numpy as np
import sys

if len(sys.argv) !=2:
	print "Usage : "+sys.argv[0]+" imagefile.jpg"
	exit(0)

drag_start = False
resize_start = False
save_y = 0
thickness = 3


def onmouse(event, x, y, flags, param):
    global drag_start,resize_start,save_y,img,coord_circle,size_circle
    if event == cv2.EVENT_LBUTTONDOWN:
        drag_start = True
    elif event == cv2.EVENT_LBUTTONUP:
        drag_start = False
    if event == cv2.EVENT_RBUTTONDOWN:
        resize_start = True
        save_y = y
    elif event == cv2.EVENT_RBUTTONUP:
        resize_start = False
    if drag_start:
        coord_circle = (x,y)
        img = imgori.copy()
        cv2.circle(img,coord_circle,size_circle,255,thickness)
        cv2.imshow('dst_rt', img)
    if resize_start:
        size_circle = max(size_circle + (save_y - y),10)
        img = imgori.copy()
        cv2.circle(img,coord_circle,size_circle,255,thickness)
        cv2.imshow('dst_rt', img)
        save_y = y


imgori = cv2.imread(sys.argv[1])
img = imgori.copy()

screen_res = 1280, 720
scale_width = screen_res[0] / img.shape[1]
scale_height = screen_res[1] / img.shape[0]
scale = min(scale_width, scale_height)
window_width = int(img.shape[1] * scale)
window_height = int(img.shape[0] * scale)

coord_circle = (screen_res[0]/2,screen_res[1]/2)
size_circle = screen_res[0]/20
cv2.circle(img,coord_circle,size_circle,255,thickness)


cv2.namedWindow('dst_rt', cv2.WINDOW_NORMAL)
cv2.resizeWindow('dst_rt', window_width, window_height)
cv2.setMouseCallback("dst_rt", onmouse)

cv2.imshow('dst_rt', img)
cv2.waitKey(0)
cv2.destroyAllWindows()


#mask_get = np.zeros((screen_res[0],screen_res[1],1), np.uchar8)
mask_get = np.zeros(img.shape,np.uint8)
cv2.circle(mask_get,coord_circle,size_circle,(255, 255, 255),-1)

img_get = imgori.copy()
img_get = cv2.bitwise_and(img_get,mask_get)

cv2.imshow("montest",img_get)
cv2.waitKey(0)
