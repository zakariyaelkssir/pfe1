#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    if( argc != 4){
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }
    Mat image1,image2,image3;
    image1 = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file
    image2 = imread(argv[2], CV_LOAD_IMAGE_COLOR);   // Read the file
    image3 = imread(argv[3], CV_LOAD_IMAGE_COLOR);   // Read the file
    if(!image1.data || !image2.data)                              // Check for invalid input
    {
        cout <<  "Could not open or find the images" << std::endl ;
        return -1;
    }
   /* cv::Mat noise(image1.size(),image1.type());
        float m = (10,12,34);
        float sigma = (1,5,50);
        cv::randn(noise, m, sigma); //mean and variance
        image1 += noise;*/
  /*  namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", image1 );                   // Show our image inside it.
    waitKey(0);        */                                  // Wait for a keystroke in the window



    // Coverge image to Gray 
    Mat image1_gray, image2_gray,image3_gray;
    cvtColor(image1, image1_gray, COLOR_BGR2GRAY);
    cvtColor(image2, image2_gray, COLOR_BGR2GRAY);
    cvtColor(image3, image3_gray, COLOR_BGR2GRAY);
    Mat flow(image1_gray.size(), CV_32FC2);
    // Calculate The Optical Flow
    calcOpticalFlowFarneback(image2_gray, image1_gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);

    // show the flow 
        Mat flow_parts[2];
        split(flow, flow_parts);
        Mat magnitude, angle, magn_norm;
        cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
        normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
        angle *= ((1.f / 360.f) * (180.f / 255.f));
        //build hsv image
        Mat _hsv[3], hsv, hsv8, bgr;
        _hsv[0] = angle;
        _hsv[1] = Mat::ones(angle.size(), CV_32F);
        _hsv[2] = magn_norm;
        merge(_hsv, 3, hsv);
        hsv.convertTo(hsv8, CV_8U, 255.0);
        cvtColor(hsv8, bgr, COLOR_HSV2BGR);


        namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display. 
        imshow( "Display window", bgr );                   // Show our image inside it.
     waitKey(0);

   Mat prevImg;
    Mat map(flow.size(), CV_32FC2);
    for (int y = 0; y < map.rows; ++y){
         for (int x = 0; x < map.cols; ++x){
                    Point2f f = flow.at<Point2f>(y, x);
                    map.at<Point2f>(y, x) = Point2f(x + f.x, y + f.y);
    }
}
    Mat newFrame;
    split(map,flow_parts);
    //remap(image1, newFrame, );

    remap(image3_gray,prevImg,flow_parts[0],flow_parts[1],INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0) );
    namedWindow( "Display window -estimation-", WINDOW_AUTOSIZE );// Create a window for display. 
    imshow( "Display window -estimation-", prevImg );  
    namedWindow( "frame 1", WINDOW_AUTOSIZE );                 // Show our image inside it.
    imshow( "frame 1", image1_gray );
    namedWindow( "frame 2", WINDOW_AUTOSIZE );
    imshow( "frame 2", image3_gray );
    waitKey(0);
    imwrite("1.png", image3_gray );
    imwrite("2.png", prevImg );

    return 0;
}