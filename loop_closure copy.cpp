#include "DBoW3/DBoW3.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>
#include <string>

#include <time.h>
#include "vfc.h"
#include <fstream> 
#include "tic_toc.h"
#include<stdio.h>
#include<stdlib.h>
#define random(x) (rand()%x)
using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;
double sigmoidFun(double x,double sigma,double miu)
{
  return 1.0/(1.0+exp(-(x-miu)*sigma));
}
void mergeImage(Mat &dst, vector<Mat> &images)
{
	int imgCount = (int)images.size();
 
	if (imgCount <= 0)
	{
		printf("the number of images is too small\n");
		return;
	}
 
	printf("imgCount = %d\n", imgCount);
 
	/*将每个图片缩小为指定大小*/
	int rows = 600;
	int cols = 900;
	vector<Mat> tmpImgs;
	for (int i = 0; i < imgCount; i++)
	{	
	      Mat tmpImg(images[i]);
// 	      imshow("1111",images[i]);
	      resize(images[i], tmpImg, Size(cols, rows));//注意区别：Size函数的两个参数分别为：宽和高，宽对应cols，高对应rows
	      imshow("dsd",tmpImg);
	      tmpImgs.push_back(tmpImg);
	}
	
// 	waitKey(0); 
	/*创建新图片的尺寸
		高：rows * imgCount/2
		宽：cols * 2
	*/
	dst.create(rows * imgCount / 2, cols * 2, CV_8UC3);
 
	for (int i = 0; i < imgCount; i++)
	{
		tmpImgs[i].copyTo(dst(Rect((i % 2) * cols, (i / 2)*rows, images[0].cols, images[0].rows)));
	}
}
/***************************************************
 * 本节演示了如何根据前面训练的字典计算相似性评分
 *  2.13 增加了可以使用findFundamentalMat去除误匹配
 * TODO 后面增加在选出的几幅图中根据匹配的效果选择最好的那张
 * ************************************************/
int main( int argc, char** argv )
{
    
    // read the images and database  
    cout<<"reading database"<<endl;
    DBoW3::Vocabulary vocab("./vocabulary3.yml.gz");
    // DBoW3::Vocabulary vocab("./vocabularyRetail.yml.gz");//指定自己的图片字典
//     DBoW3::Vocabulary vocab("./vocab_larger.yml.gz");  // use large vocab if you want: 
    if ( vocab.empty() )
    {
        cerr<<"Vocabulary does not exist."<<endl;
        return 1;
    }
    cout<<"reading images... "<<endl;
    vector<Mat> images; 
    RNG rng((unsigned)time(NULL));
    // string testdir="/aaron/slambook/slambook-master/iphone8/splitvideo/build/clear/test/";
	string testdir="/aaron/slambook/slambook-master/picmatch/test/sbk/";
    for ( int i=0; i<24; i++ )
    {
        // string path = testdir+to_string(random(2000))+".jpg";
		string path = testdir+to_string(i)+".jpg";
// 	imshow("dsd",imread(path));
// 	waitKey(0);
        images.push_back( imread(path) );
	cout<<images.back().size()<<endl;
       
    }
	cout<<"read test over!"<<endl;
    TicToc mytic;
    vector<Mat*> trainimages;     
    // string traindir="/aaron/slambook/slambook-master/iphone8/splitvideo/build/clear/train/";
	string traindir="/aaron/slambook/slambook-master/picmatch/test/alltest/";
    Mat *tmpimg;
    for ( int i=0; i<64; i++ )
    {
        // string path = traindir+to_string(random(1000))+".jpg";
		string path = traindir+to_string(i)+".jpg";
// 	imshow("dsd",imread(path));
// 	waitKey(0);
	tmpimg= new Mat();
	*tmpimg=imread(path);
	if(!tmpimg->data)
	{
        printf("No data!--Exiting the program \n");
        continue;
	}
        trainimages.push_back(tmpimg);

// 	cout<<trainimages.back().size()<<endl;
       
    }  
    cout<<"train img read time spend "<<mytic.toc()<<"ms"<<endl;
    mytic.tic();
    // NOTE: in this case we are comparing images with a vocabulary generated by themselves, this may leed to overfitting.  
    // detect ORB features
    cout<<"detecting orb features ... "<<endl;
    
//     int minHessian = 3000;

//     Ptr<SURF>   detector(minHessian);    
//     Ptr<SurfFeatureDetector> detector = SurfFeatureDetector::create(minHessian);
    Ptr< Feature2D > detector = ORB::create();
    vector<Mat> descriptors;
    vector<vector<KeyPoint>> keypoints;
    int img_index=0;
    for ( Mat& image:images )
    {        
        Mat descriptor;
	vector<KeyPoint> tmpkeypoints;
        detector->detectAndCompute( image, Mat(), tmpkeypoints, descriptor );
// 	cout<<"sdsd"<<endl;
        descriptors.push_back( descriptor );
	drawKeypoints(image,tmpkeypoints,image,Scalar::all(-1),DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	keypoints.push_back(tmpkeypoints);
	img_index++;
    }

    
    cout<<"detect test time time spend "<<mytic.toc()<<"ms"<<endl;
    mytic.tic();
    vector<Mat*> trainDescriptors;
    vector<vector<KeyPoint>*> trainkeypoints;

    
    img_index=0;
    ofstream out("out2.txt"); 
    Mat* descriptor; 
    vector<KeyPoint>* tmpkeypoints;
    for(unsigned int trainIndex=0;trainIndex<trainimages.size();trainIndex++)
    {
        
	descriptor = new Mat();
	tmpkeypoints= new vector<KeyPoint>[1000];
// 	vector<KeyPoint> tmpkeypoints;
	Mat imgTrain=*(trainimages[trainIndex]);
	detector->detectAndCompute( imgTrain, Mat(), *tmpkeypoints, *descriptor );
	trainDescriptors.push_back( descriptor );
	drawKeypoints(imgTrain,*tmpkeypoints,imgTrain,Scalar::all(-1),DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	trainkeypoints.push_back(tmpkeypoints);
    }
   
    
    cout<<"detect train time time spend "<<mytic.toc()<<"ms"<<endl;
    mytic.tic();    
    
//     for ( Mat& image:trainimages )
//     {
//         vector<KeyPoint> tmpkeypoints;
//         Mat descriptor;
//         detector->detectAndCompute( image, Mat(), tmpkeypoints, descriptor );
//         trainDescriptors.push_back( descriptor );
// 	drawKeypoints(image,tmpkeypoints,image,Scalar::all(-1),DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
// 	trainkeypoints.push_back(tmpkeypoints);
// 	img_index++;
//     }
    
    // we can compare the images directly or we can compare one image to a database 
    // images :
//     cout<<"comparing images with images "<<endl;
//     for ( int i=0; i<images.size(); i++ )
//     {
//         DBoW3::BowVector v1;
//         vocab.transform( descriptors[i], v1 );
//         for ( int j=i; j<images.size(); j++ )
//         {
//             DBoW3::BowVector v2;
//             vocab.transform( descriptors[j], v2 );
//             double score = vocab.score(v1, v2);
//             cout<<"image "<<i<<" vs image "<<j<<" : "<<score<<endl;
//         }
//         cout<<endl;
//     }
    
    
    
    vector<DMatch> matches;
//     FlannBasedMatcher matcher(new cv::flann::LinearIndexParams(),new cv::flann::SearchParams());
//     BFMatcher matcher;
    Ptr<DescriptorMatcher> matcher  = DescriptorMatcher::create ( "BruteForce-Hamming" );

//     Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
    // or with database z
    cout<<"comparing images with database "<<endl;
    mytic.tic();
    
    DBoW3::Database* db;
    db = new DBoW3::Database( vocab, false, 0);
    for ( int i=0; i<trainDescriptors.size(); i++ )
        db->add(*trainDescriptors[i]);
    
    
    cout<<"db add descriptor  time spend "<<mytic.toc()<<"ms"<<endl;
    mytic.tic();

    cout<<"database info: "<<*db<<endl;
//     out<<"--------------"<<endl;
    for ( int i=0; i<descriptors.size(); i++ )
    {
//         string path = testdir+to_string(i)+".jpg";
//       	vector<Mat> imgt;
	Mat dst;
// 	imgt.push_back(images[i]);  
// 	imshow("",imread(path));

    mytic.tic();
        DBoW3::QueryResults ret;
        db->query( descriptors[i], ret, 3);      // max result=4
	
    cout<<"db->query  time spend "<<mytic.toc()<<"ms"<<endl;
    mytic.tic();
//         cout<<"searching for image "<<i<<" returns "<<ret<<endl<<endl;
// 	if(ret[0].Score<0.035)
// 	{
// 	  out<<i<<","<<0<<","<<0<<","<<0<<","<<0<<","<<"该张照片得分过低,库中没有有效的照片"<<endl;
// 	  continue;
// 	}
	double firstScore=0;
	double firstInlineCount=1e-2;
	double firstConsistency=1e-2;
	double resCount=-1;
	
	for(DBoW3::Result res:ret)
	{
	  resCount+=1;

	

	matcher->match(descriptors[i],*(trainDescriptors[res.Id]),matches);


	//-- Step 4: Remove mismatches by vector field consensus (VFC)
	// preprocess data format
// 	vector<Point2f> X;
// 	vector<Point2f> Y;
// 	X.clear();
// 	Y.clear();
// 	for (unsigned int j = 0; j < matches.size(); j++) {
// 		int idx1 = matches[j].queryIdx;
// 		int idx2 = matches[j].trainIdx;
// 		X.push_back(keypoints[i][idx1].pt);
// 		Y.push_back(trainkeypoints[ret[0].Id][idx2].pt);
// 	}
// 	// main process
// 	double t = (double)getTickCount();
// 
// 	VFC myvfc;
// 	myvfc.setData(X, Y);
// 	myvfc.optimize();
// 	vector<int> matchIdx = myvfc.obtainCorrectMatch();
// 	t = 1000 * ((double)getTickCount() - t) / getTickFrequency();
// 	cout << "Times (ms): " << t << endl;
// 
// 	// postprocess data format
// 	std::vector< DMatch > correctMatches;
// 	std::vector<KeyPoint> correctKeypoints_1, correctKeypoints_2;
// 	correctMatches.clear();
// 	for (unsigned int j = 0; j < matchIdx.size(); j++) {
// 		int idx = matchIdx[j];
// 		correctMatches.push_back(matches[idx]);
// 		correctKeypoints_1.push_back(keypoints[i][idx]);
// 		correctKeypoints_2.push_back(trainkeypoints[ret[0].Id][idx]);
// 	}
	//看下矫正的match里能不能去掉一部分
	
/*	std::vector< DMatch > good_matches,goodchose_matches;
	for ( int j = 0; j < matches.size(); j++ )
	{
	    
	    if ( matches[j].distance <= 200.0)
	    {
		good_matches.push_back ( matches[j] );
		printf("the %d th distance is %.3f \n",j,matches[j].distance);
	    }
	}*/	
//     //-- 第四步:匹配点对筛选
//     double min_dist=10000, max_dist=0;
// 
//     //找出所有匹配之间的最小距离和最大距离, 即是最相似的和最不相似的两组点之间的距离
//     for ( int i = 0; i < matches.size(); i++ )
//     {
//         double dist = matches[i].distance;
//         if ( dist < min_dist ) min_dist = dist;
//         if ( dist > max_dist ) max_dist = dist;
//     }
//     

    //当描述子之间的距离大于两倍的最小距离时,即认为匹配有误.但有时候最小距离会非常小,设置一个经验值30作为下限.
    std::vector< DMatch > good_matches;

    
//  使用findfundermantael 去除野点
      // 分配空间

      int ptCount = (int)matches.size();
      Mat p1(ptCount, 2, CV_32F);
      Mat p2(ptCount, 2, CV_32F);
	
      // 把Keypoint转换为Mat

      Point2f pt;

      vector<KeyPoint> tmpkp=*(trainkeypoints[res.Id]);

      for (int jk=0; jk<ptCount; jk++)
      {
	  pt = keypoints[i][matches[jk].queryIdx].pt;
	  p1.at<float>(jk, 0) = pt.x;
	  p1.at<float>(jk, 1) = pt.y;
	  pt = tmpkp[matches[jk].trainIdx].pt;
	  p2.at<float>(jk, 0) = pt.x;
	  p2.at<float>(jk, 1) = pt.y;
      }
      // 用RANSAC方法计算F

      Mat m_Fundamental;

      // 上面这个变量是基本矩阵

      vector<uchar> m_RANSACStatus;

      // 上面这个变量已经定义过，用于存储RANSAC后每个点的状态

      m_Fundamental = findFundamentalMat(p1, p2, m_RANSACStatus, FM_RANSAC);
      // 计算野点个数

      int OutlinerCount = 0;
      for (int jk=0; jk<ptCount; jk++)
      {
	  if (m_RANSACStatus[jk] == 0) // 状态为0表示野点
	  {
		OutlinerCount++;
	  }
      }
      // 计算内点

      vector<Point2f> m_LeftInlier;
      vector<Point2f> m_RightInlier;
//       vector<DMatch> good_matches;

      // 上面三个变量用于保存内点和匹配关系

      int InlinerCount = ptCount - OutlinerCount;
      good_matches.resize(InlinerCount);
      m_LeftInlier.resize(InlinerCount);
      m_RightInlier.resize(InlinerCount);
      InlinerCount = 0;
      Point2d consistency_mean(0,0);

      vector <Point2d> consistency_vector;
      for (int jk=0; jk<ptCount; jk++)
      {
	  if (m_RANSACStatus[jk] != 0)
	  {
		m_LeftInlier[InlinerCount].x = p1.at<float>(jk, 0);
		m_LeftInlier[InlinerCount].y = p1.at<float>(jk, 1);
		m_RightInlier[InlinerCount].x = p2.at<float>(jk, 0);
		m_RightInlier[InlinerCount].y = p2.at<float>(jk, 1);
		good_matches[InlinerCount].queryIdx = InlinerCount;
		good_matches[InlinerCount].trainIdx = InlinerCount;
		double dx=m_RightInlier[InlinerCount].x-m_LeftInlier[InlinerCount].x;
		double dy=m_RightInlier[InlinerCount].y-m_LeftInlier[InlinerCount].y;
		double dxy=sqrt(dx*dx+dy*dy);
		if(dxy!=0)
		{
		dx/=dxy;
		dy/=dxy;
		}
		if(dx<0)
		{
		  dx*=-1;
		  dy*=-1;
		}
		consistency_vector.push_back(Point2d(dx,dy));
		consistency_mean.x+=dx;
		consistency_mean.y+=dy;
		// out<<i<<","<<res.Id<<","<<dx<<","<<dy<<endl;
		InlinerCount++;
	    
	  }
      }
      if(InlinerCount<25)
      {
	out<<i<<","<<0<<","<<0<<","<<0<<","<<0<<","<<"内点数量小于25,此时 匹配照片不成功"<<res.Id<<endl;
	continue;
      }
      
      
      
      double consistency_ratio=0;
      if (InlinerCount>2)
      {
	consistency_mean.x/=InlinerCount;
	consistency_mean.y/=InlinerCount;
	double consistency_norm=sqrt(consistency_mean.x*consistency_mean.x+consistency_mean.y*consistency_mean.y);
	consistency_mean.x/=consistency_norm;
	consistency_mean.y/=consistency_norm;
	double consistency_min=2;
	double consistency_max=-2;
	double consistency_sum=0;
	for(Point2d tmpPoint:consistency_vector)
	{
	  double cosValue=tmpPoint.x*consistency_mean.x+tmpPoint.y*consistency_mean.y;
	  cosValue=abs(cosValue);
	  if(cosValue>consistency_max)
	  {
	    consistency_max=cosValue;
	  }
	  if(cosValue<consistency_min)
	  {
	    consistency_min=cosValue;
	  }
	  consistency_sum+=cosValue;
	}
	consistency_ratio=consistency_sum-consistency_min-consistency_min;
	consistency_ratio=consistency_ratio*1.0f/(InlinerCount-2);
	
      
      }
      
      // 把内点转换为drawMatches可以使用的格式

      vector<KeyPoint> key1(InlinerCount);
      vector<KeyPoint> key2(InlinerCount);
      KeyPoint::convert(m_LeftInlier, key1);
      KeyPoint::convert(m_RightInlier, key2);
      // 显示计算F过后的内点匹配

      // Mat m_matLeftImage;
      // Mat m_matRightImage;
      // 以上两个变量保存的是左右两幅图像
//       Mat OutImage;
    
	    //	
      
// 	Mat Mrands,mask,srcpt,dstpt;
// 	vector<uchar> mask2;
// 	for(DMatch& m:good_matches)
// 	{
// 	  srcpt.push_back(keypoints[i][m.queryIdx].pt);
// 	  dstpt.push_back(trainkeypoints[i][m.trainIdx].pt);
// 	}
// 	Mrands=cv::findHomography(srcpt,dstpt,cv::RANSAC,3,mask2);
// 	for(int j = 0; j < good_matches.size(); j++)
// 	{
// 	  if(mask2[j]>0)
// 	  {
// 	    goodchose_matches.push_back(good_matches[j]);
// 	  }
// 	  else
// 	  {
// 	    cout<<j<<"jjj"<<endl;
// 	  }
// 	}
       if(resCount==0)
       {
	 firstScore=res.Score;
	 firstInlineCount=InlinerCount;
	 firstConsistency=consistency_ratio;
       }
       if(resCount==0 || (res.Score/firstScore>0.8&&((InlinerCount/firstInlineCount>1.8&&consistency_ratio>0.5)||(InlinerCount/firstInlineCount>1.3&&consistency_ratio/firstConsistency>1))))
       {
      
	//-- Draw mismatch removal result
 	Mat img_correctMatches;
	
 	drawMatches(images[i], key1, *trainimages[res.Id], key2, good_matches, img_correctMatches);  
// 	drawMatches(images[i], keypoints[i], trainimages[ret[0].Id], trainkeypoints[ret[0].Id] ,good_matches, img_correctMatches);
	
	resize(img_correctMatches,img_correctMatches,Size(1000,600));
	//-- Show mismatch removal result
	double lastResult=0;
	double pro1=sigmoidFun(res.Score,100,0.05);
	double pro2=sigmoidFun(InlinerCount,0.15,40);
	double pro3=sigmoidFun(consistency_ratio,20,0.7);
	lastResult=(pro1+pro2+pro3)/3.0f;
	// if(lastResult<0.5)
	// {
	//   continue;
	// }
	string tofile="./result/Score="+to_string(res.Score)+"-"+to_string(pro1)+"InlinerCount="+to_string(InlinerCount)+"-"+to_string(pro2)+"consistency="+to_string(consistency_ratio)+"-"+to_string(pro3)+"pro"+to_string(lastResult)+".jpg";
// 	cout<<tofile<<endl;
	imwrite(tofile,img_correctMatches);
	// imshow("Score="+to_string(res.Score)+"/"+to_string(pro1)+"InlinerCount="+to_string(InlinerCount)+"/"+to_string(pro2)+"consistency="+to_string(consistency_ratio)+"/"+to_string(pro3)+"pro"+to_string(lastResult), img_correctMatches);
	// waitKey(0);
	}
	out<<i<<","<<res.Id<<","<<res.Score<<","<<InlinerCount<<","<<consistency_ratio<<endl;
// 	Mat img_goodmatch;
// 	drawMatches ( images[i], keypoints[i], trainimages[ret[0].Id], trainkeypoints[ret[0].Id], good_matches, img_goodmatch ,Scalar::all(-1),Scalar::all(-1));
// 
// 	resize(img_goodmatch, img_goodmatch, Size(1800, 900));
// 	imshow ( "匹配成功:优化后匹配点对"+to_string(good_matches.size()), img_goodmatch );

// 	mergeImage(dst, imgt);
// 	imshow("dst", dst);
       
	}
//  	cv::waitKey(0);
 	// cv::destroyAllWindows();
    }
    delete tmpimg;
//     delete tmpkeypoints;
    delete descriptor;
    
    out.close();
    cout<<"done."<<endl;
}