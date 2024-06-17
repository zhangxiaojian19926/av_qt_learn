#include "jcavcoderhandler.h"

JCAVCoderHandler::JCAVCoderHandler()
    :m_videoPathString(""),
      m_videoWidth(0),
      m_videoHeight(0),
      m_pformatCtx(NULL)
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



    return 0;
}

// uninit videocodec x264
void JCAVCoderHandler::UnInitVideoCode()
{

}
