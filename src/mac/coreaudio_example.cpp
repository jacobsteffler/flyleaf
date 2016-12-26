/* CoreAudio utility functions, public domain.
http://kaniini.dereferenced.org/2014/08/31/CoreAudio-sucks.html */

#include "coreaudio_example.h"

namespace coreaudio_example {

    struct CoreAudioFormatDescriptionMap {
        enum format_type type;
        int bits_per_sample;
        int bytes_per_sample;
        unsigned int flags;
    };

    static struct CoreAudioFormatDescriptionMap format_map[] = {
        {FMT_S16_LE, 16, sizeof (int16_t), kAudioFormatFlagIsSignedInteger},
        {FMT_S16_BE, 16, sizeof (int16_t), kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian},
        {FMT_S32_LE, 32, sizeof (int32_t), kAudioFormatFlagIsSignedInteger},
        {FMT_S32_BE, 32, sizeof (int32_t), kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian},
        {FMT_FLOAT,  32, sizeof (float),   kAudioFormatFlagIsFloat},
    };

    static AudioComponent output_comp;
    static AudioComponentInstance output_instance;

    bool init (void)
    {
        /* open the default audio device */
        AudioComponentDescription desc;
        desc.componentType = kAudioUnitType_Output;
        desc.componentSubType = kAudioUnitSubType_DefaultOutput;
        desc.componentFlags = 0;
        desc.componentFlagsMask = 0;
        desc.componentManufacturer = kAudioUnitManufacturer_Apple;

        output_comp = AudioComponentFindNext (nullptr, & desc);
        if (! output_comp)
        {
            fprintf (stderr, "Failed to open default audio device.\n");
            return false;
        }

        if (AudioComponentInstanceNew (output_comp, & output_instance))
        {
            fprintf (stderr, "Failed to open default audio device.\n");
            return false;
        }

        return true;
    }

    void cleanup (void)
    {
        AudioComponentInstanceDispose (output_instance);
    }

    bool open_audio (int format, int rate, int chan, AURenderCallbackStruct * callback)
    {
        struct CoreAudioFormatDescriptionMap * m = nullptr;

        for (struct CoreAudioFormatDescriptionMap it : format_map)
        {
            if (it.type == format)
            {
                m = & it;
                break;
            }
        }

        if (! m)
        {
            fprintf (stderr, "The requested audio format %d is unsupported.\n", format);
            return false;
        }

        if (AudioUnitInitialize (output_instance))
        {
            fprintf (stderr, "Unable to initialize audio unit instance\n");
            return false;
        }

        AudioStreamBasicDescription streamFormat;
        streamFormat.mSampleRate = rate;
        streamFormat.mFormatID = kAudioFormatLinearPCM;
        streamFormat.mFormatFlags = m->flags;
        streamFormat.mFramesPerPacket = 1;
        streamFormat.mChannelsPerFrame = chan;
        streamFormat.mBitsPerChannel = m->bits_per_sample;
        streamFormat.mBytesPerPacket = chan * buffer_bytes_per_channel;
        streamFormat.mBytesPerFrame = chan * buffer_bytes_per_channel;

        printf ("Stream format:\n");
        printf (" Channels: %d\n", streamFormat.mChannelsPerFrame);
        printf (" Sample rate: %f\n", streamFormat.mSampleRate);
        printf (" Bits per channel: %d\n", streamFormat.mBitsPerChannel);
        printf (" Bytes per frame: %d\n", streamFormat.mBytesPerFrame);

        if (AudioUnitSetProperty (output_instance, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamFormat, sizeof(streamFormat)))
        {
            fprintf (stderr, "Failed to set audio unit input property.\n");
            return false;
        }

        if (AudioUnitSetProperty (output_instance, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, callback, sizeof (AURenderCallbackStruct)))
        {
            fprintf (stderr, "Unable to attach an IOProc to the selected audio unit.\n");
            return false;
        }

        if (AudioOutputUnitStart (output_instance))
        {
            fprintf (stderr, "Unable to start audio unit.\n");
            return false;
        }

        return true;
    }

    void close_audio (void)
    {
        AudioOutputUnitStop (output_instance);
    }

    /* value is 0..100, the actual applied volume is based on a natual decibel scale. */
    void set_volume (int value)
    {
        float factor = (value == 0) ? 0.0 : powf (10, (float) VOLUME_RANGE * (value - 100) / 100 / 20);

        /* lots of pain concerning controlling application volume can be avoided with this one neat trick... */
        AudioUnitSetParameter (output_instance, kHALOutputParam_Volume, kAudioUnitScope_Global, 0, factor, 0);
    }

    void pause_audio (bool paused)
    {
        if (paused)
            AudioOutputUnitStop (output_instance);
        else
        {
            if (AudioOutputUnitStart (output_instance))
            {
                fprintf (stderr, "Unable to restart audio unit after pausing.\n");
                close_audio ();
            }
        }
    }
} /* namespace coreaudio_example */
