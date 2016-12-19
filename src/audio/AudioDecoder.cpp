#include "AudioDecoder.h"

/*
 *  Public AudioDecoder functions
 */

AudioDecoder::AudioDecoder() : AudioDecoder(std::string())
{}

AudioDecoder::AudioDecoder(const std::string &path)
{
    init_av();

    if(path.length())
    {
        open_file(path);
    }
}

AudioDecoder::~AudioDecoder()
{
    clean_up();
}

void AudioDecoder::open_file(const std::string &path)
{
    clean_up();
    alloc();

    // Open the file or stream
    if(avformat_open_input(&format_ctx, path.c_str(), NULL, NULL) != 0)
    {
        throw 1;
    }

    // Is this a format we understand?
    if(avformat_find_stream_info(format_ctx, NULL) != 0)
    {
        throw 2;
    }

    // Find the index of the audio stream
    audio_stream = -1;
    for(unsigned i = 0; i < format_ctx->nb_streams; ++i)
    {
        if(format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audio_stream = i;
            break;
        }
    }

    if(audio_stream < 0)
    {
        throw 3;
    }

#ifndef NDEBUG
    av_dump_format(format_ctx, audio_stream, path.c_str(), 0);
#endif

    // Initialize the codec context
    if(avcodec_parameters_to_context(codec_ctx, format_ctx->streams[audio_stream]->codecpar) != 0)
    {
        throw 4;
    }

    codec = avcodec_find_decoder(codec_ctx->codec_id);
    if(codec == NULL)
    {
        throw 5;
    }

    if(avcodec_open2(codec_ctx, codec, NULL) != 0)
    {
        throw 6;
    }

    init = true;
}

int AudioDecoder::get_samples(uint8_t ***audio_data)
{
    if(!init) return 0;

    // Fill the frame with samples
    int result = fill_frame();
    if(result != 0) return result;

    // Allocate the buffer and copy to it
    result = av_samples_alloc_array_and_samples(audio_data, NULL, frame->channels,
                                                frame->nb_samples, codec_ctx->sample_fmt, 0);

    if(result < 0) return result;

    result = av_samples_copy(*audio_data, frame->extended_data, 0, 0, frame->nb_samples,
                             frame->channels, codec_ctx->sample_fmt);

    if(result != 0) return result;

    return frame->nb_samples;
}

uint64_t AudioDecoder::get_ch_layout() const
{
    if(!init) return 0;

    return codec_ctx->channel_layout;
}

AVSampleFormat AudioDecoder::get_format() const
{
    if(!init) return AV_SAMPLE_FMT_NONE;

    return codec_ctx->sample_fmt;
}

int AudioDecoder::get_rate() const
{
    if(!init) return 0;

    return codec_ctx->sample_rate;
}

/*
 *  Private AudioDecoder functions
 */

void AudioDecoder::init_av()
{
    av_register_all();
    avformat_network_init();
}

void AudioDecoder::alloc()
{
    codec_ctx   = avcodec_alloc_context3(NULL);
    packet      = av_packet_alloc();
    frame       = av_frame_alloc();
}

void AudioDecoder::clean_up()
{
    init = false;

    if(packet)      av_packet_free(&packet);
    if(frame)       av_frame_free(&frame);
    if(codec_ctx)   avcodec_free_context(&codec_ctx);
    if(format_ctx)  avformat_close_input(&format_ctx);
}

int AudioDecoder::fill_frame()
{
    // Check if a frame is already available
    if(avcodec_receive_frame(codec_ctx, frame) == 0) return 0;

    int result;
    result = av_read_frame(format_ctx, packet);

    // Keep loading packets until we get one with audio
    while(result == 0 && packet->stream_index != audio_stream)
    {
        av_packet_unref(packet);
        result = av_read_frame(format_ctx, packet);
    }

    if(result == 0)
    {
        // Send the packet for decoding
        avcodec_send_packet(codec_ctx, packet);
    }
    else if(result == AVERROR_EOF)
    {
        // Put decoder into draining mode
        avcodec_send_packet(codec_ctx, NULL);
    }
    else
    {
        // Some kind of read error
        av_packet_unref(packet);
        return result;
    }

    av_packet_unref(packet);

    // Try to receive a decoded frame
    result = avcodec_receive_frame(codec_ctx, frame);

    // Do we need to send more packets?
    if(result == AVERROR(EAGAIN))
    {
        return fill_frame();
    }
    else
    {
        return result;
    }
}
