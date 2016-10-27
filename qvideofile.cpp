#include "qvideofile.h"

QVideoFile::QVideoFile()
{
    pFormatCtx = 0;
    pCodecCtx = 0;
    pCodec = 0;
    pFrame = 0;
    pFrameRGB = 0;
    buffer = 0;
    videoStream = -1;
    optionsDict = 0;
    sws_ctx = 0;
    this->filepath="";
    this->savepath="";
    this->timestep=0;
    this->total_length=0;
    this->fps=0;
    this->width=0;
    this->height=0;
    av_register_all();
}

QVideoFile::~QVideoFile()
{
    close();
}

void QVideoFile::close()
{
    // Free the RGB image
    if(buffer)
        delete [] buffer;

    // Free the YUV frame
    if(pFrame)
        av_free(pFrame);

    // Free the RGB frame
    if(pFrameRGB)
        av_free(pFrameRGB);

    // Close the codec
    if(pCodecCtx)
        avcodec_close(pCodecCtx);

    // Close the video file
    if(pFormatCtx)
        avformat_close_input(&pFormatCtx);
    pFormatCtx = 0;
    pCodecCtx = 0;
    pCodec = 0;
    pFrame = 0;
    pFrameRGB = 0;
    buffer = 0;
    videoStream = -1;
    optionsDict = 0;
    sws_ctx = 0;
}

bool QVideoFile::openFile(QString filename)
{
    this->filepath = filename;

    if(avformat_open_input(&pFormatCtx, filename.toLocal8Bit().data(), NULL, NULL)!=0)
        return false; // Couldn't open file

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
        return false; // Couldn't find stream information
    for(unsigned i=0; i<pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoStream=i;
            break;
        }
    if(videoStream==-1)
        return false; // Didn't find a video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL)
        return false; // Codec not found

    if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0)
        return false; // Could not open codec

    // Allocate video frame
    pFrame=av_frame_alloc();

    // Allocate an AVFrame structure
    pFrameRGB=av_frame_alloc();
    if(pFrameRGB==NULL)
        return false;

    // Determine required buffer size and allocate buffer
    numBytes=avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
                                pCodecCtx->height);
    buffer=new uint8_t[numBytes];
    //(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
                   pCodecCtx->width, pCodecCtx->height);

    sws_ctx =
            sws_getContext
            (
                pCodecCtx->width,
                pCodecCtx->height,
                pCodecCtx->pix_fmt,
                pCodecCtx->width,
                pCodecCtx->height,
                AV_PIX_FMT_RGB24,
                SWS_BILINEAR,
                NULL,
                NULL,
                NULL
                );



    this->total_length = pFormatCtx->duration/AV_TIME_BASE;
    AVStream *st = pFormatCtx->streams[videoStream];

    this->width = st->codec->width;
    this->height = st->codec->height;

    int fps_t =0;
    if(st->avg_frame_rate.den && st->avg_frame_rate.num)
        fps_t = av_q2d(st->avg_frame_rate);
    if(st->r_frame_rate.den && st->r_frame_rate.num)
        fps_t = av_q2d(st->r_frame_rate);

    if(fps_t <= 0)
        fps_t = 20;
    this->fps = fps_t;

    return true;
}

QImage QVideoFile::getFrame(int sec)
{
    QImage img;
    int frameFinished;

    int64_t seek_target = (int64_t)sec*AV_TIME_BASE;

    if (pFormatCtx->start_time != AV_NOPTS_VALUE)
        seek_target += pFormatCtx->streams[videoStream]->start_time;

    seek_target = av_rescale_q(seek_target, AV_TIME_BASE_Q, pFormatCtx->streams[videoStream]->time_base);

    //int ret =
    avformat_seek_file(pFormatCtx, videoStream, INT64_MIN, seek_target, seek_target, 0);
    avcodec_flush_buffers(pFormatCtx->streams[videoStream]->codec);
    while(av_read_frame(pFormatCtx, &packet)>=0) {
        int len = avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,
                                        &packet);
        if(len >0 && frameFinished)
        {
            if(packet.dts < seek_target)
                continue;
            // Convert the image from its native format to RGB
            sws_scale
                    (
                        sws_ctx,
                        (uint8_t const * const *)pFrame->data,
                        pFrame->linesize,
                        0,
                        pCodecCtx->height,
                        pFrameRGB->data,
                        pFrameRGB->linesize
                        );

            img = frame2img(pFrameRGB);
            av_free_packet(&packet);
            break;
        }
        av_free_packet(&packet);
    }
    return img;
}


QList<QImage> QVideoFile::getFrames(int sec)
{
    QList<QImage> list;
    int frameFinished;

    int64_t seek_target = (int64_t)sec*AV_TIME_BASE;
    int64_t seek_end = (int64_t)(sec+timestep)*AV_TIME_BASE;

    if (pFormatCtx->start_time != AV_NOPTS_VALUE){
        seek_target += pFormatCtx->streams[videoStream]->start_time;
        seek_end += pFormatCtx->streams[videoStream]->start_time;
    }
    seek_target = av_rescale_q(seek_target, AV_TIME_BASE_Q, pFormatCtx->streams[videoStream]->time_base);
    seek_end = av_rescale_q(seek_end, AV_TIME_BASE_Q, pFormatCtx->streams[videoStream]->time_base);

    //int ret =
    avformat_seek_file(pFormatCtx, videoStream, INT64_MIN, seek_target, seek_target, 0);

    avcodec_flush_buffers(pFormatCtx->streams[videoStream]->codec);
    bool findfirstframe = false;
    while(av_read_frame(pFormatCtx, &packet)>=0) {
        int len = avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,
                                        &packet);
        if(len >0 && frameFinished)
        {
            //packet.dts used to detect the first frame we should seek.
            //after that, we read frame one by one util reach the timestep end.
            if(!findfirstframe && packet.dts < seek_target)
                continue;
            else
                findfirstframe = true;

            if(packet.dts > seek_end) {
                av_free_packet(&packet);
                break;
            }
            // Convert the image from its native format to RGB
            sws_scale
                    (
                        sws_ctx,
                        (uint8_t const * const *)pFrame->data,
                        pFrame->linesize,
                        0,
                        pCodecCtx->height,
                        pFrameRGB->data,
                        pFrameRGB->linesize
                        );

            QImage img = frame2img(pFrameRGB);
            img.setText("Description",QString("%1").arg(sec,5,10,QChar('0')));
            list.append(img);
            av_free_packet(&packet);
            // break;
        }
    }
    return list;
}

QImage QVideoFile::frame2img(AVFrame *frame)
{
    QImage img = QImage(pCodecCtx->width,pCodecCtx->height, QImage::Format_RGB888);
    // Write pixel data
    for(int y=0; y<height; y++)
        memcpy(img.scanLine(y), frame->data[0]+y*frame->linesize[0], width*3);
    return img;
}


QString QVideoFile::getFilePath()
{
    return this->filepath;
}

void QVideoFile::setSavePath(QString s)
{
    this->savepath = s;
}

QString QVideoFile::getSavePath()
{
    return this->savepath;
}


int QVideoFile::getLength()
{
    return this->total_length;
}

int QVideoFile::getFps()
{
    return this->fps;
}

void QVideoFile::setTimeStep(int t)
{
    this->timestep = t;
}


int QVideoFile::getTimeStep()
{
    return this->timestep;
}


int QVideoFile::getWidth()
{
    return this->width;
}

int QVideoFile::getHeight()
{
    return this->height;
}
