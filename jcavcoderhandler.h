#ifndef JCAVCODERHANDLER_H
#define JCAVCODERHANDLER_H

#include <QObject>
#include <QWidget>
#include <QtDebug>
#include <mutex>
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include "JCDataDefine.h"

#ifdef __cplusplus
extern "C" {
#endif

    #include "3rdparty/linux/libffmpeg/include/libavcodec/avcodec.h"
    #include "3rdparty/linux/libffmpeg/include/libavformat/avformat.h"
    #include "3rdparty/linux/libffmpeg/include/libswscale/swscale.h"
    #include "3rdparty/linux/libffmpeg/include/libswresample/swresample.h"
    #include "3rdparty/linux/libffmpeg/include/libavutil/imgutils.h"

#ifdef __cplusplus
}
#endif

enum MediaPlayStatus
{
    MEDIAPLAY_STATUS_PLAYING = 0,
    MEDIAPLAY_STATUS_PAUSE,
    MEDIAPLAY_STATUS_SEEK,
    MEDIAPLAY_STATUS_STOP,
};


class JCAVCoderHandler
{
private:
    QString m_videoPathString = "";
    int m_videoWidth = 0;
    int m_videoHeight = 0;

    int m_videoStreamIdx = -1;
    int m_audioStreamIdx = -1;

    bool m_bReadFileEOF = false;

    MediaPlayStatus m_eMediaStatus;

    AVFormatContext *m_pformatCtx = NULL;

    AVCodecContext  *m_pVideoCodecCtx = NULL; //baocun bianmaqixinxi
    AVCodecContext  *m_pAudioCodecCtx = NULL;

    AVFrame * m_pYUVFrame = NULL;
    AVFrame * m_pVideoFrame = NULL;
    AVFrame * m_pAudioFrame = NULL;

    AVRational       m_vStreamTimeRational;
    AVRational       m_aStreamTimeRational;

    JCTSQueue<AVPacket*> m_audioPktQue;
    JCTSQueue<AVPacket*> m_videoPktQue;

    void startMediaProcessThreads(); // chuangjian xiancheng

    void doReadMediaFrameThread();
    void doAudioDecodeThread();
    void doVideoDecodeThread();

    void readMediaPacket();
    void freePacket(AVPacket *packet);

    void stdThreadSleep(int mseconds);

public:
    JCAVCoderHandler();
    ~JCAVCoderHandler();

    void SetVideoFilePath(const QString &path);
    QString GetVideoFilePath();

    QSize GetMediaWithHeigth();

    int IntVideoCodec();
    void UnInitVideoCode();

    // kaishi yu tingzhi bofang
    void StartPlayVideo();
    void StopPlayVideo();
};

#endif // JCAVCODERHANDLER_H






























