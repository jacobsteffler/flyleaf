#ifndef AUDIODECODER_H
#define AUDIODECODER_H

#include <string>

extern "C"
{

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

}

class AudioDecoder
{
    public:

    AudioDecoder();

    AudioDecoder(const std::string &path);

    AudioDecoder(const AudioDecoder &d) = delete;

    ~AudioDecoder();

    void            open_file(const std::string &path);

    int             get_samples(uint8_t ***audio_data);

    uint64_t        get_ch_layout() const;

    AVSampleFormat  get_format()    const;

    int             get_rate()      const;

    private:

    // FFmpeg structures
    AVFormatContext *format_ctx = NULL;
    AVCodecContext  *codec_ctx  = NULL;
    AVCodec         *codec      = NULL;
    AVPacket        *packet     = NULL;
    AVFrame         *frame      = NULL;

    bool init                   = false;
    int audio_stream            = 0;

    void    init_av();

    void    alloc();

    void    clean_up();

    int     fill_frame();
};

#endif
