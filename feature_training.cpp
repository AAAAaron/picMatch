#include "DBoW3/DBoW3.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "tic_toc.h"
using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;
/***************************************************
 * 本节演示了如何根据data/目录下的十张图训练字典
 * ************************************************/

int main( int argc, char** argv )
{
    TicToc tr;
    // read the image 
    cout<<"reading images... "<<endl;
    vector<Mat> images; 
    for ( int i=0; i<441; i++ )
    {
        string path = "/aaron/slambook/slambook-master/picmatch/alldata/"+to_string(i)+".jpg";
        
	// string path ="/home/aaron/slambook/slambook-master/iphone8/splitvideo/build/clear/train/"+to_string(i)+".jpg";
        images.push_back( imread(path) );
	cout<<"now read img "<<path<<endl;
    }
    // detect ORB features
    cout<<"read time spend "<<tr.toc()<<"ms"<<endl;
    cout<<"detecting ORB features ... "<<endl;
//     int minHessian = 8000;
//     Ptr<SurfFeatureDetector> detector = SurfFeatureDetector::create(minHessian);
    Ptr < Feature2D > detector = ORB::create();
    vector<Mat> descriptors;
    for ( Mat& image:images )
    {
        vector<KeyPoint> keypoints; 
        Mat descriptor;
        detector->detectAndCompute( image, Mat(), keypoints, descriptor );
        descriptors.push_back( descriptor );
    }
    cout<<"detect orb features spend "<<tr.toc()<<"ms"<<endl;
    // create vocabulary 
    cout<<"creating vocabulary ... "<<endl;
    DBoW3::Vocabulary vocab;
    vocab.create( descriptors );
    cout<<"vocabulary info: "<<vocab<<endl;
    // vocab.save( "./vocabularyRetail.yml.gz" );
    vocab.save( "./vocabulary1.yml.gz" );
    cout<<"detect orb features spend "<<tr.toc()<<"ms"<<endl;
    cout<<"done"<<endl;
    
    return 0;
}