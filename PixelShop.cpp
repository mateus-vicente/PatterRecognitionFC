#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

#include "/Applications/root_v6.04.14/include/TH1D.h"
#include "TCanvas.h"

using namespace cv;
using namespace std;

/** Global Variables */
const int alpha_max = 200;
const int beta_max = 255;
int contrast_chip;	// Contrast
int brightness_chip;	//Brightness
int whatimage_chip;	//Image Display
int threshold_chip;	// Binary Threshold
int clarity_chip;	//Clarity = Midtones contrast

int contrast_sub;	// Contrast
int brightness_sub;	//Brightness
int whatimage_sub;	//Image Display
int threshold_sub;	// Binary Threshold
int clarity_sub;	//Clarity = Midtones contrast

int button;

int whatimage_patern;

//int const max_kernel_size = 21;

Scalar color = Scalar( 0, 255, 0 );

//Parameter for a cut on the area of the contours_chip
double minArea_sub = 2000;
double maxArea_sub = 3000;
double minArea_chip = 1150;
double maxArea_chip = 1800;

//Parameters for cut on solidity
double solidity_min_sub = 0;
double solidity_min_chip = 0.3;

//Parameter for cut on aspect ratio
double aspect_ratio_min_chip = 0.3;
double aspect_ratio_max_chip = 1.8;
double aspect_ratio_min_sub = 0.5;
double aspect_ratio_max_sub = 1.5;

double area;
double hull_area;
double aspect_ratio;
double solidity;

/** Matrices to store images */
Mat image_chip;
Mat image_chip2;
Mat image_sub;
Mat image_sub2;

vector<vector<Point> > contours_chip;
vector<vector<Point> > contours_sub;
vector<vector<Point> > contours_filtered_chip;
vector<vector<Point> > contours_filtered_sub;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void static on_trackbar_chip( int, void* ){
  Mat new_image = Mat::zeros( image_chip.size(), image_chip.type() );
  Mat blank_image = Mat::zeros( image_chip.size(), image_chip.type() );
  Mat gray_image = Mat::zeros( image_chip.size(), image_chip.type() );
  Mat filter_image = Mat::zeros( image_chip.size(), image_chip.type() );
  Mat morph_image = Mat::zeros( image_chip.size(), image_chip.type() );
  Mat morph2_image = Mat::zeros( image_chip.size(), image_chip.type() );
  Mat morph3_image = Mat::zeros( image_chip.size(), image_chip.type() );
  Mat contours_image = Mat::zeros( image_chip.size(), image_chip.type() );
  Mat overlap_image = Mat::zeros( image_chip.size(), image_chip.type() );
  Mat overlap_image2 = Mat::zeros( image_chip.size(), image_chip.type() );

  //vector<vector<Point> > contours_filtered_chip;
  vector<Vec4i> hierarchy;

  for( int y = 0; y < image_chip.rows; y++ )
    for( int x = 0; x < image_chip.cols; x++ )
      for( int c = 0; c < 3; c++ ){
        if (image_chip.at<Vec3b>(y,x)[c] > 80 && image_chip.at<Vec3b>(y,x)[c] < 174){
          new_image.at<Vec3b>(y,x)[c] =
	    saturate_cast<uchar>( ((image_chip.at<Vec3b>(y,x)[c] - 127 )*((double)contrast_chip/100) + 
				   (image_chip.at<Vec3b>(y,x)[c] - 127 )*((double)clarity_chip/100)) + 127 + brightness_chip );
	}
        else new_image.at<Vec3b>(y,x)[c] =
       saturate_cast<uchar>( (image_chip.at<Vec3b>(y,x)[c] - 127 )*((double)contrast_chip/100) + 127 + brightness_chip );
      }

  cvtColor(new_image, gray_image, COLOR_BGR2GRAY, 0);
  adaptiveThreshold(gray_image, filter_image, 255, ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, threshold_chip*2+1,0);

  Mat element = getStructuringElement(0, Size(1,1), Point(0,0) );

  morphologyEx(filter_image, morph_image, MORPH_CLOSE, element);
  morphologyEx(morph_image, morph2_image, MORPH_OPEN, element);

  morph3_image = morph2_image;
  findContours( morph3_image, contours_chip, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) );
  cout << "Chip Contours: " << contours_chip.size() << endl;

  vector<Rect> boundRect( contours_chip.size() );
  vector<vector<Point> >hull( contours_chip.size() );
  vector<Point2f> center(contours_chip.size());
  vector<float> radius(contours_chip.size());

  vector<Point2f> center2(contours_chip.size());
  vector<float> radius2(contours_chip.size());

  contours_filtered_chip = contours_chip;
  contours_filtered_chip.clear();

  overlap_image = 0 + image_chip;
  overlap_image2 = 0 + image_sub;

  for( unsigned int i = 0; i< contours_chip.size(); i++ ){
    area = contourArea(contours_chip[i]);
    boundRect[i] = boundingRect(contours_chip[i]);
    aspect_ratio = float(boundRect[i].width)/boundRect[i].height;
    convexHull( contours_chip[i], hull[i], false );
    hull_area = contourArea(hull[i]);
    if(hull_area!=0) solidity = float(area)/hull_area;
    else solidity =0;
    if (area>minArea_chip && area <maxArea_chip && (aspect_ratio<aspect_ratio_max_chip
        && aspect_ratio>aspect_ratio_min_chip) && solidity>solidity_min_chip) { 
      contours_filtered_chip.push_back(contours_chip[i]);
      minEnclosingCircle(contours_chip[i], center[i], radius[i]);
      drawContours( contours_image, contours_chip, i, color, 2, 2);
      circle( contours_image, center[i], radius[i], Scalar( 0, 255,0 ), 1, 8 );
      rectangle( contours_image, boundRect[i], Scalar( 0, 0, 255 ), 1, 8 );
      circle( contours_image, Point(boundRect[i].x+(boundRect[i].width)/2,boundRect[i].y+(boundRect[i].height)/2),
		      sqrt((boundRect[i].width)*(boundRect[i].width)+(boundRect[i].height)*(boundRect[i].height))/2, Scalar( 0, 0, 255 ), 1, 8 );
      drawContours( overlap_image, contours_chip, i, color, 2, 2);
      drawContours( overlap_image2, contours_chip, i, color, 2, 2);
    }
  }
  cout << "===> Chip Contours Filtered: " << contours_filtered_chip.size() << endl;

  if(whatimage_chip==0) imshow("Chip", image_chip);
  if(whatimage_chip==1) imshow("Chip", gray_image);
  if(whatimage_chip==2) imshow("Chip", filter_image);
  if(whatimage_chip==3) imshow("Chip", morph2_image);
  if(whatimage_chip==4) imshow("Chip", contours_image);
  if(whatimage_chip==5) imshow("Chip", overlap_image);
  if(whatimage_chip==6) imshow("Chip", overlap_image2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void static on_trackbar_sub( int, void* ){
  Mat new_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat gray_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat filter_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat morph_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat morph2_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat morph3_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat contours_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat overlap_image = Mat::zeros( image_sub.size(), image_chip.type() );
  Mat overlap_image2 = Mat::zeros( image_sub.size(), image_chip.type() );

  //vector<vector<Point> > contours_filtered_sub;
  vector<Vec4i> hierarchy;

  for( int y = 0; y < image_sub.rows; y++ )
    for( int x = 0; x < image_sub.cols; x++ )
      for( int c = 0; c < 3; c++ ){
        if (image_sub.at<Vec3b>(y,x)[c] > 80 && image_sub.at<Vec3b>(y,x)[c] < 174){
          new_image.at<Vec3b>(y,x)[c] =
	    saturate_cast<uchar>( ((image_sub.at<Vec3b>(y,x)[c] - 127 )*((double)contrast_sub/100) + 
				   (image_sub.at<Vec3b>(y,x)[c] - 127 )*((double)clarity_sub/100)) + 127 + brightness_sub );
	}
        else new_image.at<Vec3b>(y,x)[c] =
       saturate_cast<uchar>( (image_sub.at<Vec3b>(y,x)[c] - 127 )*((double)contrast_sub/100) + 127 + brightness_sub );
      }

  cvtColor(new_image, gray_image, COLOR_BGR2GRAY, 0);
  adaptiveThreshold(gray_image, filter_image, 255, ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, threshold_sub*2+1,0);

  Mat element = getStructuringElement(0, Size(1,1), Point(0,0) );

  morphologyEx(filter_image, morph_image, MORPH_CLOSE, element);
  morphologyEx(morph_image, morph2_image, MORPH_OPEN, element);

  morph3_image = morph2_image;
  findContours( morph3_image, contours_sub, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) );
  cout << "Sub Contours: " << contours_sub.size() << endl;

  vector<Rect> boundRect( contours_sub.size() );
  vector<Point2f> center(contours_sub.size());
  vector<float> radius(contours_sub.size());

  contours_filtered_sub = contours_sub;
  contours_filtered_sub.clear();

  overlap_image = 0 + image_sub;
  overlap_image2 = 0 + image_chip;

  for( unsigned int i = 0; i< contours_sub.size(); i++ ){
    area = contourArea(contours_sub[i]);
    boundRect[i] = boundingRect(contours_sub[i]);
    aspect_ratio = float(boundRect[i].width)/boundRect[i].height;
    if (area>minArea_sub && area <maxArea_sub && (aspect_ratio<aspect_ratio_max_sub && aspect_ratio>aspect_ratio_min_sub)) { 
      contours_filtered_sub.push_back(contours_sub[i]);
      minEnclosingCircle(contours_sub[i], center[i], radius[i]);
      drawContours( contours_image, contours_sub, i, color, 2, 2);
      circle( contours_image, center[i], radius[i], Scalar( 0, 0, 255 ), 1, 8 );
      drawContours( overlap_image, contours_sub, i, color, 2, 2);
      drawContours( overlap_image2, contours_sub, i, color, 2, 2);
    }
  }
  cout << "===> Sub Contours Filtered: " << contours_filtered_sub.size() << endl;

  if(whatimage_sub==0) imshow("Substrate", image_sub);
  if(whatimage_sub==1) imshow("Substrate", gray_image);
  if(whatimage_sub==2) imshow("Substrate", filter_image);
  if(whatimage_sub==3) imshow("Substrate", morph2_image);
  if(whatimage_sub==4) imshow("Substrate", contours_image);
  if(whatimage_sub==5) imshow("Substrate", overlap_image);
  if(whatimage_sub==6) imshow("Substrate", overlap_image2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void calculate(int , void*){

  vector<Point2f> center_sub(contours_filtered_sub.size());
  vector<float> radius_sub(contours_filtered_sub.size());
  vector<Point2f> center_chip(contours_filtered_chip.size());
  vector<float> radius_chip(contours_filtered_chip.size());
  vector<Rect> boundRect( contours_filtered_chip.size() );

  float distance=100;
  int match = 0;

  TH1D *hx = new TH1D("hx", "hx", 60, -14.5, 14.5);
  TH1D *hy = new TH1D("hy", "hy", 60, -14.5, 14.5);
  TH1D *px = new TH1D("px", "px", 90, -10.5, 40.5);
  TH1D *py = new TH1D("py", "py", 90, -10.5, 40.5);
  double pixel_to_um = 0.47;

  for(unsigned int i = 0; i< contours_filtered_sub.size(); i++ ){
    minEnclosingCircle(contours_filtered_sub[i], center_sub[i], radius_sub[i]);
    //px->Fill((center_sub.at(i).x - center_sub.at(i+1).x)*pixel_to_um);
    //py->Fill((center_sub.at(i).y - center_sub.at(i+1).y)*pixel_to_um);
    for( unsigned int j = 0; j< contours_filtered_chip.size(); j++ ){
      boundRect[j] = boundingRect(contours_filtered_chip[j]);
      minEnclosingCircle(contours_filtered_chip[j], center_chip[j], radius_chip[j]);
      //distance = sqrt(pow(center_sub.at(i).x - center_chip.at(j).x,2) + pow(center_sub.at(i).y - center_chip.at(j).y,2));
      distance = sqrt(pow(center_sub.at(i).x - (boundRect[j].x+(boundRect[j].width)/2),2) + pow(center_sub.at(i).y - (boundRect[j].y+(boundRect[j].height)/2),2));
      if (distance < 20) {
	      match++; 
        //hx->Fill((center_sub.at(i).x - center_chip.at(j).x)*pixel_to_um);
        //hy->Fill((center_sub.at(i).y - center_chip.at(j).y)*pixel_to_um);
        hx->Fill((center_sub.at(i).x - (boundRect[j].x+(boundRect[j].width)/2))*pixel_to_um);
        hy->Fill((center_sub.at(i).y - (boundRect[j].y+(boundRect[j].height)/2))*pixel_to_um);
      }
    }
  }

  for( unsigned long i = 0; i< center_sub.size()-1; i++ ){
    px->Fill(abs((center_sub.at(i).x - center_sub.at(i+1).x))*pixel_to_um);
    //py->Fill(abs((center_sub.at(i).y - center_sub.at(i+1).y))*pixel_to_um);
    //cout << (abs((center_sub.at(i).y - center_sub.at(i+1).y))*pixel_to_um) << endl;
  }

  cout << endl << "===> " << match << " Matches" << endl;
  TCanvas *c1 = new TCanvas();
  hx->Draw();
  hx->Fit("gaus");
  c1->SaveAs("Dist_X.png");
  hy->Draw();
  hy->Fit("gaus");
  c1->SaveAs("Dist_Y.png");
  px->Draw();
  px->Fit("gaus");
  c1->SaveAs("Pitch_X.png");
  //py->Draw();
  //py->Fit("gaus");
  //c1->SaveAs("Pitch_Y.png");
}

vector<Point> pixel;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void pixel_patern(int , int x, int y, int flags, void*){
  namedWindow("Pixel", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
  //namedWindow("Pixel2", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
  Mat pixel_contour = Mat::zeros(400, 400, CV_8UC3);
  pixel_contour = 0 + image_chip;

  Mat gray_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat inverted_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat filter_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat morph_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat morph2_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat morph3_image = Mat::zeros( image_sub.size(), image_sub.type() );
  Mat contours_image = Mat::zeros( image_sub.size(), image_sub.type() );

  vector<Vec4i> hierarchy;
  vector<Rect> boundRect( 1 );
  vector<vector<Point> > contours_pixel;

  Mat canny_output = Mat::zeros( image_sub.size(), image_sub.type() );
  
  Mat pixel_contour2 = Mat::zeros(2000, 2000, CV_8UC3);

  int cont = 0;
    if ( flags == EVENT_FLAG_CTRLKEY + EVENT_RBUTTONDOWN ) {
    cont++;
    cout << "Contour: " << cont << endl;
    }
  //if (whatimage_patern==0){
    if ( flags == EVENT_FLAG_CTRLKEY + EVENT_LBUTTONDOWN ) {
      pixel.push_back(Point(x,y));
      const cv::Point *pts = (const cv::Point*) Mat(pixel).data;
      int npts = Mat(pixel).rows;
      //pixel.push_back(Point(x,y));
      polylines(pixel_contour, &pts,&npts, 1,
          true,       // draw closed contour (i.e. joint end to start) 
          Scalar(0,255,0),// colour RGB ordering (here = green) 
          2,             // line thickness
          CV_AA, 0);
      polylines(pixel_contour2, &pts,&npts, 1, true, Scalar(0,255,0), 2, CV_AA, 0);
      //fillPoly(pixel_contour2, &pts,&npts, 1, Scalar(0,255,0), 8, 0, Point(0,0));
      imshow("Patern", pixel_contour);

      cvtColor(pixel_contour2, gray_image, COLOR_BGR2GRAY, 0);
      bitwise_not ( gray_image, inverted_image );
      adaptiveThreshold(inverted_image, filter_image, 255, ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 11,0);
      //threshold (gray_image, filter_image, 20, 255, CV_THRESH_BINARY_INV);
      Mat element = getStructuringElement(0, Size(1,1), Point(0,0) );
      morphologyEx(filter_image, morph_image, MORPH_CLOSE, element);
      //morphologyEx(morph_image, morph2_image, MORPH_OPEN, element);
      //Canny( gray_image, canny_output, 10, 300, 3 );
      //findContours( canny_output, contours_pixel, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
      findContours( morph_image, contours_pixel, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) );
      cout << "Sub Contours: " << contours_sub.size() << endl;
      
      //for( int i = 0; i< contours_sub.size(); i++ ){
	//boundRect[i] = boundingRect(contours_sub[i]);
	//aspect_ratio = float(boundRect[i].width)/boundRect[i].height;
      //}
      //imshow("Pixel", filter_image);
      imshow("Pixel", morph_image);
    }

    if ( flags == EVENT_RBUTTONDOWN ) {
      pixel.pop_back();
      pixel.pop_back();
      pixel.pop_back();
      pixel.pop_back();
      const cv::Point *pts = (const cv::Point*) Mat(pixel).data;
      int npts = Mat(pixel).rows;
      polylines(pixel_contour, &pts,&npts, 1, true, Scalar(0,255,0), 2,  CV_AA, 0);
      polylines(pixel_contour2, &pts,&npts, 1, true, Scalar(0,255,0), 2,  CV_AA, 0);
      imshow("Patern", pixel_contour);

      cvtColor(pixel_contour2, gray_image, COLOR_BGR2GRAY, 0);
      adaptiveThreshold(gray_image, filter_image, 255, ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 5,0);
      //Mat element = getStructuringElement(0, Size(1,1), Point(0,0) );
      //morphologyEx(filter_image, morph_image, MORPH_CLOSE, element);
      //morphologyEx(morph_image, morph2_image, MORPH_OPEN, element);
      //morph3_image = morph2_image;
      morph3_image = gray_image;
      findContours( morph3_image, contours_pixel, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) );
      cout << "Sub Contours: " << contours_sub.size() << endl;
      imshow("Pixel", morph3_image);
    }
    //imshow("Patern", pixel_contour);
  //}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main( int, char** argv )
{
  /// Read image given by user
  image_chip = imread( argv[1] );
  image_chip2 = imread( argv[1] );
  image_sub = imread( argv[2] );
  image_sub2 = imread( argv[2] );

  /// Initialize values
  contrast_chip = 102;
  brightness_chip = 112;
  clarity_chip = 0;
  threshold_chip = 5;

  contrast_sub = 124;
  brightness_sub = 63;
  clarity_sub = 0;
  threshold_sub = 34;

  namedWindow("Patern", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
  moveWindow("Patern", 640, 22);
  resizeWindow("Patern", 640, 780);
  setMouseCallback("Patern", pixel_patern, NULL);
  imshow("Patern", image_chip);

  /// Create Windows
  namedWindow("Chip", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
  moveWindow("Chip", 0, 22);
  resizeWindow("Chip", 640, 780);

  /// Create Trackbars
  createTrackbar( "Images", "Chip", &whatimage_chip, 6, on_trackbar_chip );
  createTrackbar( "Contrast", "Chip", &contrast_chip, alpha_max, on_trackbar_chip );
  createTrackbar( "Brightness", "Chip", &brightness_chip, beta_max, on_trackbar_chip );
  createTrackbar( "Clarity", "Chip", &clarity_chip, 100, on_trackbar_chip );
  createTrackbar( "Threshold", "Chip", &threshold_chip, 50, on_trackbar_chip );
  setTrackbarMin( "Threshold", "Chip", 0);

  /// Show some stuff
  imshow("Chip", image_chip);

  /// Create Windows
  namedWindow("Substrate", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
  moveWindow("Substrate", 640, 22);
  resizeWindow("Substrate", 640, 780);

  /// Create Trackbars
  createTrackbar( "Images", "Substrate", &whatimage_sub, 6, on_trackbar_sub );
  createTrackbar( "Contrast", "Substrate", &contrast_sub, alpha_max, on_trackbar_sub );
  createTrackbar( "Brightness", "Substrate", &brightness_sub, beta_max, on_trackbar_sub );
  createTrackbar( "Clarity", "Substrate", &clarity_sub, 100, on_trackbar_sub);
  createTrackbar( "Threshold", "Substrate", &threshold_sub, 50, on_trackbar_sub);
  setTrackbarMin( "Threshold", "Substrate", 0);

  /// Show some stuff
  imshow("Substrate", image_sub);

  cvCreateButton("Calculate", calculate, &button, CV_PUSH_BUTTON, 0);
  //cvCreateButton("Overlap", overlap, &button2, CV_PUSH_BUTTON, 0);

  /// Wait until user press some key
  waitKey();
  return 0;
}
