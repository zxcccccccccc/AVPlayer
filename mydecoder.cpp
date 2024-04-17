#include "mydecoder.h"
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <QtConcurrent>
#include <QHBoxLayout>

myDecoder::myDecoder(const char *_inputFilePath) : QWidget(nullptr)
{
//    setFixedSize(800,400);
    setWindowTitle("myavplayerrrrrrrrrrrrrrrr");
    m_pLabel = new QLabel(this);
//    m_pLabel->setFixedSize(800,400);
    m_pLabel->setText("label");
    this->setLayout(new QHBoxLayout);
    this->layout()->addWidget(m_pLabel);

    m_inputFilePath = QString(_inputFilePath);

    qDebug() << "myDecoder construct---";

    QFuture<void> f2 =QtConcurrent::run(this,&myDecoder::playVideo);
}

void myDecoder::playVideo()
{
    m_pFormatCtx = avformat_alloc_context();
    const char* fp = m_inputFilePath.toStdString().c_str();
    if (avformat_open_input(&m_pFormatCtx, fp, NULL, NULL) != 0) {
        qDebug() << "Couldn't open input stream.";
    }

    if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0) {
        qDebug() << "Couldn't find stream information.";
    }

    m_iVideoindex = -1;

    for (unsigned int i = 0; i < m_pFormatCtx->nb_streams; i++)
        if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_iVideoindex = i;
            break;
        }

    m_pCodecParam = m_pFormatCtx->streams[m_iVideoindex]->codecpar;
    m_pCodec = avcodec_find_decoder(m_pCodecParam->codec_id);
    if (m_pCodec == NULL) {
        qDebug() << "Codec not found.";
    }

    m_pCodecCtx = avcodec_alloc_context3(m_pCodec);
    avcodec_parameters_to_context(m_pCodecCtx, m_pCodecParam);
    if (avcodec_open2(m_pCodecCtx, m_pCodec, NULL) < 0) {
        qDebug() << "Could not open codec.";
    }

    m_pFrame = av_frame_alloc();
    if (!m_pFrame) {
        qDebug() << "Could not open codec.";
    }
    m_pFrameRGB = av_frame_alloc();
    m_packet = av_packet_alloc();

    struct SwsContext* swsCtx = sws_getContext(
        m_pCodecCtx->width,
        m_pCodecCtx->height,
        m_pCodecCtx->pix_fmt,
        m_pCodecCtx->width,
        m_pCodecCtx->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );

    while (av_read_frame(m_pFormatCtx, m_packet) >= 0)
    {
        if (m_packet->stream_index == m_iVideoindex)
        {
            if (avcodec_send_packet(m_pCodecCtx, m_packet) < 0)
            {
                break;
            }
            int frameFinished = 1;
            while (frameFinished >= 0)
            {
                frameFinished = avcodec_receive_frame(m_pCodecCtx, m_pFrame);

                if (frameFinished == AVERROR(EAGAIN) || frameFinished == AVERROR(AVERROR_EOF))
                {
                    break;
                }
                else if(frameFinished < 0)
                {
                    break;
                }
                if(frameFinished >= 0 && m_pFrame != NULL)
                {
                    av_image_alloc(m_pFrameRGB->data, m_pFrameRGB->linesize,
                        m_pCodecCtx->width, m_pCodecCtx->height, AV_PIX_FMT_RGB24, 1);

                    sws_scale
                    (
                        swsCtx,
                        m_pFrame->data,
                        m_pFrame->linesize,
                        0,
                        m_pCodecCtx->height,
                        m_pFrameRGB->data,
                        m_pFrameRGB->linesize
                    );

                    QImage img((uchar*)m_pFrameRGB->data[0], m_pCodecCtx->width, m_pCodecCtx->height, QImage::Format_RGB888);
                    m_pLabel->setPixmap(QPixmap::fromImage(img));
                    qDebug() << "decoding video...";
                    QEventLoop loop;
                    QTimer::singleShot(40, &loop, SLOT(quit()));
                    loop.exec();
                }
            }
        }
        av_packet_unref(m_packet);
    }
    sws_freeContext(swsCtx);

    av_frame_free(&m_pFrame);
    av_frame_free(&m_pFrameRGB);

    avcodec_free_context(&m_pCodecCtx);
    avformat_close_input(&m_pFormatCtx);
}
