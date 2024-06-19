#include "jcavcoderhandler.h"

#define MIN(x, y) ((x) > (y)) ? (y) : (x)

std::atomic<bool> m_bThreadRunning(false);
std::atomic<bool> m_bFileThreadRunning(false);
std::atomic<bool> m_bVideoThreadRunning(false);
std::atomic<bool> m_bAudioThreadRunning(false);

JCAVCoderHandler::JCAVCoderHandler()
    :m_videoPathString(""),
      m_videoWidth(0),
      m_videoHeight(0),
      m_videoStreamIdx(0),
      m_audioStreamIdx(0),
      m_pformatCtx(NULL),
      m_pVideoCodecCtx(NULL),
      m_pAudioCodecCtx(NULL)
{
    // register all ffmpeg
    avcodec_register_all();
}

JCAVCoderHandler::~JCAVCoderHandler()
{

}


void JCAVCoderHandler::SetVideoFilePath(const QString &path)
{
    m_videoPathString = path;
}

QString JCAVCoderHandler::GetVideoFilePath()
{
    return m_videoPathString;
}

QSize JCAVCoderHandler::GetMediaWithHeigth()
{
    return QSize(0, 0);
}

void JCAVCoderHandler::StartPlayVideo()
{
    startMediaProcessThreads();
}

void JCAVCoderHandler::StopPlayVideo()
{

}

void JCAVCoderHandler::stdThreadSleep(int mseconds)
{
    std::chrono::microseconds sleepTime(mseconds);
    std::this_thread::sleep_for(sleepTime);
}


void JCAVCoderHandler::freePacket(AVPacket *packet)
{
    if(packet == NULL)
    {
        return;
    }

    av_free_packet(packet);
    free(packet);
}

void JCAVCoderHandler::readMediaPacket()
{
    AVPacket * packet = (AVPacket *) malloc(sizeof (AVPacket));
    if(!packet)
    {
        return ;
    }

    av_init_packet(packet);

    m_eMediaStatus = MEDIAPLAY_STATUS_PLAYING;

    int reValue = av_read_frame(m_pformatCtx, packet);
    if(reValue == 0)
    {
        if(packet != NULL && packet->stream_index == m_videoStreamIdx)
        {
            if(!av_dup_packet(packet))
            {
                m_videoPktQue.enquue(packet);
            }
            else
            {
                // shi fang neicun
                freePacket(packet);
                packet = NULL;
            }
        }

        if(packet != NULL && packet->stream_index == m_audioStreamIdx)
        {
            if(!av_dup_packet(packet))
            {
                m_audioPktQue.enquue(packet);
            }
            else
            {
                // shi fang neicun
                freePacket(packet);
                packet = NULL;
            }
        }
    }
    else if(reValue < 0)
    {
        if(reValue == AVERROR_EOF) // wenjianduwanle
        {
            m_bReadFileEOF = true;
        }

        return;
    }

    return ;
}

// xuanlanshipin
void JCAVCoderHandler::convertAndRenderVideo(AVFrame* decodedFrame, long long ppts)// xuanlan
{
    if(decodedFrame == NULL)
    {
        return;
    }

    // jinxing genshizhuanhuan
    if(m_pVideoSwsCtx == NULL)
    {
        m_pVideoSwsCtx = sws_getContext(m_videoWidth, m_videoHeight, m_pVideoCodecCtx->pix_fmt,\
                                          m_pVideoCodecCtx->width, m_pVideoCodecCtx->height, AV_PIX_FMT_YUV420P, \
                                          SWS_BICUBIC, NULL, NULL, NULL);
    }

    sws_scale(m_pVideoSwsCtx, decodedFrame->data, decodedFrame->linesize,
              0, m_pVideoCodecCtx->height, m_pYUVFrame->data, m_pVideoFrame->linesize);

    // ffmpeg yiban 32zijieduiqi
    unsigned int lumaLength = m_pVideoCodecCtx->height * (MIN(m_pVideoFrame->linesize[0],m_pVideoCodecCtx->width));
    unsigned int chromBLength = m_pVideoCodecCtx->height * MIN(m_pVideoFrame->linesize[1],m_pVideoCodecCtx->width) / 4;
    unsigned int chromRLength = m_pVideoCodecCtx->height * MIN(m_pVideoFrame->linesize[2],m_pVideoCodecCtx->width) / 4;

    stH264YUV_Frame *updateYUVFrame = new stH264YUV_Frame();

    updateYUVFrame->width = m_pVideoCodecCtx->width;
    updateYUVFrame->height = m_pVideoCodecCtx->height;
    updateYUVFrame->pts = ppts;

    updateYUVFrame->luma.length = lumaLength;
    updateYUVFrame->chromoB.length = chromBLength;
    updateYUVFrame->chromoR.length = chromRLength;

    updateYUVFrame->luma.dataBuffer = (unsigned char*)malloc(lumaLength);
    updateYUVFrame->chromoB.dataBuffer = (unsigned char*)malloc(chromBLength);
    updateYUVFrame->chromoR.dataBuffer = (unsigned char*)malloc(chromRLength);

    copyDecodeFrame420(m_pYUVFrame->data[0], updateYUVFrame->luma.dataBuffer, m_pVideoFrame->linesize[0],
                            m_pVideoCodecCtx->width, m_pVideoCodecCtx->height);

    copyDecodeFrame420(m_pYUVFrame->data[1], updateYUVFrame->chromoB.dataBuffer, m_pVideoFrame->linesize[1],
                            m_pVideoCodecCtx->width / 2, m_pVideoCodecCtx->height / 2);

    copyDecodeFrame420(m_pYUVFrame->data[2], updateYUVFrame->chromoR.dataBuffer, m_pVideoFrame->linesize[2],
                            m_pVideoCodecCtx->width / 2, m_pVideoCodecCtx->height / 2);

}

void JCAVCoderHandler::copyDecodeFrame420(uint8_t *src, uint8_t *dst, int linesize, int width, int height)
{
    width = MIN(linesize, width);
    for(int i = 0; i < height; ++i)
    {
        memcpy(dst, src, width);// baduiqizhihou de youxiaoshuju kaobeidao dst
        dst += width;
        src +=linesize;// manzuduiqiyaoqiu
    }
}

void JCAVCoderHandler::convertAndPlayAudio(AVFrame *decodedFrame)
{

}

void JCAVCoderHandler::doReadMediaFrameThread()
{
    while(m_bThreadRunning)
    {
        m_bFileThreadRunning = true;

        if(MEDIAPLAY_STATUS_PAUSE == m_eMediaStatus)
        {
            stdThreadSleep(10);
            continue;
        }

        if(m_videoPktQue.size() > 600 && m_audioPktQue.size() > 1200)
        {
            stdThreadSleep(10);
            continue;
        }

        if(false == m_bReadFileEOF)
        {
            readMediaPacket();
        }
        else
        {
            stdThreadSleep(10);
        }
    }
}

void JCAVCoderHandler::doAudioDecodeThread()
{
    if(m_pformatCtx == NULL)
    {
        return;
    }

    if(m_pAudioFrame == NULL)
    {
        m_pAudioFrame = av_frame_alloc();
    }

    while (m_bThreadRunning)
    {
        m_bVideoThreadRunning = true;

        if(m_eMediaStatus == MEDIAPLAY_STATUS_PAUSE)
        {
            stdThreadSleep(10);
            continue;
        }

        if(m_audioPktQue.isEmpty())
        {
            stdThreadSleep(10);
            continue;
        }

        AVPacket *pkt = (AVPacket*)m_audioPktQue.dequeue();
        if(pkt == NULL)
        {
            break;
        }

        int reValue = avcodec_send_packet(m_pAudioCodecCtx, pkt);
        if(reValue != 0)
        {
            freePacket(pkt);
            continue;
        }

        int decodeRet = avcodec_receive_frame(m_pAudioCodecCtx, m_pAudioFrame);
        if(decodeRet == 0)
        {
            // xuanlanshipin
            convertAndRenderVideo(m_pVideoFrame, pkt->pts);
        }

        freePacket(pkt);
    }

    qDebug()<< "audio decode thread exit..." << endl;
}

void JCAVCoderHandler::doVideoDecodeThread()
{
    if(m_pformatCtx == NULL)
    {
        return;
    }

    if(m_pVideoFrame == NULL)
    {
        m_pVideoFrame = av_frame_alloc();
    }

    while (m_bThreadRunning)
    {
        m_bVideoThreadRunning = true;

        if(m_eMediaStatus == MEDIAPLAY_STATUS_PAUSE)
        {
            stdThreadSleep(10);
            continue;
        }

        if(m_videoPktQue.isEmpty())
        {
            stdThreadSleep(10);
            continue;
        }

        AVPacket *pkt = (AVPacket*)m_videoPktQue.dequeue();
        if(pkt == NULL)
        {
            break;
        }

        int reValue = avcodec_send_packet(m_pVideoCodecCtx, pkt);
        if(reValue != 0)
        {
            freePacket(pkt);
            continue;
        }

        int decodeRet = avcodec_receive_frame(m_pVideoCodecCtx, m_pVideoFrame);
        if(decodeRet == 0)
        {
            // xuanlanshipin
        }

        freePacket(pkt);
    }

    qDebug()<< "video decode thread exit..." << endl;
}

void JCAVCoderHandler::startMediaProcessThreads()
{
    m_bThreadRunning = true;

    // c++11 de xianchengku
    std::thread readThread(&JCAVCoderHandler::doReadMediaFrameThread, this);
    readThread.detach();// fenlichulai

    std::thread videoDecodeThread(&JCAVCoderHandler::doVideoDecodeThread, this);
    videoDecodeThread.detach();// fenlichulai

    std::thread audioDecodeThread(&JCAVCoderHandler::doAudioDecodeThread, this);
    audioDecodeThread.detach();// fenlichulai
}

// init videocodec x264
int JCAVCoderHandler::IntVideoCodec()
{
    // path not empty
    if(m_videoPathString.isEmpty())
    {
        qDebug() <<"video file path is Empty.....\n";
        return -1;
    }

    // the most function,find the avcformatcontex
    QByteArray pathByTesArray = m_videoPathString.toLocal8Bit();
    const char * filePath = pathByTesArray;

    if(avformat_open_input(&m_pformatCtx, filePath, NULL, NULL) != 0)
    {
        qDebug() << "Open input stream failed....\n";
        return -1;
    }

    if(avformat_find_stream_info(m_pformatCtx, NULL) != 0)
    {
        qDebug() << "Open find stream failed....\n";
        return -1;
    }

    av_dump_format(m_pformatCtx, 0, filePath, 0); // print stream info

    m_videoStreamIdx = -1;
    m_audioStreamIdx = -1;

    for(int i = 0; i < (int)m_pformatCtx->nb_streams; i++)
    {
        // huoqushipin jiemaxinxi,yiji codec xinxi
        AVCodecParameters *codecParameters = m_pformatCtx->streams[i]->codecpar;
        if(AVMEDIA_TYPE_VIDEO == codecParameters->codec_type)
        {
            m_videoStreamIdx = i;
            m_pVideoCodecCtx = m_pformatCtx->streams[i]->codec;
            AVCodec *codec = avcodec_find_decoder(m_pVideoCodecCtx->codec_id);
            if(codec == nullptr)
            {
                qDebug() << "video codec is null"<< endl;
                return -1;
            }

            if(avcodec_open2(m_pVideoCodecCtx, codec, NULL) < 0)
            {
                qDebug() << "couldn't open codec"<< endl;
                return -1;
            }
        }

        // huoquyinpin jiemaxinxi,yiji codec xinxi
        if(AVMEDIA_TYPE_AUDIO == codecParameters->codec_type)
        {
            m_audioStreamIdx = i;
            m_pAudioCodecCtx = m_pformatCtx->streams[i]->codec;
            AVCodec *codec = avcodec_find_decoder(m_pAudioCodecCtx->codec_id);
            if(codec == nullptr)
            {
                qDebug() << "video codec is null"<< endl;
                return -1;
            }

            if(avcodec_open2(m_pAudioCodecCtx, codec, NULL) < 0)
            {
                qDebug() << "couldn't open codec"<< endl;
                return -1;
            }
        }
    }

    // fenpei yuv neicun
    if(m_pYUVFrame == NULL)
    {
        m_pYUVFrame = av_frame_alloc();
    }

    if(m_pYUV420pBuffer == NULL)
    {
        m_pYUV420pBuffer = (uint8_t*)malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, \
                                                                      m_pVideoCodecCtx->width, \
                                                                      m_pVideoCodecCtx->height, 1));
    }

    av_image_fill_arrays(m_pYUVFrame->data, m_pYUVFrame->linesize, \
                         m_pYUV420pBuffer, AV_PIX_FMT_YUV420P, \
                         m_pVideoCodecCtx->width, m_pVideoCodecCtx->height, 1);


    m_videoWidth = m_pVideoCodecCtx->width;
    m_videoHeight = m_pVideoCodecCtx->height;

    AVStream *videoStream = m_pformatCtx->streams[m_videoStreamIdx];
    AVStream *audioStream = m_pformatCtx->streams[m_audioStreamIdx];

    // num / den biaoshi meiyizhenzhijian de shijian jiege
    if(nullptr != videoStream)
    {
        m_vStreamTimeRational = videoStream->time_base;
        qDebug()<< "videosize:" << m_videoWidth << "x" << m_videoHeight << endl;

        // shijiankedugainian,
        qDebug()<< "AVstream V Rational:" << videoStream->time_base.num << " " << videoStream->time_base.den;
    }


    if(nullptr != audioStream)
    {
       m_aStreamTimeRational = audioStream->time_base;
       qDebug()<< "AVstream A Rational:" << audioStream->time_base.num<< " " << audioStream->time_base.den;
    }

    return 0;
}

// uninit videocodec x264
void JCAVCoderHandler::UnInitVideoCode()
{

}
