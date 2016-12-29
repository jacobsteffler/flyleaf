#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>

#include "playback.h"

namespace Playback {
    struct CoreAudioFormatDescriptionMap
    {
        format_type type;
        int bits_per_sample;
        int bytes_per_sample;
        unsigned int flags;
    };
    
    OSStatus render_callback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags,
                             const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                             UInt32 inNumberFrames, AudioBufferList *ioData);
}
