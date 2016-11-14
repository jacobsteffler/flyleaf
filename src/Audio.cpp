#include "Audio.h"

#include <algorithm>

/*
 *  Public Audio functions
 */

Audio::Audio() : Audio(std::string(), 0, AV_SAMPLE_FMT_NONE, 0)
{}

Audio::Audio(const std::string &path, uint64_t ch_layout,
             AVSampleFormat format, int rate)
{
    if(path.length())
    {
        open_file(path, ch_layout, format, rate);
    }
}

Audio::~Audio()
{
    clean_up();
}

void Audio::open_file(const std::string &path, uint64_t ch_layout,
                      AVSampleFormat format, int rate)
{
    clean_up();

    Audio::format = format;

    r = new AudioResampler(path, ch_layout, format, rate);

    nb_ch   = av_get_channel_layout_nb_channels(ch_layout);
    init    = true;
}

int Audio::get_samples(uint8_t **buffer, int samples_wanted)
{
    if(!init) return 0;

    int num_copy;
    int samples_given = 0;

    while(samples_wanted)
    {
        if(remaining_samples == 0)
        {
            remaining_samples = r->get_samples(&samples);
            consumed_samples = 0;
        }

        // Couldn't get any more data
        if(remaining_samples < 0)
        {
            remaining_samples = 0;
            break;
        }

        // Copy no more than we want, and no more than we can
        num_copy = std::min(samples_wanted, remaining_samples);

        av_samples_copy(buffer, samples, samples_given, consumed_samples,
                        num_copy, nb_ch, format);

        samples_given       += num_copy;
        consumed_samples    += num_copy;
        remaining_samples   -= num_copy;
        samples_wanted      -= num_copy;

        // Free the buffers if there's nothing in them
        if(remaining_samples == 0)
        {
            av_freep(&samples[0]);
            av_freep(&samples);
        }
    }

    return samples_given;
}

/*
 *  Private Audio functions
 */

void Audio::clean_up()
{
    if(r) delete r;

    if(remaining_samples)
    {
        av_freep(&samples[0]);
        av_freep(&samples);
    }

    init = false;
}
