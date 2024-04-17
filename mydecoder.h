#ifndef MYDECODER_H
#define MYDECODER_H

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"

#include "SDL2/SDL.h"
}
#include <QWidget>
#include <QThread>
#include <QLabel>

class myDecoder : public QWidget
{
    Q_OBJECT
public:
    myDecoder(const char* _inputFilePath);

public:
    AVFormatContext*		m_pFormatCtx;
    int                     m_iVideoindex;
    AVCodecContext*			m_pCodecCtx;
    AVCodecParameters*		m_pCodecParam;
    AVCodecParserContext*	m_parser;
    const AVCodec*			m_pCodec;
    AVFrame*				m_pFrame;
    AVFrame*				m_pFrameRGB;
    uint8_t*				m_out_buffer;
    AVPacket*				m_packet;

    QString                 m_inputFilePath;

    QLabel*                 m_pLabel;
    void                    playVideo();
};

#endif // MYDECODER_H
