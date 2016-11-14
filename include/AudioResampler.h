#ifndef RESAMPLER_H
#define RESAMPLER_H

#include "AudioDecoder.h"

#include <string>

extern "C" {

#include <libswresample/swresample.h>

}

class AudioResampler
{
    public:

    AudioResampler();

    AudioResampler(const std::string &path, uint64_t ch_layout,
                   AVSampleFormat format, int rate);

    AudioResampler(const AudioResampler &r) = delete;

    ~AudioResampler();

    void open_file(const std::string &path, uint64_t ch_layout,
                   AVSampleFormat format, int rate);

    int get_samples(uint8_t ***buffers);

    private:

    AudioDecoder    *dec        = NULL;
    SwrContext      *swr_ctx    = NULL;

    bool            init        = false;
    uint64_t        ch_layout;
    AVSampleFormat  format;

    void init_swr();

    void clean_up();
};

#endif
