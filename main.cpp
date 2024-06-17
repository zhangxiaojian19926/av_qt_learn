#include "jcmplayer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    JCMPlayer w;
    w.show();
    return a.exec();
}
