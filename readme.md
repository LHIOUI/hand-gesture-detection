## 编译环境

这个 Porject 的编译环境包含了：Qt5.3.1 （MinGW4.8.2、OpenGL）、OpenCV

编译环境搭建可以参看文章：[在 Qt 中使用 opencv](http://qiqi8226.com/tech/2014/08/18/qt-and-opencv.html)

## 程序简介

![](/i/1.png)

上图是程序运行主窗口，首先用户需要将手盖住图像上的7个绿点然后点击右上角的`color collection`按钮以便为以后的检测提供基准数据，使用过程中如果需要重新采集数据可以点击`color re-collection`按钮。如果需要对基准数据的阀值范围做调整请点击最下方`Thershold trackbar`按钮便可以在 HLS 颜色空间控制颜色范围。

![](/i/2.png)
![](/i/3.png)

程序中 `source` 与 `binary` 两个 radioButton 可以切换显示源图像与识别后的二值图像。下方的 `con defect` checkBox 可以勾选是否显示识别出的凸包点。

![](/i/4.png)
![](/i/5.png)

`contour` checkBox 可以勾选是否显示图中用蓝色显示的识别出的轮廓，`poly` 是对轮廓进行多边形拟合后的结果，图中用红色标识。

![](/i/6.png)

`mouse on/off` checkBox 控制是否开启鼠标控制功能。同时激活的 `grid` 表示图中的方框表格，`rect` 表示对手识别后的矩形范围。

鼠标功能开启后会激活显示蓝色叉符号，它的坐标由图中所有绿色圆圈求平均所得。以叉号的位置与图中的表格做对比控制鼠标移动。

图像的左下方最后一行 `command number` 显示识别到的手势数字，数字 2、3、1 分别表示left_click、left_down、left_up 事件。
