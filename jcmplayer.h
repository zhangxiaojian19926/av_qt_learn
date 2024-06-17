#ifndef JCMPLAYER_H
#define JCMPLAYER_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QUrl>

#include "jcavcoderhandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class JCMPlayer; }
QT_END_NAMESPACE

class JCMPlayer : public QMainWindow
{
    Q_OBJECT

public:
    JCMPlayer(QWidget *parent = nullptr);
    ~JCMPlayer();

private:
    Ui::JCMPlayer *ui;

    JCAVCoderHandler *m_pAVCodecHandler = NULL;

    void setupPlayerCentralWidget();
    void releasePlayCentralWidget();

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

private slots:
    void openMediaPlayerWithPath(const QString &filePath);

};
#endif // JCMPLAYER_H
