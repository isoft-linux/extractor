#ifndef QVIDEOFILE_H
#define QVIDEOFILE_H
extern "C"{
#define UINT64_C(val) val##ULL
#define INT64_C(val) val##LL
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>
#include <assert.h>


  /* Minimum of signed integral types.  */
# define INT8_MIN       (-128)
# define INT16_MIN      (-32767-1)
# define INT32_MIN      (-2147483647-1)
# define INT64_MIN      (-INT64_C(9223372036854775807)-1)
//# define INT64_MIN      (-__INT64_C(9223372036854775807)-1)
  /* Maximum of signed integral types.  */
# define INT8_MAX       (127)
# define INT16_MAX      (32767)
# define INT32_MAX      (2147483647)
# define INT64_MAX      (INT64_C(9223372036854775807))
}

#include <QString>
#include <QImage>
#include <QList>

class QVideoFile
{
public:
    QVideoFile();
    ~QVideoFile();
    bool openFile(QString filename);
    void close();
    QImage getFrame(int sec);
    QList<QImage> getFrames(int startsec);
    //bool saveFrame2Png(int sec);
    QString getFilePath();
    void setSavePath(QString);
    QString getSavePath();
    int getLength();
    int getFps();
    void setTimeStep(int);
    int getTimeStep();
    int getWidth();
    int getHeight();

protected:
    AVFormatContext *pFormatCtx;
    int             videoStream;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVFrame         *pFrame;
    AVFrame         *pFrameRGB;
    AVPacket        packet;
    int             numBytes;
    uint8_t         *buffer;

    AVDictionary    *optionsDict;
    struct SwsContext      *sws_ctx;

private:
    QImage frame2img(AVFrame *frame);
    QString filepath;
    int total_length;
    int fps;
    int width;
    int height;

    QString savepath;
    int timestep;



};

#endif // QVIDEOFILE_H
