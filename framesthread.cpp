#include"framesthread.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <QTextCodec>
#include <QProcess>
#include <iostream>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>

FramesThread::FramesThread()
{
}

void FramesThread::setVideo(QVideoFile *v)
{
    this->video = v;
}
void FramesThread::setStartTime(int t)
{
    this->stime = t;
}

void FramesThread::run()
{
   QList<QImage> list = video->getFrames(stime);
   emit(getAllFrames(list));
}

