#include"extractthread.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <QTextCodec>
#include <QProcess>
#include <iostream>
#include <QDir>
#include <QCoreApplication>

ExtractThread::ExtractThread()
{
}

void ExtractThread::setVideo(QVideoFile *v)
{
    this->video = v;
}

void ExtractThread::run()
{
    int length = video->getLength();
    int timestep = video->getTimeStep();

    for(int i = 0; i < length/timestep; i++)
    {
        QImage img = video->getFrame(i*timestep);
        if(img.isNull())
            continue;
        img = img.scaledToWidth(256);
        emit getOnePreviewImage(img,i*timestep);
    }
}
