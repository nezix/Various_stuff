#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <time.h>
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;
using namespace cv;

vector<Vec3b> get_values(Mat &mymat);
vector<uchar> get_spec_values(vector<Vec3b> &myvec,int id);

void usage(int argc,char **argv){
  if(argc != 2){
    cout << "Usage : "<<argv[0]<<" namefile"<<endl;
    exit(-1);
  }
}


Point2i coord_circle;
int size_circle;
bool drag_start = false;
bool resize_start = false;
int save_y = 0;
int thickness = 3;
Mat imgori;
Scalar cur_color;

void onmouse(int event, int x, int y, int flags, void* userdata) {

    if (event == EVENT_LBUTTONDOWN )
        drag_start = true;
    else if (event == EVENT_LBUTTONUP )
        drag_start = false;
    if (event == EVENT_RBUTTONDOWN ){
        resize_start = true;
        save_y = y;
    }
    else if (event == EVENT_RBUTTONUP )
        resize_start = false;
    if (drag_start){
        coord_circle = Point2i(x,y);
        //Mat img = Mat(imgori);
        Mat img = imgori.clone();
        circle( img, coord_circle, size_circle,cur_color, thickness );
        imshow("Image", img);
    }
    if (resize_start){
        size_circle = max(size_circle + (save_y - y),10);
        Mat img = imgori.clone();//Mat(imgori);
        circle( img, coord_circle, size_circle,cur_color, thickness );
        imshow("Image", img);
        save_y = y;
    }
}

float percentile(vector<uchar> &values,float percent){
    sort(values.begin(),values.end());
    float k = (values.size()-1)*percent;
    int f = floor(k);
    int c = ceil(k);
    if (f == c)
        return values[k];

    float d0 = values[f] * (c-k);
    float d1 = values[c] * (k-f);
    return d0+d1;
}
vector<uchar> remove_outliers(vector<uchar> &values){
    vector<uchar> ret;
    float q75 = percentile(values,0.75);
    float q30 = percentile(values,0.30);
    float H = 1.5 * (q75 - q30);
    for(int i=0;i<values.size();i++){
        if(values[i] >= (q30-H) && values[i] <= (q75+H))
            ret.push_back(values[i]);
    }

    return ret;
}

uchar mymin(vector<uchar> &myvec){
    uchar ret;
    if(myvec.size()!=0){
        ret = myvec[0];
        for(int i=1;i<myvec.size();i++)
            ret = min(myvec[i],ret);
    }
    return ret;
}
uchar mymax(vector<uchar> &myvec){
    uchar ret ;
    if(myvec.size()!=0){
        ret = myvec[0];
        for(int i=1;i<myvec.size();i++)
            ret = max(myvec[i],ret);
    }
    return ret;
}
int main(int argc, char** argv) {
    usage(argc,argv);
    setUseOptimized(true);

    Mat capture;
    capture = imread(argv[1]);

    if(!capture.data){
        cerr << "Image not loaded"<<endl;
        exit(0);
    }
    Mat frame;
    size_circle = 50;
    coord_circle = Point2i(1920/2,1080/2);
    cur_color = Scalar(0,255,255);
    resize(capture, frame, Size(1920, 1080));

    imgori = frame.clone();//Mat(frame);

    imshow("Image",frame);
    setMouseCallback("Image", onmouse, NULL);

    waitKey(0);

    Mat hsv_fullimg;
    cvtColor(frame,hsv_fullimg,CV_BGR2HSV);

    Mat mask_get = Mat(frame.size(), CV_8U);
    mask_get.setTo(0);

    circle( mask_get, coord_circle, size_circle*0.7f,(unsigned char)1, -1);
    Mat img_get;

    bitwise_and(imgori,imgori,img_get,mask_get);

    Mat hsv_img;
    cvtColor(img_get,hsv_img,CV_BGR2HSV);

    vector<Vec3b> hsv_values = get_values(hsv_img);

    vector<uchar> h_values = get_spec_values(hsv_values,0);
    vector<uchar> s_values = get_spec_values(hsv_values,1);
    vector<uchar> v_values = get_spec_values(hsv_values,2);

    vector<uchar> clean_h_values = remove_outliers(h_values);
    vector<uchar> clean_s_values = remove_outliers(s_values);
    vector<uchar> clean_v_values = remove_outliers(v_values);

    Scalar min_hsv = Scalar(mymin(clean_h_values),mymin(clean_s_values),mymin(clean_v_values));
    Scalar max_hsv = Scalar(mymax(clean_h_values),mymax(clean_s_values),mymax(clean_v_values));
    cout << min_hsv << "  /  "<<max_hsv<<endl;

    Mat filtered;
    inRange(hsv_fullimg,min_hsv,max_hsv,filtered);

    imshow("filtered",filtered);
    waitKey(0);

}

vector<Vec3b> get_values(Mat &mymat){

    vector<Vec3b> ret;
    Vec3b val;
    for (int i = 0; i < mymat.rows; ++i) {
        Vec3b* pixel = mymat.ptr<Vec3b>(i); // point to first pixel in row
        for (int j = 0; j < mymat.cols; ++j) {
            val = pixel[j];
            if (val[0] != 0 || val[1]!=0 || val[2]!=0){
                ret.push_back(val);
            }
        }
    }
    return ret;
}
vector<uchar> get_spec_values(vector<Vec3b> &myvec,int id){
    vector<uchar> ret;
    for(int i=0;i<myvec.size();i++){
        ret.push_back(myvec[i][id]);
    }
    return ret;
}
