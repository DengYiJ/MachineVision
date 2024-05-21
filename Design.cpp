#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;


// 寻找初始中心点函数
vector<Point2f> findInitialCenter(const Mat& image, const Mat& output) {
    Mat binarized;//二值化图像
    threshold(image, binarized, 80,255, THRESH_BINARY);
    //threshold(image, binarized, 0, 255, THRESH_OTSU | THRESH_BINARY);
    /*namedWindow("Binarized", WINDOW_NORMAL);
    resizeWindow("Binarized", binarized.cols, binarized.rows);
    imshow("Binarized", binarized);
    waitKey(0);*/

    // 使用形态学操作（闭运算）去除噪声并填充椭圆内的空隙。
    Mat morph;
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    morphologyEx(binarized, morph, MORPH_CLOSE, element);
    /*namedWindow("morgh", WINDOW_NORMAL);
    resizeWindow("morgh", morph.cols,morph.rows);
    imshow("morgh", morph);
    waitKey(0);*/

    // 找轮廓
    vector<vector<Point>> contours;
    findContours(morph, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//RETR_EXTERNAL：只检测最外层的轮廓。CHAIN_APPROX_SIMPLE：压缩水平、垂直和对角线段，只保留他们的端点。
   // drawContours(output, contours, (int)i, Scalar(0, 0, 255), 2);//画轮廓成功
    // 存储所有椭圆的中心点
    vector<Point2f> centers;

    //for (size_t i = 0; i < contours.size(); i++) 
    //{
    //    //
    //    if (contourArea(contours[i]) > 100 )// && contourArea(contours[i]) < 50
    //    { 
    //        // 获取轮廓的包围矩形框
    //        Rect boundingBox = boundingRect(contours[i]);

    //        // 计算宽高比
    //        float aspectRatio = (float)boundingBox.width / (float)boundingBox.height;

    //        // 检查宽高比是否接近1（圆形的特点）
    //        if (aspectRatio > 0.8 && aspectRatio < 1.2)
    //        {
    //            Moments mu = moments(contours[i]);
    //            Point2f center(mu.m10 / mu.m00, mu.m01 / mu.m00);
    //            circle(output, center, 3, Scalar(0, 255, 0), -1);//画圆心，把半径取很小，近似圆心

    //            drawContours(output, contours, (int)i, Scalar(0, 0, 255), 2);//画轮廓成功
    //            centers.push_back(center);
    //        }

    //    }

    // 
    //   
    //}
    //使用霍夫圆变换识别圆画圆
    //vector<Vec3f>  HCircles;
    //HoughCircles(morph, HCircles, HOUGH_GRADIENT, 1, morph.rows / 64, 200, 100, 0, 0);
    //    for (size_t i = 0; i < HCircles.size(); i++) {
    //        Point center(cvRound(HCircles[i][0]), cvRound(HCircles[i][1]));
    //        int radius = cvRound(HCircles[i][2]);

    //        // 绘制检测到的圆
    //        circle(morph, center, radius, Scalar(0, 0, 255), 2);
    //  }
    //if (!HCircles.empty())
    //{
    //    for (size_t i = 0; i < HCircles.size(); i++)
    //    {
    //        cout << "成功存入第" << i << "个椭圆圆心(x,y): " << "(" << HCircles[i][0] << "," << HCircles[i][1]<<")" << endl;
    //    }
    //   
    //}
    //else
    //{
    //    cout << "Could not find the houghCircle!" << endl;
    //}
 
   /*    
    namedWindow("HoughCircle", WINDOW_NORMAL);
    resizeWindow("HoughCircle", output.cols, output.rows);
    imshow("HoughCircle", output);
    waitKey(0);*/
    
    namedWindow("HoughCircle", WINDOW_NORMAL);
    resizeWindow("HoughCircle", morph.cols, morph.rows);
    imshow("HoughCircle", morph);
    waitKey(0);
    return centers;
}

//// Helper function to find edge points using ray casting
//vector<Point2f> findEdgePoints(const Mat& image, const Point& center) {
//    vector<Point2f> edgePoints;
//
//    // Parameters for ray casting
//    int numRays = 360;
//    double maxLength = min(image.rows, image.cols) / 2.0;
//
//    for (int angle = 0; angle < numRays; angle++) {
//        double radian = angle * CV_PI / 180.0;
//        Point2f direction(cos(radian), sin(radian));
//
//        for (double length = 0; length < maxLength; length++) {
//            Point2f currentPoint = center + length * direction;
//            if (currentPoint.x < 0 || currentPoint.y < 0 || currentPoint.x >= image.cols || currentPoint.y >= image.rows) {
//                break;
//            }
//            if (image.at<uchar>(currentPoint) == 0) {
//                edgePoints.push_back(currentPoint);
//                break;
//            }
//        }
//    }
//
//    return edgePoints;
//}

int main()
{
    // 读取输入图像
    Mat src = imread("C:\\Users\\Ste'fan\\Desktop\\MachineVision\\design\\Image\\Image01.jpg",IMREAD_GRAYSCALE);//, IMREAD_GRAYSCALE
    if (src.empty()) {
        cout << "Could not open or find the image!" << endl;
        return -1;
    }
    else 
    {
        cout << "Sucessfully find picture!" << endl;
        namedWindow("OriginalPicture", WINDOW_NORMAL);

        //调整窗口大小以适应图片大小
        resizeWindow("OriginalPicture", src.cols, src.rows);

        // 显示图片
        imshow("OriginalPicture", src);
        waitKey(0);
    }
    GaussianBlur(src, src, Size(9, 9), 2, 2); // 高斯模糊
    Mat output = Mat::zeros(src.size(), CV_8UC3);

    // 找到初始椭圆中心
    vector<Point2f> initialCenter = findInitialCenter(src,output);

    //// 从中心向外发射射线，找到边缘点
    //vector<Point2f> edgePoints = findEdgePoints(src, initialCenter);

    //// 使用边缘点进行椭圆拟合
    //if (edgePoints.size() < 5) {
    //    cout << "Not enough edge points to fit an ellipse." << endl;
    //    return -1;
    //}

    //RotatedRect ellipse = fitEllipse(edgePoints);

    //// 显示结果
    //Mat result;
    //cvtColor(src, result, COLOR_GRAY2BGR);
    //circle(result, initialCenter, 3, Scalar(0, 0, 255), -1);
    //ellipse(result, ellipse, Scalar(0, 255, 0), 2);

    //imshow("Fitted Ellipse", result);
    //waitKey(0);

    return 0;
}
