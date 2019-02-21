#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Date    : 2019-01-29 17:44:19
# @Author  : Your Name (you@example.org)
# @Link    : http://example.org
# @Version : $Id$

import os

def find_file(filedir):
    folders = os.listdir(filedir)
    for name in folders:
        curname = os.path.join(filedir,name)
        if os.path.isfile(curname):
            # 文件名中满足的条件，可以在后面使用and filedir.find(dir_tag)来满足文件目录的要求
            if  name.find('.jpg') > 0:#and name.find(file_tag2) < 0
                filename_list.append(curname)
        else:
            find_file(curname)

# filedir='/home/aaron/slambook/slambook-master/picmatch/alldata'
filedir ='/home/aaron/slambook/slambook-master/iphone8/splitvideo/build/clear/'
filename_list=[]
find_file(filedir)
traindir=os.path.join(filedir,'train')
testdir=os.path.join(filedir,'test')
isExists=os.path.exists(traindir)
if not isExists:
    os.mkdir(traindir)
    os.mkdir(testdir)
testcount=0
for index,item in enumerate(filename_list):
    filename=item.split("/")[-1]
    # filename=filename.split('.')[0]
    filename = filename[:-4]
    if index%3==0:
    	os.rename(item,os.path.join(traindir,str(index/3)+'.jpg'))
    else:
        os.rename(item,os.path.join(testdir,str(testcount)+'.jpg'))
        testcount=testcount+1