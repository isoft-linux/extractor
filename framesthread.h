#ifndef FRAMESTHREAD_H
#define FRAMESTHREAD_H

#include<QThread>
#include <QString>
#include <QList>
#include <QImage>

#include "qvideofile.h"
class FramesThread: public QThread

{
Q_OBJECT
public:
    FramesThread();
    void run();
    void setVideo(QVideoFile *v);
    void setStartTime(int t);
private:
    QVideoFile * video;
    int stime;
signals:
    void getAllFrames(QList<QImage>);
};

#endif // FRAMESTHREAD_H
