/*
 * Based on Cube Eye's Simple Viewer
 * http://www.cube-eye.co.kr/data/Cube_Eye_v2.6_Win_11_Jun_2018.zip
 */

#include <stdio.h>

typedef unsigned long long ULONGLONG;

typedef struct _mtfIntrinsicParam//Camera Intrinsic Parameter
{
        float fFx;      //Focal length X
        float fFy;      //Focal length Y
        float fCx;      //Principal point X
        float fCy;      //Principal point Y

} mtfIntrinsicParam;//lens intrinsic parameter

typedef struct _mtfDistortionParam//Camera Distortion Parameter
{
        float fK1;      //Distortion K1
        float fK2;      //Distortion K2
        float fK3;      //Distortion K3
        float fP1;      //Distortion P1
        float fP2;      //Distortion P2
        float fSkew;

} mtfDistortionParam;//lens distortion parameter

typedef struct _stFRAMEHEADER
{
        int nWidth;
        int nHeight;
        int nRGBWidth;
        int nRGBHeight;
        int nFrames;
        int nBufSize;
        int nRGBBufSize;
        int nDeviceType;//0: Only IR,Depth, 1:Depth,IR + RGB888

        mtfIntrinsicParam stIntrinsicParam;
        mtfDistortionParam stDistortionParam;

} stFrameHeader;

stFrameHeader   m_stFrameHeader;//파일 헤더 구조체
FILE            *m_pLoadFile;

unsigned short  *m_pCameraBuf[2];//[0]:Amplitude Data, [1]: Depth Data
unsigned char   *m_pRGBCameraBuf;
unsigned char   *m_pRGBCameraBuf_re;

long            m_nFrameIndex;  //Frame Index [저장될 당시  Frame Index]
ULONGLONG       m_nTimestamp;   //Time Stamp(unit: ms)

//RGB Model
long            m_nRGBFrameIndex;
ULONGLONG       m_nRGBTimestamp;
