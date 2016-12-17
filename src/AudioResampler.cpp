#include "AudioResampler.h"

/*
 *  Public AudioResampler functions
 */

AudioResampler::AudioResampler() : AudioResampler(std::string(), 0, AV_SAMPLE_FMT_NONE, 0)
{}

AudioResampler::AudioResampler(const std::string &path, uint64_t ch_layout,
                               AVSampleFormat format, int rate)
{
    dec = new AudioDecoder();

    if(path.length())
    {
        open_file(path, ch_layout, format, rate);
    }
}

AudioResampler::~AudioResampler()
{
    clean_up();
}

void AudioResampler::open_file(const std::string &path, uint64_t ch_layout,
                               AVSampleFormat format, int rate)
{
    AudioResampler::ch_layout   = ch_layout;
    AudioResampler::format      = format;

    dec->open_file(path);

    if(swr_ctx) swr_free(&swr_ctx);
    swr_ctx = swr_alloc_set_opts(
        swr_ctx,
        ch_layout,
        format,
        rate,
        dec->get_ch_layout(),
        dec->get_format(),
        dec->get_rate(),
        0,
        NULL
    );

    if(!swr_ctx)
    {
        throw 8;
    }

    if(swr_init(swr_ctx) != 0)
    {
        throw 9;
    }

    init = true;
}

int AudioResampler::get_samples(uint8_t ***buffers)
{
    if(!init) return 0;

    uint8_t **audio_data;
    int     in_samples;
    int     out_samples;
    int     result;

    in_samples = dec->get_samples(&audio_data);

    if(in_samples < 0)
    {
        in_samples = 0;
    }

    // Upper bound of sample count after resampling
    out_samples = swr_get_out_samples(swr_ctx, in_samples);

    if(out_samples == 0) return AVERROR_EOF;

    result = av_samples_alloc_array_and_samples(
        buffers,
        NULL,
        av_get_channel_layout_nb_channels(ch_layout),
        out_samples,
        format,
        0
    );

    if(result < 0) return 0;

    // Get the final output samples if there is no more input
    if(in_samples == 0)
    {
        result = swr_convert(swr_ctx, *buffers, out_samples, NULL, 0);
        if(result == 0) result = AVERROR_EOF;
    }
    else
    {
        result = swr_convert(swr_ctx, *buffers, out_samples,
            (const uint8_t **) audio_data, in_samples);
    }

    // Free the input buffers if they contain anything
    if(in_samples)
    {
        av_freep(&audio_data[0]);
        av_freep(&audio_data);
    }

    // Free the output buffers if we couldn't fill them
    if(result < 0)
    {
        av_freep(buffers[0]);
        av_freep(buffers);
    }

    return result;
}

/*
 *  Private AudioResampler functions
 */

void AudioResampler::clean_up()
{
    init = false;

    if(dec)     delete dec;
    if(swr_ctx) swr_free(&swr_ctx);
}
