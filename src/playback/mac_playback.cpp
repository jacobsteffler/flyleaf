#include "mac_playback.h"
#include "Audio.h"
#include <mutex>

namespace Playback
{
    static AudioComponent           output_comp;
    static AudioComponentInstance   output_instance;

    static const int rate           = 48000;
    static const int chan           = 2;
    static const format_type format = FMT_S32_LE;

    static std::mutex audiolck;
    static Audio *a;

    static AURenderCallbackStruct s_callback;

    static CoreAudioFormatDescriptionMap format_map[] =
    {
        {FMT_S16_LE, 16, sizeof(int16_t), kAudioFormatFlagIsSignedInteger},
        {FMT_S16_BE, 16, sizeof(int16_t), kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian},
        {FMT_S32_LE, 32, sizeof(int32_t), kAudioFormatFlagIsSignedInteger},
        {FMT_S32_BE, 32, sizeof(int32_t), kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian},
        {FMT_FLOAT,  32, sizeof(float),   kAudioFormatFlagIsFloat},
    };

    void init()
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
            if(it.type == format)
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
        stream_format.mSampleRate        = rate;
        stream_format.mFormatID          = kAudioFormatLinearPCM;
        stream_format.mFormatFlags       = m->flags;
        stream_format.mFramesPerPacket   = 1;
        stream_format.mChannelsPerFrame  = chan;
        stream_format.mBitsPerChannel    = m->bits_per_sample;
        stream_format.mBytesPerPacket    =
        stream_format.mBytesPerFrame     = chan * m->bytes_per_sample;

        printf("Stream format:\n");
        printf(" Channels: %d\n", stream_format.mChannelsPerFrame);
        printf(" Sample rate: %f\n", stream_format.mSampleRate);
        printf(" Bits per channel: %d\n", stream_format.mBitsPerChannel);
        printf(" Bytes per frame: %d\n", stream_format.mBytesPerFrame);

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

    void open_stream(const std::string &path)
    {
        audiolck.lock();
        a->open_file(path, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S32, rate);
        audiolck.unlock();
    }

    void play(bool play)
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
    
    OSStatus render_callback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags,
                             const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                             UInt32 inNumberFrames, AudioBufferList *ioData)
    {
        audiolck.lock();
        a->get_samples((uint8_t **) &(ioData->mBuffers[0].mData),
                       ioData->mBuffers[0].mDataByteSize / (8));
        audiolck.unlock();
        
        return 0;
    }
}
