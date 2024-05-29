# #MachineVision

## 1.问题

课题3.复杂环境中亚像素椭圆形中心点计算，可以采用的步骤为blob分析确定初始椭圆中心；从中心向外发射射线，按一维边缘点计算椭圆边界亚像素边缘点；再用椭圆拟合计算出亚像素椭圆中心。

## 2.BLOB分析

blob分析主要有三部分组成

### 获取图片:

使用 imread，转化为灰度图  成功

### 分割图片:

然后使用thersold,获得二值图   成功

出现问题：每张图片的阈值怎么取，用`THRESH_OTSU`：采用OTSU自动选择阈值能否可行？

形态学特征处理morph

1. **连通区域标记：** 使用连通区域标记算法找到图像中的连通区域（Blob），为每个不同的连通区域分配一个唯一的标记。

   **反转很重要，不然检测不到！！！因为连通域检测很重要的是它检测是白色部分**

   label:![image-20240528201550200](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240528201550200.png)

   ![image-20240528201612204](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240528201612204.png)

   ![image-20240528203151637](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240528203151637.png)

   显然，同一个连通域颜色一样，我现在任务是检测圆形，先把形状分析提上来，**圆形特征**，**面积**

   使用宽高比接近1来提取圆形，成功！

   ![image-20240528204321906](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240528204321906.png)

   再用面积进行限制，先打印出了每个连通区域的面积

   ![image-20240528211153182](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240528211153182.png)![image-20240528211212062](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240528211212062.png)

   显然的发现，圆形的面积应该在8k-2w内

   成功纯净的提取出圆形！

   ![image-20240528211623298](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240528211623298.png)

   接下来存储获得的好的连通域（圆形）

   ![image-20240528212423833](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240528212423833.png)

   利用矩计算出中心位置

   ![image-20240528212941039](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240528212941039.png)

   提取边缘点（成功！）

   每一个连通域（圆形）成功提取出了一组边缘点并打印在屏幕上

   ![image-20240529110155261](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240529110155261.png)

   （图片展示不完全）

   现在只需要在每一组边缘点提取后，先不管算法，先拟合出椭圆中心

   ![image-20240529111224173](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240529111224173.png)

   貌似提取到了所谓的亚像素椭圆中心，但计算的慢，怎么解决，同时最好能在这个center上标记一个圆心

   ![image-20240529113635511](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240529113635511.png)

   标记上了圆心，但明显这个圆心不是我想要的

   轮廓有问题？no，我已经分割出了圆形轮廓

   ![](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240529114137115.png)

   感觉检测到的是边缘点，是我的center不对。

   ![image-20240529120705623](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240529120705623.png)

   说明，打印pair.second其实是识别到了圆心的

   接下来利用pair.second向四周发射射线，然后判断条件是像素灰度值的剧烈变化，获得边缘点，最后在用这些边缘点拟合出椭圆中心，并画出拟合轨迹

   ![image-20240529121900436](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240529121900436.png)

   成功检测到绿色圆的边缘以及红色圆心，阈值为50

   ![image-20240529122047617](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240529122047617.png)

   阈值为100，边缘效果明显好了很多

   ![image-20240529143128481](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240529143128481.png)

   成功获得亚像素中心点，效果如下

   ![image-20240529143155335](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240529143155335.png)

   不显示原本的红色圆心后，只显示拟合圆心

   ![image-20240529143929380](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240529143929380.png)

   ![image-20240529144203619](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240529144203619.png)![image-20240529144212610](C:\Users\Ste'fan\AppData\Roaming\Typora\typora-user-images\image-20240529144212610.png)

2. **特征提取：** 对每个连通区域进行特征提取，例如计算 Blob 的面积、周长、中心位置等特征。

3. **形状分析：** 分析 Blob 的形状特征，比如长宽比、凸包等。

4. **过滤和选择：** 可以根据提取的特征对特定的 Blob 进行过滤、选择或标记。

### 提取特征:

way1.轮廓提取。

出现问题：分割椭圆的面积阈值怎么选择，明显的是,如下选择阈值

```c++
 if (contourArea(contours[i]) > 10 &&contourArea(contours[i]) < 50)`
     
```

最后的只存入了12个椭圆，实际上fig1中有63个椭圆

```
成功存入第0个椭圆中心: [443.868, 1963.5]
成功存入第1个椭圆中心: [428.118, 1618.68]
成功存入第2个椭圆中心: [389.845, 1594.96]
成功存入第3个椭圆中心: [320.091, 1566.82]
成功存入第4个椭圆中心: [300.43, 1529.01]
成功存入第5个椭圆中心: [243.512, 1531.2]
成功存入第6个椭圆中心: [111.797, 1454.99]
成功存入第7个椭圆中心: [123.366, 1456.67]
成功存入第8个椭圆中心: [101.03, 1451.18]
成功存入第9个椭圆中心: [340.326, 1443.41]
成功存入第10个椭圆中心: [56.1538, 1399.8]
成功存入第11个椭圆中心: [61.5256, 1384.29]
成功存入第12个椭圆中心: [130.692, 876.6]
//12个椭圆
```

解决办法：设置更广泛的阈值，是否有函数能直接将分割好的椭圆圈出来？



```c++
 if (contourArea(contours[i]) > 100 )// && contourArea(contours[i]) < 50
        { 
            // 获取轮廓的包围矩形框
            Rect boundingBox = boundingRect(contours[i]);        // 计算宽高比
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
```

//事实证明不好用，压根检测不出圆



way2：使用霍夫圆变换检测圆

不好用，再试试



way3 模板匹配

待定



接下来的思路是用模板匹配，匹配出圆形轮廓，找到圆心，再根据要求后续操作
