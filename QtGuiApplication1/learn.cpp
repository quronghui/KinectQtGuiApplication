#include <windows.h>
#include <iostream> 
#include <NuiApi.h>
#include <opencv2/opencv.hpp>
 
using namespace std;
using namespace cv;
 
//处理深度数据的每一个像素，如果属于同一个用户的ID，那么像素就标为同种颜色，不同的用户，
//其ID不一样，颜色的标示也不一样，如果不属于某个用户的像素，那么就采用原来的深度值
RGBQUAD shortDepth2RGBquad( USHORT depthID )
{ 
	//每像素共16bit的信息，其中最低3位是ID（所捕捉到的人的ID），剩下的13位才是信息
    USHORT realDepth = (depthID & 0xfff8) >> 3; //提取距离信息，高13位 
    USHORT player =  depthID & 0x07 ;  //提取ID信息 ，低3位
 
    //因为提取的信息是距离信息，为了便于显示，这里归一化为0-255
    BYTE depth = 255 - (BYTE)(256*realDepth/0x0fff); 
 
    RGBQUAD q; 
    q.rgbRed = q.rgbBlue = q.rgbGreen = 0; 
 
    //RGB三个通道的值都是相等的话，就是灰度的
	//Kinect系统能够处理辨识传感器前多至6个人物的信息，但同一时刻最多只有2个玩家可被追踪（即骨骼跟踪）
	switch( player ) 
    { 
		case 0:  
			q.rgbRed = depth / 2; 
			q.rgbBlue = depth / 2; 
			q.rgbGreen = depth / 2; 
			break; 
		case 1: 
			q.rgbRed = depth; 
			break; 
		case 2: 
			q.rgbGreen = depth;  
			break; 
		case 3: 
			q.rgbRed = depth / 4; 
			q.rgbGreen = depth; 
			q.rgbBlue = depth; 
			break; 
		case 4: 
			q.rgbRed = depth; 
			q.rgbGreen = depth; 
			q.rgbBlue = depth / 4; 
			break; 
		case 5: 
			q.rgbRed = depth; 
			q.rgbGreen = depth / 4; 
			q.rgbBlue = depth; 
			break; 
		case 6: 
			q.rgbRed = depth / 2; 
			q.rgbGreen = depth / 2; 
			q.rgbBlue = depth; 
			break; 
		case 7: 
			q.rgbRed = 255 - ( depth / 2 ); 
			q.rgbGreen = 255 - ( depth / 2 ); 
			q.rgbBlue = 255 - ( depth / 2 ); 
		} 
 
    return q; 
}
 
int main10(int argc, char *argv[])
{
	Mat image;
	image.create(240, 320, CV_8UC3); 
 
    //1、初始化NUI，注意这里是DEPTH_AND_PLAYER_INDEX
    HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX); 
    if (FAILED(hr)) 
    { 
        cout<<"NuiInitialize failed"<<endl; 
		getchar();
        return hr; 
    } 
 
    //2、定义事件句柄 
	//创建读取下一帧的信号事件句柄，控制KINECT是否可以开始读取下一帧数据
    HANDLE nextColorFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    HANDLE depthStreamHandle = NULL; //保存图像数据流的句柄，用以提取数据 
 
    //3、打开KINECT设备的彩色图信息通道，并用depthStreamHandle保存该流的句柄，以便于以后读取
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240, 
							0, 2, nextColorFrameEvent, &depthStreamHandle); 
    if( FAILED( hr ) )//判断是否提取正确 
    { 
        cout<<"Could not open color image stream video"<<endl; 
        NuiShutdown();  
		getchar();
        return hr; 
    }
	namedWindow("depthImage", CV_WINDOW_AUTOSIZE);
 
    //4、开始读取深度数据 
    while(1) 
    { 
        const NUI_IMAGE_FRAME * pImageFrame = NULL; 
 
		//4.1、无限等待新的数据，等到后返回
        if (WaitForSingleObject(nextColorFrameEvent, INFINITE)==0) 
        { 
			//4.2、从刚才打开数据流的流句柄中得到该帧数据，读取到的数据地址存于pImageFrame
            hr = NuiImageStreamGetNextFrame(depthStreamHandle, 0, &pImageFrame); 
			if (FAILED(hr))
			{
				cout<<"Could not get depth image"<<endl; 
				NuiShutdown(); 
				getchar();
				return -1;
			}
 
			INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
			NUI_LOCKED_RECT LockedRect;
 
			//4.3、提取数据帧到LockedRect，它包括两个数据对象：pitch每行字节数，pBits第一个字节地址
			//并锁定数据，这样当我们读数据的时候，kinect就不会去修改它
            pTexture->LockRect(0, &LockedRect, NULL, 0); 
			//4.4、确认获得的数据是否有效
            if( LockedRect.Pitch != 0 ) 
            { 
				//4.5、将数据转换为OpenCV的Mat格式
				for (int i=0; i<image.rows; i++) 
                {
					uchar *ptr = image.ptr<uchar>(i);  //第i行的指针
					
					//其二是既表示深度值又含有人物序号，则像素值的高13位保存了深度值，低三位保存用户序号，
					//注意这里需要转换，因为每个数据是2个字节，存储的同上面的颜色信息不一样，
                    uchar *pBufferRun = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
					USHORT * pBuffer = (USHORT*) pBufferRun;
					 
                    for (int j=0; j<image.cols; j++) 
                    {
						//对于每一个像素，我们通过它的深度数据去修改它的RGB值；
						RGBQUAD rgb = shortDepth2RGBquad(pBuffer[j]);
                        ptr[3*j] = rgb.rgbBlue; 
                        ptr[3*j+1] = rgb.rgbGreen; 
                        ptr[3*j+2] = rgb.rgbRed; 
                    } 
				} 
                imshow("depthImage", image); //显示图像 
            } 
            else 
            { 
                cout<<"Buffer length of received texture is bogus\r\n"<<endl; 
            }
 
			//5、这帧已经处理完了，所以将其解锁
			pTexture->UnlockRect(0);
            //6、释放本帧数据，准备迎接下一帧 
            NuiImageStreamReleaseFrame(depthStreamHandle, pImageFrame ); 
        } 
        if (cvWaitKey(20) == 27) 
            break; 
    } 
    //7、关闭NUI链接 
    NuiShutdown(); 
	return 0;
}
