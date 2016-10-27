#ifndef EXTRACTTHREAD_H
#define EXTRACTTHREAD_H

#include<QThread>
#include <QString>
#include <QImage>

#include "qvideofile.h"
class ExtractThread: public QThread

{
Q_OBJECT
public:
    ExtractThread();
    void setVideo(QVideoFile*);
    void run();
signals:
    void getOnePreviewImage(QImage img,int sec);
private:
    QVideoFile *video;
};
#endif // EXTRACTTHREAD_H
