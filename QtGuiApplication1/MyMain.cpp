#include "MyMain.h"


void MyMain::getSkeletonImage()
{
	// 骨骼帧信息
	NUI_SKELETON_FRAME skeletonFrame = { 0 };
	// 骨骼信息是否存在的标志
	bool bFoundSkeleton = false;
	// 如果获得下一帧骨骼信息
	if (NuiSkeletonGetNextFrame(0, &skeletonFrame) == S_OK)
	{	// 遍历所有 id
		for (int i = 0; i < NUI_SKELETON_COUNT; i++)
		{	// 如果存在骨骼状态处于跟踪态的
			if (skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED)
			{	// 变更标志位
				bFoundSkeleton = true;
				break;
			}
		}
	}
	else
	{
		cout << "without appropriate bone" << endl;
		return;
	}
	// 如果标志位未变更，直接结束函数，提高效率
	if (!bFoundSkeleton) return;
	//平滑骨骼帧, 消除抖动 
	NuiTransformSmooth(&skeletonFrame, NULL);     
	// 遍历所有 id
	for (int i = 0; i < NUI_SKELETON_COUNT; i++)
	{	// 如果存在骨骼状态处于跟踪态的，并且肩膀中心也处于跟踪态的
		if (skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&
			skeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{	// 承接坐标信息
			float fx, fy;
			// 遍历骨骼点
			for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)
			{	// 所有骨骼点坐标转化为深度图的坐标
				NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[j], &fx, &fy);
				skeletonPoint[i][j].x = (int)fx;
				skeletonPoint[i][j].y = (int)fy;
			}
		}
	}
}

void getTheContour(Mat &image, int whichone, Mat &mask)
{
	// 遍历 image 的行
	for (int i = 0; i < image.rows; i++)
	{	// 赋值用以处理每个像素
		uchar *ptr = image.ptr<uchar>(i);
		// 遮罩的行列数和 image 相同
		uchar *ptrmask = mask.ptr<uchar>(i);
		// 遍历 image 的列
		for (int j = 0; j < image.cols; j++)
		{	// 都为0的时候予以忽略 
			if (ptr[3 * j] == 0 && ptr[3 * j + 1] == 0 && ptr[3 * j + 2] == 0) 
			{
				ptrmask[3 * j] = ptrmask[3 * j + 1] = ptrmask[3 * j + 2] = 0;
			}// id 为 1 的时候，显示绿色
			else if (ptr[3 * j] == 0 && ptr[3 * j + 1] == 0 && ptr[3 * j + 2] != 0)
			{
				ptrmask[3 * j] = 0;
				ptrmask[3 * j + 1] = 255;
				ptrmask[3 * j + 2] = 0;
			}// id 为 2 的时候，显示红色
			else if (ptr[3 * j] == 0 && ptr[3 * j + 1] != 0 && ptr[3 * j + 2] == 0) 
			{
				ptrmask[3 * j] = 0;
				ptrmask[3 * j + 1] = 0;
				ptrmask[3 * j + 2] = 255;
			}// id 为 3 的时候，显示蓝色 
			else if (ptr[3 * j] == ptr[3 * j + 1] && ptr[3 * j + 2] == ptr[3 * j] / 4)
			{
				ptrmask[3 * j] = 255;
				ptrmask[3 * j + 1] = 255;
				ptrmask[3 * j + 2] = 0;
			}// id 为 4 的时候，显示紫色
			else if (ptr[3 * j + 1] == ptr[3 * j] / 4 && ptr[3 * j + 1] == ptr[3 * j + 2])
			{
				ptrmask[3 * j] = 255;
				ptrmask[3 * j + 1] = 0;
				ptrmask[3 * j + 2] = 255;
			}// id 为 5 的时候，显示青色
			else if (ptr[3 * j + 1] == ptr[3 * j] / 4 && ptr[3 * j] == ptr[3 * j + 2])
			{
				ptrmask[3 * j] = 0;
				ptrmask[3 * j + 1] = 255;
				ptrmask[3 * j + 2] = 255;
			}// id 为 3 的时候，显示蓝色
			else if (ptr[3 * j + 1] == ptr[3 * j] / 2 && ptr[3 * j + 1] == ptr[3 * j + 2])
			{
				ptrmask[3 * j] = 255;
				ptrmask[3 * j + 1] = 0;
				ptrmask[3 * j + 2] = 0;
			}// id 为 7 的时候或者 id 为 0 的时候，显示黑色
			else if (ptr[3 * j] == ptr[3 * j + 1] && ptr[3 * j] == ptr[3 * j + 2])
			{
				ptrmask[3 * j] = 0;
				ptrmask[3 * j + 1] = 0;
				ptrmask[3 * j + 2] = 0;
			}
			else
			{
				cout << "如果输出这段代码，说明有遗漏的情况，请查询getTheContour函数" << endl;
			}
		}
	}
}

void addImage(Mat image, Mat logoImage, int x, int y)
{
	// 先复制一份原版
	Mat imageClone = image.clone();
	// 单通道初始化遮罩
	Mat mask = Mat::zeros(logoImage.size(), CV_8UC1);
	// 颜色转换：三通道转单通道
	cvtColor(logoImage, mask, CV_BGR2GRAY);
	// 激活一个操作区域
	Mat imageROI = image(Range(y, logoImage.rows + y), Range(x, logoImage.cols + x));
	// 根据遮罩复制到原图
	logoImage.copyTo(imageROI, mask);
	// 在备份上激活一个操作区域
	Mat imageCloneROI = imageClone(Range(y, logoImage.rows + y), Range(x, logoImage.cols + x));
	// 将备份高亮区域与原图进行透明度混合
	addWeighted(imageROI, 0.5, imageCloneROI, 0.5, 0., imageROI);
}

MyMain::MyMain(void)
{	// 深度图初始化 240 * 320
	depthImage.create(240, 320, CV_8UC3);
	// 有色图初始化 480 * 640
	colorImage.create(480, 640, CV_8UC3);
	// 计时器实例化
	mt = new MyTimer(this);
	// 脚本器实例化
	script = new Script(10,5);
	// 身体数据初始化
	armShow = bellyShow = legShow = heightShow = 0;
	// 骨骼点初始化
	for (int i = 0; i < NUI_SKELETON_COUNT; i++)
		for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)
			skeletonPoint[i][j] = cvPoint(0, 0);
	// 有色摄像头事件，深度摄像头事件，骨骼点事件初始化
	colorEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	depthEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	skeletonEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	// 有色图像流，深度图像流初始化
	colorStreamHandle = NULL;
	depthStreamHandle = NULL;

	
	hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON);
	if (hr != S_OK) 
	{ 
		oss << "未连接 KINECT" << " (NuiInitialize failed)";
		wrong = oss.str();
		return;
	}
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, NULL, 4, colorEvent, &colorStreamHandle);
	if (hr != S_OK)
	{ 
		oss << "主摄像头开启失败" << " (Open the color Stream failed)";
		wrong = oss.str();
		NuiShutdown();
		return;
	}
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240, NULL, 2, depthEvent, &depthStreamHandle);
	if (hr != S_OK) 
	{
		oss << "深度摄像头开启失败" << " (Open the depth Stream failed)";
		wrong = oss.str();
		NuiShutdown();
		return;
	}
	hr = NuiSkeletonTrackingEnable(skeletonEvent, 0);//打开骨骼跟踪事件   
	if (hr != S_OK) 
	{ 
		oss << "骨骼信息开启失败" << " (NuiSkeletonTrackingEnable failed)";
		wrong = oss.str();
		NuiShutdown();
		return;
	}
	// 初始化人体框，id 偏移
	r = rect + 1;
	// 初始化 id
	id = -1;
	// 计时器开始计时
	mt->Start();
	// fixed 是设置为定点输出格式
	// floatfield 是设置输出时按浮点格式，小数点后有6位数字
	oss.setf(ios_base::fixed, ios_base::floatfield);
	// 设置小数点为两位
	oss.precision(2);

}

void MyMain::TikSink(long time)
{
	cout << "time: " << time << endl;
	// 将数据流的中值赋予数据显示值
	armShow = arm.Avg();
	bellyShow = belly.Avg();
	legShow = leg.Avg();
	heightShow = height.Avg();
	// 数据流情况
	arm.clear();
	belly.clear();
	leg.clear();
	height.clear();
}

void MyMain::getColorImage()
{
	// 初始化 colorFrame
	const NUI_IMAGE_FRAME *colorFrame = NULL;
	// 从 Nui 的流中获取下一帧，并锁定
	NuiImageStreamGetNextFrame(colorStreamHandle, 0, &colorFrame);
	INuiFrameTexture *pTexture = colorFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);
	// 如果有下一帧
	if (LockedRect.Pitch != 0)
	{	// 遍历 colorImage 行
		for (int i = 0; i < colorImage.rows; i++)
		{	// 初始化 Mat 第 i 行的指针
			uchar *ptr = colorImage.ptr<uchar>(i);
			// 初始化 NUI 第 i 行的指针
			uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
			// 遍历 colorImage 的列
			for (int j = 0; j < colorImage.cols; j++)
			{	//内部数据是4个字节，0-1-2是BGR，第4个现在未使用 
				ptr[3 * j] = pBuffer[4 * j];  
				ptr[3 * j + 1] = pBuffer[4 * j + 1];
				ptr[3 * j + 2] = pBuffer[4 * j + 2];
			}
		}
		// 如果是面朝摄像机，获取手臂，大腿，腹部信息
		if (script->isFaceMeasure())
		{
			script->arm = armShow;
			script->leg = legShow / 2;
			script->belly1 = bellyShow;
		}
		// 如果是侧对摄像机，获取身高，腹部信息（其中 1.03 是为了弥补红外线对人头发的识别不清晰导致的误差，经验系数）
		if (script->isSideMeasure())
		{
			script->belly2 = bellyShow;
			script->height = heightShow * 1.03;
		}
	}
	else cout << "捕捉色彩图像出现错误" << endl;
	// 解除锁定，释放这一帧
	pTexture->UnlockRect(0);
	NuiImageStreamReleaseFrame(colorStreamHandle, colorFrame);
	// 初始化人物有色填充图和其放大版（为适应深度摄像机和有色摄像机不同的分辨率）
	Mat bodyColor;
	bodyColor.create(240, 320, CV_8UC3);
	Mat bigBodyColor;
	bigBodyColor.create(480, 640, CV_8UC3);
	// 人物有色填充图 背景设置为全黑
	bodyColor.setTo(0);
	// 从深度相机获得带人物填充的图
	getTheContour(depthImage, id, bodyColor);
	// 放大，其中 1.9 为两摄像头的误差（经验系数）
	resize(bodyColor, bigBodyColor, Size(0, 0), 1.9, 1.9, CV_INTER_NN);
	// 将放大版加到 colorIamge 上
	addImage(colorImage, bigBodyColor, 0, 24);
}

void MyMain::getDepthImage()
{	// 遍历人物 id，初始化人物矩形
	for (int i = -1; i < 7; i++)
		r[i] = Rect(-1, -1, 0, 0);
	// 初始化 depthFrame
	const NUI_IMAGE_FRAME *depthFrame = NULL;
	// 从 Nui 的流中获取下一帧，并锁定
	NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame);
	INuiFrameTexture *pTexture = depthFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);
	// 初始化有色点 q
	RGBQUAD q;
	// 如果获得下一帧
	if (LockedRect.Pitch != 0)
	{	// 遍历 depthImage 的行
		for (int i = 0; i < depthImage.rows; i++)
		{	// 初始化 Mat 第 i 行的指针
			uchar *ptr = depthImage.ptr<uchar>(i);
			// 初始化 NUI 第 i 行的指针
			uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
			// 类型转变方便位运算
			USHORT *pBufferRun = (USHORT*)pBuffer;
			// 遍历 depthImage 的列
			for (int j = 0; j < depthImage.cols; j++)
			{	// 从后 3 位获得 player
				int player = pBufferRun[j] & 7;
				// 从前 13 位获得深度数据
				int data = (pBufferRun[j] & 0xfff8) >> 3;
				// 图形重新映射到 16 位
				uchar imageData = 255 - (uchar)(256 * data / 0x0fff);
				// 初始化有色点 q 的三种颜色
				q.rgbBlue = q.rgbGreen = q.rgbRed = 0;
				// 根据 player 给深度图上多种颜色，并保留一定的深度信息
				switch (player)
				{	// 该规则与 getTheContour() 一一对应
				case 0:
					q.rgbBlue = imageData / 2;
					q.rgbGreen = imageData / 2;
					q.rgbRed = imageData / 2;
					break;
				case 1:
					q.rgbRed = imageData;
					break;
				case 2:
					q.rgbGreen = imageData;
					break;
				case 3:
					q.rgbBlue = imageData;
					q.rgbGreen = imageData;
					q.rgbRed = imageData / 4;
					break;
				case 4:
					q.rgbBlue = imageData / 4;
					q.rgbGreen = imageData;
					q.rgbRed = imageData;
					break;
				case 5:
					q.rgbBlue = imageData;
					q.rgbGreen = imageData / 4;
					q.rgbRed = imageData;
					break;
				case 6:
					q.rgbBlue = imageData;
					q.rgbGreen = imageData / 2;
					q.rgbRed = imageData / 2;
					break;
				case 7:
					q.rgbBlue = 255 - (imageData / 2);
					q.rgbGreen = 255 - (imageData / 2);
					q.rgbRed = 255 - (imageData / 2);
				}
				ptr[3 * j] = q.rgbBlue;
				ptr[3 * j + 1] = q.rgbGreen;
				ptr[3 * j + 2] = q.rgbRed;
				// 从 buffer中获取 id 需要 -1 与骨骼 id 相对应，有效值为 0 ~ 6

				Point p = Point(j, i);
				// 更新人体矩形
				updateRectangle(r, p, player - 1);
				// 更新身体数据
				bodyDate.updateBody(p, player - 1);
			}
		}
		// 最大的矩形初始化（通过筛选出最大的人体矩形，确定 id ）
		Rect max = Rect(0, 0, 0, 0);
		// 遍历有效 id 
		for (int i = 0; i < 6; i++)
			if (r[i].area() > max.area())
			{
				id = i;
				max = r[i];
			}
		// 身体数据填充到身体数据流
		height.append(r[id].height);
		arm.append(bodyDate.getDate("arm", id));
		belly.append(bodyDate.getDate("belly", id));
		leg.append(bodyDate.getDate("leg", id));
	}
	else
	{
		cout << "捕捉深度图像出现错误" << endl;
	}
	// 解除锁定，释放这一帧
	pTexture->UnlockRect(0);
	NuiImageStreamReleaseFrame(depthStreamHandle, depthFrame);
}

int MyMain::Frame()
{	// 如果 hr 有问题，返回false
	if (hr != S_OK) return false;
	// 如果有骨骼事件
	if (WaitForSingleObject(skeletonEvent, INFINITE) == 0)
	{	// 设置骨骼点图，将骨骼点图应用到身体数据
		getSkeletonImage();
		bodyDate = BodyDate(skeletonPoint);
	}// 设置有色图和深度图
	if (WaitForSingleObject(depthEvent, 0) == 0) getDepthImage();
	if (WaitForSingleObject(colorEvent, 0) == 0) getColorImage();
	return true;
}

MyMain::~MyMain()
{
	delete(script);
	delete(mt);
}


/* TODO
 * 用户可以获得的功能
 * 模式一（auto）：自动完成拍摄正面和侧面（缺省参数10，5）
 * 模式二（手动）：每次按下一步进行
 * 模式三（全自动）：自动循环测量
 *
 * 流程图  * 正视图
 *        |
 *        * 侧视图
 *        |
 *        * 测量完成
 *
 * [范例文件][导入][导出]
 */
