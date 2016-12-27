#ifndef AUDIO_H
#define AUDIO_H

#include "AudioResampler.h"

class Audio
{
    public:

    Audio();

    Audio(const std::string &path, uint64_t ch_layout,
          AVSampleFormat format, int rate);

    Audio(const Audio &a) = delete;

    ~Audio();

    void open_file(const std::string &path, uint64_t ch_layout,
                   AVSampleFormat format, int rate);

    int get_samples(uint8_t **buffer, int samples_wanted);

    private:

    int             nb_ch               = 0;
    bool            init                = false;
    AudioResampler  *r                  = NULL;
    AVSampleFormat  format              = AV_SAMPLE_FMT_NONE;

    uint8_t         **samples           = NULL;
    int             remaining_samples   = 0;
    int             consumed_samples    = 0;

    void deinit();
    void clean_up();
};

#endif
