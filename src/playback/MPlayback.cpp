#include "MPlayback.h"

namespace Playback
{
    static AudioComponent           output_comp;
    static AudioComponentInstance   output_instance;

    static const int rate               = 48000;
    static const int chan               = 2;
    static const AVSampleFormat format  = AV_SAMPLE_FMT_S32;
    static const uint64_t ch_layout     = AV_CH_LAYOUT_STEREO;

    static std::mutex audio_lock;
    static Audio *a;

    static AURenderCallbackStruct s_callback;

    static CoreAudioFormatDescriptionMap format_map[] =
    {
        {AV_SAMPLE_FMT_S16, 16, sizeof(int16_t), kAudioFormatFlagIsSignedInteger},
        {AV_SAMPLE_FMT_S32, 32, sizeof(int32_t), kAudioFormatFlagIsSignedInteger},
        {AV_SAMPLE_FMT_FLT, 32, sizeof(float),   kAudioFormatFlagIsFloat}
    };
}

void Playback::init()
{
    a = new Audio();

    AudioComponentDescription desc;
    desc.componentType          = kAudioUnitType_Output;
    desc.componentSubType       = kAudioUnitSubType_DefaultOutput;
    desc.componentFlags         = 0;
    desc.componentFlagsMask     = 0;
    desc.componentManufacturer  = kAudioUnitManufacturer_Apple;

    output_comp = AudioComponentFindNext(NULL, &desc);
    if(!output_comp)
    {
        throw 1;
    }

    if(AudioComponentInstanceNew(output_comp, &output_instance))
    {
        throw 2;
    }

    CoreAudioFormatDescriptionMap *m = NULL;
    for(CoreAudioFormatDescriptionMap it : format_map)
    {
        if(it.format == format)
        {
            m = &it;
            break;
        }
    }

    if(!m)
    {
        throw 3;
    }

    if(AudioUnitInitialize(output_instance))
    {
        throw 4;
    }

    AudioStreamBasicDescription stream_format;
    stream_format.mSampleRate       = rate;
    stream_format.mFormatID         = kAudioFormatLinearPCM;
    stream_format.mFormatFlags      = m->flags;
    stream_format.mFramesPerPacket  = 1;
    stream_format.mChannelsPerFrame = chan;
    stream_format.mBitsPerChannel   = m->bits_per_sample;
    stream_format.mBytesPerPacket   =
    stream_format.mBytesPerFrame    = chan * m->bytes_per_sample;

#ifndef NDEBUG
    printf("Stream format:\n");
    printf(" Flags: %d\n", stream_format.mFormatFlags);
    printf(" Channels: %d\n", stream_format.mChannelsPerFrame);
    printf(" Sample rate: %f\n", stream_format.mSampleRate);
    printf(" Bits per channel: %d\n", stream_format.mBitsPerChannel);
    printf(" Bytes per frame: %d\n", stream_format.mBytesPerFrame);
#endif

    if(AudioUnitSetProperty(output_instance, kAudioUnitProperty_StreamFormat,
                            kAudioUnitScope_Input, 0, &stream_format, sizeof(stream_format)))
    {
        throw 5;
    }

    s_callback.inputProc = render_callback;
    s_callback.inputProcRefCon = NULL;

    if(AudioUnitSetProperty(output_instance, kAudioUnitProperty_SetRenderCallback,
                            kAudioUnitScope_Input, 0, &s_callback, sizeof(AURenderCallbackStruct)))
    {
        throw 6;
    }
}

void Playback::open_stream(const std::string &path)
{
    audio_lock.lock();
    a->open_file(path, ch_layout, format, rate);
    audio_lock.unlock();
}

void Playback::play(bool play)
{
    if(play)
    {
        if(AudioOutputUnitStart(output_instance))
        {
            throw 7;
        }
    }
    else
    {
        if(AudioOutputUnitStop(output_instance))
        {
            throw 8;
        }
    }
}

OSStatus Playback::render_callback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags,
                                   const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                                   UInt32 inNumberFrames, AudioBufferList *ioData)
{
    audio_lock.lock();
    a->get_samples((uint8_t **) &(ioData->mBuffers[0].mData),
                   ioData->mBuffers[0].mDataByteSize / (8));
    audio_lock.unlock();

    return 0;
}
