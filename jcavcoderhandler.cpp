#include "jcavcoderhandler.h"

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

void JCAVCoderHandler::doReadMediaFrameThread()
{

}

void JCAVCoderHandler::doAudioDecodeThread()
{

}

void JCAVCoderHandler::doVideoDecodeThread()
{

}

void JCAVCoderHandler::startMediaProcessThreads()
{
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
