#ifndef CCYUVDATADEFINE_H
#define CCYUVDATADEFINE_H


#include <stdio.h>
#include <stdint.h>

// 1 zijie duiqi
#pragma pack(push, 1)

#define MAX_AUDIO_FRAME_IN_QUEUE 1200
#define MAX_VIDOE_FRAME_IN_QUEUE 600

typedef struct H264FrameDef
{
    unsigned int length;
    unsigned char *dataBuffer;
} stCCYUVFrame;

typedef struct H264YUVDef
{
    unsigned int width;
    unsigned int height;
    stCCYUVFrame luma;
    stCCYUVFrame chromoB;
    stCCYUVFrame chromoR;
    long long pts;
}stH264YUV_Frame;

typedef struct DecodedAudioDataDef
{
    unsigned char *dataBuffer;
    unsigned int dataLenght;
}JCDecodeAudioData;

#pragma pack(pop)

#endif // CCYUVDATADEFINE_H
