#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>

#include <mutex>

#include "Playback.h"
#include "Audio.h"

namespace Playback {
    struct CoreAudioFormatDescriptionMap
    {
        AVSampleFormat format;
        int bits_per_sample;
        int bytes_per_sample;
        unsigned int flags;
    };
    
    OSStatus render_callback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags,
                             const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                             UInt32 inNumberFrames, AudioBufferList *ioData);
}
