#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Date    : 2019-02-22 13:40:29
# @Author  : Your Name (you@example.org)
# @Link    : http://example.org
# @Version : $Id$

import os
import cv2
import numpy as np
from matplotlib import pyplot as plt
from skimage import measure


def find_file(filedir):
    folders = os.listdir(filedir)
    for name in folders:
        curname = os.path.join(filedir, name)
        if os.path.isfile(curname):
            # 文件名中满足的条件，可以在后面使用and filedir.find(dir_tag)来满足文件目录的要求
            if name.find('.jpg') > 0:  # and name.find(file_tag2) < 0
                filename_list.append(curname)
        else:
            find_file(curname)


def hsv_mask(image, lower_color, upper_color, kernel_size):
    lower = np.array(lower_color, np.uint8)
    upper = np.array(upper_color, np.uint8)
    # 得到二值图像，在lower~upper范围内的值为255，不在的值为0
    mask = cv2.inRange(image, lower, upper)
    # 进行腐蚀和膨胀
    if kernel_size!=0:
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, kernel_size)
        dilated = cv2.dilate(mask, kernel)
        eroded = cv2.erode(dilated, kernel)
        return eroded,mask
    else:
        return mask,mask


def getpos(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        print(HSV[y, x])
# th2=cv2.adaptiveThreshold(imagegray,255,cv2.ADAPTIVE_THRESH_MEAN_C,cv2.THRESH_BINARY,11,2)
filedir = '/home/aaron/slambook/slambook-master/iphone8/splitvideo/build/clear/train'
filename_list = []
find_file(filedir)
for item in filename_list[1:10]:
	image = cv2.imread(item)
	# image = cv2.GaussianBlur(image, (5, 5), 0)
	# image = cv2.resize(image, (700, 500), interpolation=cv2.INTER_LINEAR);
	HSV = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
	lower = np.array([0, 0, 230])
	upper = np.array([150, 150, 255])

	#
	kernel_size = (25, 25)
	img,mask = hsv_mask(HSV, lower, upper,kernel_size)
	res=cv2.bitwise_and(image,image,mask=mask)
	cv2.imshow('image',image)
	# find liantong
	labels = measure.label(img, connectivity=2)
	pro = measure.regionprops(labels)
    # 画矩形框
	fig, ax = plt.subplots()
	# im = img[:, :, (2, 1, 0)]
	ax.imshow(res)
	for region in pro:
		if region.area>1.0/207.0*res.shape[0]*res.shape[1] :
			box = region.bbox 
			ax.add_patch(plt.Rectangle((box[1], box[0]),box[3] - box[1],box[2] - box[0], fill=False,edgecolor='red', linewidth=2))
			# ax.text(box[1], box[0],region.area,color = "r")
	# 去除坐标
	plt.axis('off')

    # dpi是设置清晰度的，大于300就很清晰了
	plt.savefig('result.png', dpi=300)
	newimg=cv2.imread('result.png')
	newimg = cv2.resize(newimg, (800, 600), interpolation=cv2.INTER_LINEAR);
	cv2.imshow('result',newimg)
	# mask=cv2.inRange(HSV,lower,upper)
	# res=cv2.bitwise_and(img,img,mask=mask)


	# # cv2.imshow("mask",mask)
	# cv2.imshow('res',res)
	
	
	# cv2.setMouseCallback("image",getpos)
	cv2.waitKey(0)
	cv2.destroyAllWindows()
