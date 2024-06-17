#include "jcmplayer.h"
#include "ui_jcmplayer.h"

JCMPlayer::JCMPlayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::JCMPlayer)
{
    ui->setupUi(this);

    setupPlayerCentralWidget();
}

JCMPlayer::~JCMPlayer()
{
    delete ui;

    releasePlayCentralWidget();
}

void JCMPlayer::setupPlayerCentralWidget()
{
    m_pAVCodecHandler = new JCAVCoderHandler();
}

void JCMPlayer::releasePlayCentralWidget()
{
    if(m_pAVCodecHandler != NULL)
    {
        delete m_pAVCodecHandler;
        m_pAVCodecHandler = NULL;
    }
}

void JCMPlayer::dropEvent(QDropEvent *event)
{
    QUrl url = event->mimeData()->urls().first();
    if(url.isEmpty())
    {
        qDebug() << "URL is empty\n";
        return ;
    }

    QByteArray byte = QByteArray(url.toString().toUtf8());
    QUrl encodedUrl(byte);
    encodedUrl = encodedUrl.fromEncoded(byte);
    QString realPath = encodedUrl.toLocalFile();

    openMediaPlayerWithPath(realPath);
}

void JCMPlayer::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
}

void JCMPlayer::openMediaPlayerWithPath(const QString &filePath)
{

}



