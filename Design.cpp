#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;


// 寻找初始中心点函数,传入连通区域图
vector<Point2f> findInitialCenter(const Mat& image) {
    
    Mat output;
    // 找轮廓
    vector<vector<Point>> contours;
    findContours(image, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//RETR_EXTERNAL：只检测最外层的轮廓。CHAIN_APPROX_SIMPLE：压缩水平、垂直和对角线段，只保留他们的端点。
   // drawContours(output, contours, (int)i, Scalar(0, 0, 255), 2);//画轮廓成功
    // 存储所有椭圆的中心点
    vector<Point2f> centers;

    for (size_t i = 0; i < contours.size(); i++) 
    {
        //
        if (contourArea(contours[i]) > 100 )// && contourArea(contours[i]) < 50
        { 
            // 获取轮廓的包围矩形框
            Rect boundingBox = boundingRect(contours[i]);

            // 计算宽高比
            float aspectRatio = (float)boundingBox.width / (float)boundingBox.height;

            // 检查宽高比是否接近1（圆形的特点）
            if (aspectRatio > 0.8 && aspectRatio < 1.2)
            {
                Moments mu = moments(contours[i]);
                Point2f center(mu.m10 / mu.m00, mu.m01 / mu.m00);
                circle(output, center, 3, Scalar(0, 255, 0), -1);//画圆心，把半径取很小，近似圆心

                drawContours(output, contours, (int)i, Scalar(0, 0, 255), 2);//画轮廓成功
                centers.push_back(center);
            }

        }

     
       
    }
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
    
   
    return centers;
}


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

        // 显示原始图片
        imshow("OriginalPicture", src);
        waitKey(0);
    }
    GaussianBlur(src, src, Size(9, 9), 2, 2); // 高斯模糊

    // 反转图像（黑白反转）
    Mat inverted;
    bitwise_not(src, inverted);

    Mat binarized; // 二值化图像
    threshold(inverted, binarized, 80, 255, THRESH_BINARY);

    namedWindow("binarizedPicture", WINDOW_NORMAL);
    resizeWindow("binarizedPicture", binarized.cols, binarized.rows);
    imshow("binarizedPicture", binarized);
    waitKey(0);

    // 使用形态学操作（闭运算）去除噪声并填充椭圆内的空隙。
    Mat morph;
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    morphologyEx(binarized, morph, MORPH_CLOSE, element);
   
    //使用连通区域标记
    Mat labels;
    int numLabels = connectedComponents(binarized, labels);

    // 输出找到的连通组件数量
    std::cout << "Number of labels: " << numLabels << std::endl;
    
    // 创建一个彩色版本的标记图像
    Mat coloredLabels = Mat::zeros(src.size(), CV_8UC3);
    vector<Vec3b> colors(numLabels);
    colors[0] = Vec3b(0, 0, 0); // 背景设为黑色
    for (int i = 1; i < numLabels; i++) {
        colors[i] = Vec3b(rand() % 256, rand() % 256, rand() % 256); // 生成随机颜色
    }

    //// 将标记的连通域着色到彩色标记图像上
    //for (int y = 0; y < coloredLabels.rows; y++) {
    //    for (int x = 0; x < coloredLabels.cols; x++) {
    //        int label = labels.at<int>(y, x);
    //        coloredLabels.at<Vec3b>(y, x) = colors[label];
    //    }
    //}

   
 // 创建一个容器来存储符合条件的 label 和它们的中心位置
    std::vector<std::pair<int, Point2f>> targetLabelsAndCenters;


    // 遍历每个连通域并计算其宽高比
    for (int label = 1; label < numLabels; ++label) 
    {
        Mat mask = (labels == label);

        // 查找连通域的边界框
        vector<vector<Point>> contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        
        if (contours.size() > 0 ){
            Rect boundingBox = boundingRect(contours[0]);
            float aspectRatio = static_cast<float>(boundingBox.width) / boundingBox.height;
           
            
                double area = contourArea(contours[0]);
                //std::cout << "Contour " << label << " area: " << area << std::endl;//输出面积，便于找到阈值
           //判断面积是否在8k-2w内
                if (area > 8000 && area < 20000)
                {
                    // 计算轮廓的矩
                    Moments M = moments(contours[0]);

                    // 计算质心
                    Point2f center(M.m10 / M.m00, M.m01 / M.m00);

                    // 将 label 和其中心位置添加到容器中
                    targetLabelsAndCenters.push_back(std::make_pair(label, center));
                    

                    // 判断宽高比是否接近1
                    if (aspectRatio > 0.9 && aspectRatio < 1.1)
                    {
                        // 将标记的连通域着色到彩色标记图像上
                        for (int y = 0; y < coloredLabels.rows; y++)
                        {
                            for (int x = 0; x < coloredLabels.cols; x++)
                            {
                                if (labels.at<int>(y, x) == label)
                                {
                                    coloredLabels.at<Vec3b>(y, x) = colors[label];
                                }
                            }
                        }
                    }
                }
        }
    }
    // 输出符合条件的 label 和它们的中心位置
    std::cout << "Target Labels and Centers: ";
    for (const auto& pair : targetLabelsAndCenters) {
        std::cout << "Label: " << pair.first << ", Center: (" << pair.second.x << ", " << pair.second.y << ")" << std::endl;
    }



    // 显示标记后的图像
    namedWindow("Connected Components", WINDOW_NORMAL);
    resizeWindow("Connected Components", coloredLabels.cols, coloredLabels.rows);
    imshow("Connected Components", coloredLabels);
    waitKey(0);

    //// 定义每个射线的角度间隔（可以根据需要调整）
    //const double angleInterval = CV_PI / 180.0;

    //// 定义边缘检测的阈值（可以根据需要调整）
    //const double edgeThreshold = 2.0;

    //// 创建一个容器来存储边缘点
    //std::vector<Point2f> edgePoints;

    //for (const auto& pair : targetLabelsAndCenters)
    //{
    //    int label = pair.first;
    //    Point2f center = pair.second;

    //    // 获取当前连通域的轮廓
    //    Mat mask = (labels == label);
    //    vector<vector<Point>> contours;
    //    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    //    if (contours.size() > 0)
    //    {
    //        // 获取当前连通域的轮廓边界
    //        vector<Point> contour = contours[0];

    //        // 计算当前连通域的最小外接椭圆
    //        RotatedRect ellipse = fitEllipse(contour);

    //        // 计算当前连通域的边界矩形
    //        Rect boundingRect = boundingRect(contour);

    //        // 发射射线并计算边缘点
    //        for (double angle = 0; angle < 360; angle += angleInterval)
    //        {
    //            double radian = angle * CV_PI / 180.0;
    //            double dx = cos(radian);
    //            double dy = sin(radian);

    //            // 发射射线
    //            for (double t = 0; t < max(boundingRect.width, boundingRect.height); t += 0.5)
    //            {
    //                Point2f point(center.x + t * dx, center.y + t * dy);

    //                // 判断点是否在轮廓内
    //                double distance = pointPolygonTest(contour, point, true);

    //                // 如果距离小于阈值，则认为该点是边缘点
    //                if (distance > -edgeThreshold && distance < edgeThreshold)
    //                {
    //                    // 将边缘点添加到容器中
    //                    edgePoints.push_back(point);
    //                    break;
    //                }
    //            }
    //        }
    //    }
    //}

  

    return 0;
}
