#include <windows.h>
#include <iostream> 
#include <NuiApi.h>
#include <opencv2/opencv.hpp>
 
using namespace std;
using namespace cv;
 
//����������ݵ�ÿһ�����أ��������ͬһ���û���ID����ô���ؾͱ�Ϊͬ����ɫ����ͬ���û���
//��ID��һ������ɫ�ı�ʾҲ��һ�������������ĳ���û������أ���ô�Ͳ���ԭ�������ֵ
RGBQUAD shortDepth2RGBquad( USHORT depthID )
{ 
	//ÿ���ع�16bit����Ϣ���������3λ��ID������׽�����˵�ID����ʣ�µ�13λ������Ϣ
    USHORT realDepth = (depthID & 0xfff8) >> 3; //��ȡ������Ϣ����13λ 
    USHORT player =  depthID & 0x07 ;  //��ȡID��Ϣ ����3λ
 
    //��Ϊ��ȡ����Ϣ�Ǿ�����Ϣ��Ϊ�˱�����ʾ�������һ��Ϊ0-255
    BYTE depth = 255 - (BYTE)(256*realDepth/0x0fff); 
 
    RGBQUAD q; 
    q.rgbRed = q.rgbBlue = q.rgbGreen = 0; 
 
    //RGB����ͨ����ֵ������ȵĻ������ǻҶȵ�
	//Kinectϵͳ�ܹ������ʶ������ǰ����6���������Ϣ����ͬһʱ�����ֻ��2����ҿɱ�׷�٣����������٣�
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
 
    //1����ʼ��NUI��ע��������DEPTH_AND_PLAYER_INDEX
    HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX); 
    if (FAILED(hr)) 
    { 
        cout<<"NuiInitialize failed"<<endl; 
		getchar();
        return hr; 
    } 
 
    //2�������¼���� 
	//������ȡ��һ֡���ź��¼����������KINECT�Ƿ���Կ�ʼ��ȡ��һ֡����
    HANDLE nextColorFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    HANDLE depthStreamHandle = NULL; //����ͼ���������ľ����������ȡ���� 
 
    //3����KINECT�豸�Ĳ�ɫͼ��Ϣͨ��������depthStreamHandle��������ľ�����Ա����Ժ��ȡ
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240, 
							0, 2, nextColorFrameEvent, &depthStreamHandle); 
    if( FAILED( hr ) )//�ж��Ƿ���ȡ��ȷ 
    { 
        cout<<"Could not open color image stream video"<<endl; 
        NuiShutdown();  
		getchar();
        return hr; 
    }
	namedWindow("depthImage", CV_WINDOW_AUTOSIZE);
 
    //4����ʼ��ȡ������� 
    while(1) 
    { 
        const NUI_IMAGE_FRAME * pImageFrame = NULL; 
 
		//4.1�����޵ȴ��µ����ݣ��ȵ��󷵻�
        if (WaitForSingleObject(nextColorFrameEvent, INFINITE)==0) 
        { 
			//4.2���ӸղŴ���������������еõ���֡���ݣ���ȡ�������ݵ�ַ����pImageFrame
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
 
			//4.3����ȡ����֡��LockedRect���������������ݶ���pitchÿ���ֽ�����pBits��һ���ֽڵ�ַ
			//���������ݣ����������Ƕ����ݵ�ʱ��kinect�Ͳ���ȥ�޸���
            pTexture->LockRect(0, &LockedRect, NULL, 0); 
			//4.4��ȷ�ϻ�õ������Ƿ���Ч
            if( LockedRect.Pitch != 0 ) 
            { 
				//4.5��������ת��ΪOpenCV��Mat��ʽ
				for (int i=0; i<image.rows; i++) 
                {
					uchar *ptr = image.ptr<uchar>(i);  //��i�е�ָ��
					
					//����Ǽȱ�ʾ���ֵ�ֺ���������ţ�������ֵ�ĸ�13λ���������ֵ������λ�����û���ţ�
					//ע��������Ҫת������Ϊÿ��������2���ֽڣ��洢��ͬ�������ɫ��Ϣ��һ����
                    uchar *pBufferRun = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
					USHORT * pBuffer = (USHORT*) pBufferRun;
					 
                    for (int j=0; j<image.cols; j++) 
                    {
						//����ÿһ�����أ�����ͨ�������������ȥ�޸�����RGBֵ��
						RGBQUAD rgb = shortDepth2RGBquad(pBuffer[j]);
                        ptr[3*j] = rgb.rgbBlue; 
                        ptr[3*j+1] = rgb.rgbGreen; 
                        ptr[3*j+2] = rgb.rgbRed; 
                    } 
				} 
                imshow("depthImage", image); //��ʾͼ�� 
            } 
            else 
            { 
                cout<<"Buffer length of received texture is bogus\r\n"<<endl; 
            }
 
			//5����֡�Ѿ��������ˣ����Խ������
			pTexture->UnlockRect(0);
            //6���ͷű�֡���ݣ�׼��ӭ����һ֡ 
            NuiImageStreamReleaseFrame(depthStreamHandle, pImageFrame ); 
        } 
        if (cvWaitKey(20) == 27) 
            break; 
    } 
    //7���ر�NUI���� 
    NuiShutdown(); 
	return 0;
}
