﻿#include <windows.h>
#include <iostream> 
#include <NuiApi.h>
#include <opencv2/opencv.hpp>
#include "cv.h"                             //  OpenCV 文件头

#include "highgui.h"

#include "cvaux.h"

#include "cxcore.h"
//#include "opencv2/imgproc.hpp"
 
using namespace std;
using namespace cv;
 
bool tracked[NUI_SKELETON_COUNT]={FALSE}; 
CvPoint skeletonPoint[NUI_SKELETON_COUNT][NUI_SKELETON_POSITION_COUNT]={cvPoint(0,0)}; 
CvPoint colorPoint[NUI_SKELETON_COUNT][NUI_SKELETON_POSITION_COUNT]={cvPoint(0,0)}; 
 
void getColorImage(HANDLE &colorEvent, HANDLE &colorStreamHandle, Mat &colorImage); 
void getDepthImage(HANDLE &depthEvent, HANDLE &depthStreamHandle, Mat &depthImage); 
void getSkeletonImage(HANDLE &skeletonEvent, Mat &skeletonImage, Mat &colorImage, Mat &depthImage); 
void drawSkeleton(Mat &image, CvPoint pointSet[], int witchone); 
void getTheContour1(Mat &image, int whichone, Mat &mask);
 
int main1(int argc, char *argv[])
{
	Mat colorImage;
	colorImage.create(480, 640, CV_8UC3); 
	Mat depthImage;
	depthImage.create(240, 320, CV_8UC3); 
	Mat skeletonImage;
	skeletonImage.create(240, 320, CV_8UC3); 
	Mat mask;
	mask.create(240, 320, CV_8UC3); 
	Mat bigMask;
	bigMask.create(480, 640, CV_8UC3); 
 
    HANDLE colorEvent = CreateEvent( NULL, TRUE, FALSE, NULL ); 
    HANDLE depthEvent = CreateEvent( NULL, TRUE, FALSE, NULL ); 
    HANDLE skeletonEvent = CreateEvent( NULL, TRUE, FALSE, NULL ); 
 
    HANDLE colorStreamHandle = NULL; 
    HANDLE depthStreamHandle = NULL; 
 
    HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON);   
    if( hr != S_OK )   
    {   
        cout<<"NuiInitialize failed"<<endl;   
        return hr;   
    } 
 
    hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, NULL, 4, colorEvent, &colorStreamHandle); 
    if( hr != S_OK )   
    {   
        cout<<"Open the color Stream failed"<<endl; 
        NuiShutdown(); 
        return hr;   
    } 
    hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240, NULL, 2, depthEvent, &depthStreamHandle); 
    if( hr != S_OK )   
    {   
        cout<<"Open the depth Stream failed"<<endl; 
        NuiShutdown(); 
        return hr;   
    } 
    hr = NuiSkeletonTrackingEnable( skeletonEvent, 0 );//打开骨骼跟踪事件   
    if( hr != S_OK )   
    {   
        cout << "NuiSkeletonTrackingEnable failed" << endl;   
        NuiShutdown(); 
        return hr;   
    } 
  
    namedWindow("mask", CV_WINDOW_AUTOSIZE);
	namedWindow("colorImage", CV_WINDOW_AUTOSIZE);
	namedWindow("depthImage", CV_WINDOW_AUTOSIZE);
	namedWindow("skeletonImage", CV_WINDOW_AUTOSIZE);
	
    while (1) 
    { 
        if(WaitForSingleObject(colorEvent, 0)==0) 
            getColorImage(colorEvent, colorStreamHandle, colorImage); 
        if(WaitForSingleObject(depthEvent, 0)==0) 
            getDepthImage(depthEvent, depthStreamHandle, depthImage); 
		//这里使用INFINITE是为了避免没有激活skeletonEvent而跳过此代码出现colorimage频闪的现象 
        if(WaitForSingleObject(skeletonEvent, INFINITE)==0)
            getSkeletonImage(skeletonEvent, skeletonImage, colorImage, depthImage); 
          
        for (int i=0; i<6; i++)  
        { 
            if(tracked[i] == TRUE) 
            { 
                mask.setTo(0);
                getTheContour1(depthImage, i, mask); 
                tracked[i] = FALSE; 
                break; 
            } 
        } 

		//处理mask和colorImage
		//double k = 0.9;
		//Size s = Size((int)(colorImage.size().width * k), (int)(colorImage.size().height * k));
		resize(mask, bigMask, Size(0, 0), 1.9, 1.9, CV_INTER_NN);
		//cvRectangle( &bigMask, cvPoint(100, 100), cvPoint(200, 200), cvScalar(0, 0, 255), 3, 4, 0 );   
		addWeighted(colorImage(Rect(0, 24, bigMask.cols,bigMask.rows)),0.5,bigMask,0.5,0.0,colorImage(Rect(0, 24, bigMask.cols,bigMask.rows)));

		//Mat imageROI;  
       //imageROI=srcImage4(Rect(200,250,logoImage.cols,logoImage.rows));
		//




 
        imshow("mask", mask); 
        imshow("bigMask", bigMask); 
        imshow("colorImage", colorImage); 
        imshow("depthImage", depthImage); 
        imshow("skeletonImage", skeletonImage); 
 
        if(cvWaitKey(1)==27) 
            break; 
    } 
 
    NuiShutdown(); 
    return 0; 
}
 
 
void getColorImage(HANDLE &colorEvent, HANDLE &colorStreamHandle, Mat &colorImage) 
{ 
    const NUI_IMAGE_FRAME *colorFrame = NULL; 
 
    NuiImageStreamGetNextFrame(colorStreamHandle, 0, &colorFrame); 
    INuiFrameTexture *pTexture = colorFrame->pFrameTexture;   
 
    NUI_LOCKED_RECT LockedRect; 
    pTexture->LockRect(0, &LockedRect, NULL, 0);   
 
    if( LockedRect.Pitch != 0 ) 
    { 
		for (int i=0; i<colorImage.rows; i++) 
        {
			uchar *ptr = colorImage.ptr<uchar>(i);  //第i行的指针					
			//每个字节代表一个颜色信息，直接使用uchar
            uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
            for (int j=0; j<colorImage.cols; j++) 
            { 
                ptr[3*j] = pBuffer[4*j];  //内部数据是4个字节，0-1-2是BGR，第4个现在未使用 
                ptr[3*j+1] = pBuffer[4*j+1]; 
                ptr[3*j+2] = pBuffer[4*j+2]; 
            } 
		} 
    } 
    else 
    { 
        cout<<"捕捉色彩图像出现错误"<<endl; 
    }
 
	pTexture->UnlockRect(0); 
    NuiImageStreamReleaseFrame(colorStreamHandle, colorFrame );
} 
 
void getDepthImage(HANDLE &depthEvent, HANDLE &depthStreamHandle, Mat &depthImage) 
{ 
    const NUI_IMAGE_FRAME *depthFrame = NULL; 
 
    NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame); 
    INuiFrameTexture *pTexture = depthFrame->pFrameTexture;   
 
    NUI_LOCKED_RECT LockedRect; 
    pTexture->LockRect(0, &LockedRect, NULL, 0);   
 
    RGBQUAD q; 
 
    if( LockedRect.Pitch != 0 ) 
    { 
        for (int i=0; i<depthImage.rows; i++) 
        { 
            uchar *ptr = depthImage.ptr<uchar>(i); 
            uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
			USHORT *pBufferRun = (USHORT*) pBuffer; 
			
            for (int j=0; j<depthImage.cols; j++) 
            { 
                int player = pBufferRun[j]&7; 
                int data = (pBufferRun[j]&0xfff8) >> 3; 
                 
                uchar imageData = 255-(uchar)(256*data/0x0fff); 
                q.rgbBlue = q.rgbGreen = q.rgbRed = 0; 
 
                switch(player) 
                { 
					case 0:   
						q.rgbRed = imageData / 2;   
						q.rgbBlue = imageData / 2;   
						q.rgbGreen = imageData / 2;   
						break;   
					case 1:    
						q.rgbRed = imageData;   
						break;   
					case 2:   
						q.rgbGreen = imageData;   
						break;   
					case 3:   
						q.rgbRed = imageData / 4;   
						q.rgbGreen = q.rgbRed*4;  //这里利用乘的方法，而不用原来的方法可以避免不整除的情况 
						q.rgbBlue = q.rgbRed*4;  //可以在后面的getTheContour()中配合使用，避免遗漏一些情况 
						break;   
					case 4:   
						q.rgbBlue = imageData / 4;  
						q.rgbRed = q.rgbBlue*4;   
						q.rgbGreen = q.rgbBlue*4;   
						break;   
					case 5:   
						q.rgbGreen = imageData / 4;  
						q.rgbRed = q.rgbGreen*4;   
						q.rgbBlue = q.rgbGreen*4;   
						break;   
					case 6:   
						q.rgbRed = imageData / 2;   
						q.rgbGreen = imageData / 2;    
						q.rgbBlue = q.rgbGreen*2;   
						break;   
					case 7:   
						q.rgbRed = 255 - ( imageData / 2 );   
						q.rgbGreen = 255 - ( imageData / 2 );   
						q.rgbBlue = 255 - ( imageData / 2 ); 
				} 	 
				ptr[3*j] = q.rgbBlue; 
				ptr[3*j+1] = q.rgbGreen; 
				ptr[3*j+2] = q.rgbRed; 
            } 
        } 
    } 
    else 
    { 
        cout << "捕捉深度图像出现错误" << endl; 
    } 
	
	pTexture->UnlockRect(0);
    NuiImageStreamReleaseFrame(depthStreamHandle, depthFrame);  
} 
 
void getSkeletonImage(HANDLE &skeletonEvent, Mat &skeletonImage, Mat &colorImage, Mat &depthImage) 
{ 
    NUI_SKELETON_FRAME skeletonFrame = {0};
    bool bFoundSkeleton = false;  
 
    if(NuiSkeletonGetNextFrame( 0, &skeletonFrame ) == S_OK )   
    {   
        for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )   
        {   
            if( skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED ) 
            {   
                bFoundSkeleton = true;   
                break; 
            }   
        }   
    } 
    else 
    { 
        cout << "没有找到合适的骨骼帧." << endl; 
        return;  
    } 
 
    if( !bFoundSkeleton )   
    {   
        return;  
    }   
 
    NuiTransformSmooth(&skeletonFrame, NULL);//平滑骨骼帧,消除抖动   
    skeletonImage.setTo(0);   
	
    for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )   
    {   
        if( skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&   
            skeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED)   
        {   
            float fx, fy;   
 
            for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)//所有的坐标转化为深度图的坐标   
            {   
                NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[j], &fx, &fy );   
                skeletonPoint[i][j].x = (int)fx;   
                skeletonPoint[i][j].y = (int)fy;   
            }   
 
            for (int j=0; j<NUI_SKELETON_POSITION_COUNT ; j++)   
            {   
                if (skeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[j] != NUI_SKELETON_POSITION_NOT_TRACKED)//跟踪点一用有三种状态：1没有被跟踪到，2跟踪到，3根据跟踪到的估计到   
                {   
                    LONG colorx, colory; 
                    NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, 0,  
                        skeletonPoint[i][j].x, skeletonPoint[i][j].y, 0,&colorx, &colory); 
                    colorPoint[i][j].x = int(colorx);
					colorPoint[i][j].y = int(colory); //存储坐标点 
                    circle(colorImage, colorPoint[i][j], 4, cvScalar(0, 255, 255), 1, 8, 0); 
                    circle(skeletonImage, skeletonPoint[i][j], 3, cvScalar(0, 255, 255), 1, 8, 0); 
 
                    tracked[i] = TRUE; 
                } 
            } 
 
            drawSkeleton(colorImage, colorPoint[i], i);  
            drawSkeleton(skeletonImage, skeletonPoint[i], i); 
        } 
    }   
} 
 
void drawSkeleton(Mat &image, CvPoint pointSet[], int whichone) 
{ 
    CvScalar color; 
    switch(whichone) //跟踪不同的人显示不同的颜色 
    { 
    case 0: 
        color = cvScalar(255); 
        break; 
    case 1: 
        color = cvScalar(0,255); 
        break; 
    case 2: 
        color = cvScalar(0, 0, 255); 
        break; 
    case 3: 
        color = cvScalar(255, 255, 0); 
        break; 
    case 4: 
        color = cvScalar(255, 0, 255); 
        break; 
    case 5: 
        color = cvScalar(0, 255, 255); 
        break; 
    } 
 
    if((pointSet[NUI_SKELETON_POSITION_HEAD].x!=0 || pointSet[NUI_SKELETON_POSITION_HEAD].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_HEAD], pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_SPINE].x!=0 || pointSet[NUI_SKELETON_POSITION_SPINE].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER], pointSet[NUI_SKELETON_POSITION_SPINE], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_SPINE].x!=0 || pointSet[NUI_SKELETON_POSITION_SPINE].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_HIP_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_CENTER].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_SPINE], pointSet[NUI_SKELETON_POSITION_HIP_CENTER], color, 2); 
 
    //左上肢 
    if((pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER], pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT], pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_WRIST_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_WRIST_LEFT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT], pointSet[NUI_SKELETON_POSITION_WRIST_LEFT], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_WRIST_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_WRIST_LEFT].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_HAND_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_HAND_LEFT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_WRIST_LEFT], pointSet[NUI_SKELETON_POSITION_HAND_LEFT], color, 2); 
 
    //右上肢 
    if((pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER], pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT], pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT], pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_HAND_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_HAND_RIGHT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT], pointSet[NUI_SKELETON_POSITION_HAND_RIGHT], color, 2); 
 
    //左下肢 
    if((pointSet[NUI_SKELETON_POSITION_HIP_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_CENTER].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_HIP_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_LEFT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_HIP_CENTER], pointSet[NUI_SKELETON_POSITION_HIP_LEFT], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_HIP_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_LEFT].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_KNEE_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_KNEE_LEFT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_HIP_LEFT], pointSet[NUI_SKELETON_POSITION_KNEE_LEFT], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_KNEE_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_KNEE_LEFT].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_KNEE_LEFT], pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT].y!=0) &&  
        (pointSet[NUI_SKELETON_POSITION_FOOT_LEFT].x!=0 || pointSet[NUI_SKELETON_POSITION_FOOT_LEFT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT], pointSet[NUI_SKELETON_POSITION_FOOT_LEFT], color, 2); 
 
    //右下肢 
    if((pointSet[NUI_SKELETON_POSITION_HIP_CENTER].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_CENTER].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_HIP_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_RIGHT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_HIP_CENTER], pointSet[NUI_SKELETON_POSITION_HIP_RIGHT], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_HIP_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_HIP_RIGHT].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_HIP_RIGHT], pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT],color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT], pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT], color, 2); 
    if((pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT].y!=0) && 
        (pointSet[NUI_SKELETON_POSITION_FOOT_RIGHT].x!=0 || pointSet[NUI_SKELETON_POSITION_FOOT_RIGHT].y!=0)) 
        line(image, pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT], pointSet[NUI_SKELETON_POSITION_FOOT_RIGHT], color, 2); 
} 
 
 //根据给定的深度数据的关系（在getDepthImage()中的）确定不同的跟踪目标 
void getTheContour1(Mat &image, int whichone, Mat &mask)
{ 
    for (int i=0; i<image.rows; i++) 
    { 
		uchar *ptr = image.ptr<uchar>(i); 
        uchar *ptrmask = mask.ptr<uchar>(i);  
        for (int j=0; j<image.cols; j++) 
        { 
            if (ptr[3*j]==0 && ptr[3*j+1]==0 && ptr[3*j+2]==0) //都为0的时候予以忽略 
            { 
                ptrmask[3*j]=ptrmask[3*j+1]=ptrmask[3*j+2]=0; 
            }else if(ptr[3*j]==0 && ptr[3*j+1]==0 && ptr[3*j+2]!=0)//ID为1的时候，显示绿色 
            { 
                ptrmask[3*j] = 0; 
                ptrmask[3*j+1] = 255; 
                ptrmask[3*j+2] = 0; 
            }else if (ptr[3*j]==0 && ptr[3*j+1]!=0 && ptr[3*j+2]==0)//ID为2的时候，显示红色 
            { 
                ptrmask[3*j] = 0; 
                ptrmask[3*j+1] = 0; 
                ptrmask[3*j+2] = 255; 
            }else if (ptr[3*j]==ptr[3*j+1] && ptr[3*j]==4*ptr[3*j+2])//ID为3的时候 
            { 
                ptrmask[3*j] = 255; 
                ptrmask[3*j+1] = 255; 
                ptrmask[3*j+2] = 0; 
            }else if (4*ptr[3*j]==ptr[3*j+1] && ptr[3*j+1]==ptr[3*j+2])//ID为4的时候 
            { 
                ptrmask[3*j] = 255; 
                ptrmask[3*j+1] = 0; 
                ptrmask[3*j+2] = 255; 
            }else if (ptr[3*j]==4*ptr[3*j+1] && ptr[3*j]==ptr[3*j+2])//ID为5的时候 
            { 
                ptrmask[3*j] = 0; 
                ptrmask[3*j+1] = 255; 
                ptrmask[3*j+2] = 255; 
            }else if (ptr[3*j]==2*ptr[3*j+1] && ptr[3*j+1]==ptr[3*j+2])//ID为6的时候 
            { 
                ptrmask[3*j] = 255; 
                ptrmask[3*j+1] = 0; 
                ptrmask[3*j+2] = 0; 
            }else if (ptr[3*j]==ptr[3*j+1] && ptr[3*j]==ptr[3*j+2])//ID为7的时候或者ID为0的时候，显示白色 
            { 
                ptrmask[3*j] = 255; 
                ptrmask[3*j+1] = 255; 
                ptrmask[3*j+2] = 255; 
            }else 
            { 
                cout <<"如果输出这段代码，说明有遗漏的情况，请查询getTheContour函数" << endl; 
            } 
        } 
    } 
}
