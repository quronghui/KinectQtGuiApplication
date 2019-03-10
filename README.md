# Kinect QtGuiApplication
+ Kinect for windows.
+ [Kinect SDK Studying 参考博客](https://quronghui.github.io)

[TOC]



# Kinect in Windows

- [Kinect in Windows 开发指南 ](https://blog.csdn.net/cz19800823/article/list/1?)
- [kinect + opencv + vs 环境配置](https://blog.csdn.net/lizhiguo18/article/details/51037672)

## 安装 Kinect SDK 

1. Bug
   - 首次安装KinectSDK-v1.8-Setup
   - 连上kinect后，打开一次后，第二次打开提示 “ No kinet to connect ”
   - 重新卸载后，安装KinectSDK-v1.7-Setup，成功
2. 在官方网站下载Kinect for Windows SDK和Developer Toolkit：
   - [参考教程](https://blog.csdn.net/zouxy09/article/details/8146055)
   - [KinectSDK-v1.7-Setup.exe](https://www.microsoft.com/en-us/download/details.aspx?id=36996)
   - [KinectDeveloperToolkit-v1.7.0-Setup.exe](<https://www.microsoft.com/en-us/download/details.aspx%3Fid%3D36998>)
   - 安装成功后，连上kinect, 设备管理器出现 “ Kinect for windows ”
   - 通过kinect tools 测试 XBox的好坏

## 安装 Visual Studio

### VS2017

1. [安装教程](https://blog.csdn.net/fengbingchun/article/details/83990685)
2. Download [Visual Studio Community](https://visualstudio.microsoft.com/free-developer-offers/)免费版
3. 选择工作负载这里仅勾选”使用C++的桌面开发”，单个组件和语音包使用默认
4. 按照需要选择要安装的模块（但是我下载后没有已安装的选项）
5. VS2017的文件目录 C:\Users\quronghui\source\repos

### 安装注意

1. 直接安装VS2017，并且卸载完之前安装的VS2015版本（如果可以的话直接升级）
2. 我的是因为之前安装的VS2015 ios镜像不成功，卸载不完全，导致的系统重装.
3. Mircosoft 的软件安装的时候，不要中途暂停，不然容易造成卸载不了的问题
4. **解决方案**
   - 重装系统后，直接安装VS2017，这样就不会报错了
   - 因该是以前装VS其他版本没有卸载干净，导致的后果

## Kiinect SDK 关于 VS 的配置

1. Kiinect SDK 关于 VS 的配置

   - 使用VS进行开发的话，需要配置include和lib路径

   - 进入VS，右键-项目属性，Debug + x64，选择VC++Directories:

     {% asset_img KinectEnvironment.PNG %}

     ```
     Debug and Release 都要添加
     	Include files : add  C:\Program Files\Microsoft SDKs\Kinect\v1.7\inc；
     	Library files : add  C:\Program Files\Microsoft SDKs\Kinect\v1.7\lib\x64；
     	链接器         : add  Kinect10.lib
     ```

2. C:\Program Files\MicrosoftSDKs\Kinect\v1.6\inc 目录下有：

   ```
   - NuiApi.h ---包含所有的NUI(自然用户界面) API头文件和定义基本的初始化和函数访问入口。这是我们C++工程的主要头文件，它已经包含了NuiImageCamera.h 和 NuiSkeleton.h。
   - NuiImageCamera.h ---定义了图像和摄像头服务的API，包括调整摄像头的角度和仰角，打开数据流和读取数据流等。
   - NuiSkeleton.h ---骨架有关的API，包括使能骨架跟踪，获取骨架数据，骨架数据转换和平滑渲染等。
   - NuiSensor.h ---音频API，包括ISoundSourceLocalizer接口，用于返回声源的方向（波束形成）和音频的位置。
   ```

## 安装 OpenCV

- 本次安装的opencv，是为了迁移上一个工程师的代码，所以opencv的版本需要一致才能编译成功。
- [opencv 2.4.6.0，vc10](https://pan.baidu.com/s/1kJIOV-g_wIDIAjqAsB8FSA)
  - 提取码：f3pz 
- [ Opencv 的安装和配置教程 ](https://blog.csdn.net/qq_41175905/article/details/80560429)

### Opencv 的安装

1. 我是直接复制工程师的 opencv 解压包，只是进行依赖链接的配置。

2. 环境变量

   {% asset_img OpencvEnvironment.jpg %}

3. Opencv Start Dll 文件的复制

   - dll 文件都在安装的opencv的 bin 目录下

   - ```
     copy   : opencv_ffmpeg .dll  to  C:\Windows\System32
     copy   : opencv_other  .dll  to  C:\Windows\SysWOW64
     ```

   {% asset_img OpencvStartDll.PNG %}

### Opencv With VS2017

1. VS  属性中（ Include Direction）:

   ```
    Include Direction： 
   	C:\Software\opencv\opencv\include
   	C:\Software\opencv\opencv\build\include\opencv
   	C:\Software\opencv\opencv\build\include\opencv2
   	C:\Software\opencv\freetype\include			// 为了在opencv里面检测的图片上加上字
   ```

   - {% asset_img OpencvVsInclude.PNG %}

2. VS 属性里面的 库目录

   ```
   Library Direction:
   	C:\Software\opencv\opencv\build\x64\vc10\lib
   ```

   - {% asset_img OpencvVsLibrary.PNG %}

3. VS 属性 -- 链接器 -- 输入项 -- 附加依赖项

   ```
   " **在 Debug  x64 下面进行配置**"
   Link :
   	opencv_core246d.lib			// lib 库下面 opencv_xxx "d" : 代表的是Debug版本使用的库
   	opencv_highgui246d.lib
   	opencv_video246d.lib
   	opencv_imgproc246d.lib
   	opencv_photo246d.lib
   ```

   - {% asset_img OpencvVsLink.PNG %}

   ```
   " **切换 Release  x64 下面进行配置**"
   Link :
   	opencv_core246.lib			// lib 库下面 opencv_xxx  : 代表的是Release版本使用的库
   	opencv_highgui246.lib
   	opencv_video246.lib
   	opencv_imgproc246.lib
   	opencv_photo246.lib
   ```

   - {% asset_img OpencvVsLinkR.PNG %}

## 安装QT

### QT 的安装

1. [安装方式](https://blog.csdn.net/gaojixu/article/details/82185694)

   ```
   Notes:
   	QT组件的选取：
   	MSVC 2017 64-bit  : VS2017 的QT编译器
   	MinGW 5.3.0 32 bit : gcc 的编译器吧
   ```

### QT with VS2017

1. [配置方式](https://blog.csdn.net/gaojixu/article/details/82185694)

2. Notes

   ```
   VS 属性 -- 链接器 -- 输入项 -- 附加依赖项 ：加入QT. lib
   如果报错再加入：
   	qtmain.lib
   	Qt5Core.lib
   	Qt5Gui.lib
   	Qt5Widgets.lib
   ```

   {% asset_img OTVsLinkDR.PNG %} 

## 代码迁移后需要改动

### VS 的项目  --  配置属性

1. VC ++ 目录 ：

   - 下面的包含目录和库目录。
   - 注意修改迁移项目的目录链接。
   - （之前背的工程师的SDK or opencv  放在其他的目录下）

2. Qt Meta-Object Complier:

   - Include Path

     {% asset_img QtVsInclude.PNG %} 

3. C/C++ 目录

   - 常规 -- 附加包含目录：

     {% asset_img C++Direction.PNG %} 

   - 所有选项 -- 附加包含目录

### 计算机中丢失 MSVCP100D.dll

1. 代码迁移后，会报计算机中丢失 MSVCP100D.dll

2. 解决方案：去对应工程师的电脑下，拷贝**MSVCP100D.dll**

   - [download](https://pan.baidu.com/s/1YZ6eC3sa_3U9GlKovBDpUw) 
     提取码：fkvr 

   ```
   copy  xxx.dll files to C:\Windows\SysWOW64 
   copy  xxx.dll files to C:\Windows\System32
   ```

# QtGuiApplication Code

![代码逻辑](https://github.com/quronghui/KinectQtGuiApplication/blob/master/code1.jpg)

![功能函数](https://github.com/quronghui/KinectQtGuiApplication/blob/master/code2.jpg)

## main.cpp

+ [建立Qt Gui 的函数](https://quronghui.github.io/2019/03/01/QtAndVs/)

1. 主函数 main.cpp 就是QtGui的主函数；
2.  QtGuiApplication1.h and QtGuiApplication1.cpp
   + 里面定义之后的字段以及函数声明，以及槽的实现，都是在这里，Qt的逻辑功能设计主要是修改这个文件。
   + 这个就是逻辑函数

## QtGuiApplication1.cpp

1. QtGuiApplication1.h

   ```
   // QtGuiApplication1(工程的名字Widget)类的头文件，定义一些字段和函数声明，
   //包括最重要的slots（槽）函数的声明，以及界面ui句柄，以便通过“ui.***”的方式访问到界面的各个控件，
   // 比如访问界面的Label控件里的文字可以这样：ui.label->text();就是字面意思，很容易理解。
   ```

2. QtGuiApplication1.cpp

   + 逻辑代码文件；
   + 所有实现功能的函数文件

## Mymain.cpp

1. /*进行Kinect的连接和初始化 */

2. 获取Color , Skeleton, and Depth Data

3. 自动完成拍摄正面和侧面

   ```
   调用函数：
   （1）mt = new MyTimer(this);	
    (2) /*界面上显示的测量时间倒计时，以及显示文字，是否可以换成语音播报*/ 
   	script = new Script(7,4);
   （3）unit.cpp
   ```

4. 		// 身体数据填充到身体数据流
      		/**************3.函数进行第三次跳转，获取bodydate*********/

##  bodydate.cpp

1. Skeleton 骨骼点数据时，使用的函数

##  bodydateByFlow.cpp

1. // 摄像头采集的frame，每秒30帧

## Human

1. 没找到在哪里调用。

## Learning

1. 一个学习的函数，不在工程里。

# VS2017 项目打包部署（.exe files）

- [参考链接](https://blog.csdn.net/u010638673/article/details/80652831)

1. [ Vs的部署方式 部署连接，Mircosoft官网](https://docs.microsoft.com/zh-cn/previous-versions/visualstudio/visual-studio-2010/e2444w33%28v%3dvs.100%29)

## Vs + Qt 的应用程序发布部署

- [通过编译Release 得到可执行文件exe，生成相关可执行文件](https://blog.csdn.net/china_jeffery/article/details/78402623)

1.  将需要发布的exe（Release ）文件编译后（如QtGuiApplication1.exe），放到单独的目录。

2. 在“开始菜单”启动Qt 5.9.1 32-bit for Desktop (MSVC 2015);

   1.  Win + R 进去 cmd编辑；

   2. cd  C:\Software\kinect\QtGuiApplication1\x64\Release  进入 exe 文件的目录

   3. 执行命令

      ```
      windeployqt.exe QtGuiApplication1.exe --qmldir C:\Qt\Qt5.12.1\5.12.1\msvc2017_64\qml
      ```

   4. 生成运行时需要的文件。（源程序发生改变，需要重新生成一次）

3. Vs 2017的依赖库

   ```
   进入路径
   C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\Remote Debugger\x64
   拷贝文件
   api-ms-win-core-* 
   concrt140.dll
   msvcp140.dll
   ucrtbase.dll
   vcruntime140.dll
   粘贴到 *步骤一的目录下*
   ```

4. 这样迁移到相同屏幕尺寸的电脑下，就可以运行了。

5. 如果出现Qt提示的文字超出界面，修改字体的大小就能自适应平板了。