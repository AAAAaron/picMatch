#include "commonFun.h"
#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
using namespace std;
int main ( int argc, char** argv )
{
    string testdir="/aaron/slambook/slambook-master/picmatch/test/alltest/";
    
    for ( int i=0; i<24; i++ )
    {

    string path = testdir+to_string(i)+".jpg";
    Mat img=imread(path);
    

    double DR=DefRto(img);
    string baseResult="清晰度";
    if(DR<7)
    {
      baseResult+="模糊";
    }
    else{
      if(DR>14)
      {
	baseResult+="清楚";
      }
      else{
	baseResult+="一般";
      }
    }
    float cast, da, db;
    colorException(img,cast,da,db);
    baseResult+="色差";
    baseResult+=to_string(cast);
    // if(cast<1)
    // {
    //   baseResult+="正常";
    // }
    // else{
    //   baseResult+="不正常";
    // }
    brightnessException(img,cast,da);
    baseResult+="亮度";
    if(cast<1)
    {
      baseResult+="正常";
    }
    else
    {
      if(da>0)
      {
	baseResult+="过亮";
      }
      else{
	baseResult+="过暗";
      }
    }
    resize(img,img,Size(1000,600));
    imshow(baseResult+to_string(DR),img);
    waitKey(0);
    destroyAllWindows();
    }
  return 0;
}