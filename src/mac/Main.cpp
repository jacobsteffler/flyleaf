#include "Audio.h"
#include "coreaudio_example.h"

#include <cstdint>
#include <fstream>
#include <iostream>

Audio *a;

OSStatus test(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{
    a->get_samples((uint8_t **) &(ioData->mBuffers[0].mData), ioData->mBuffers[0].mDataByteSize / 8);

    return 0;
}

AURenderCallbackStruct blah;

int main(int argc, const char *argv[])
{
    a = new Audio("https://www2.iis.fraunhofer.de/AAC/ChID-BLITS-EBU-Narration.mp4", AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S32, 48000);

    blah.inputProc = test;
    blah.inputProcRefCon = NULL;

    coreaudio_example::init();

    coreaudio_example::open_audio(coreaudio_example::FMT_S32_LE, 48000, 2, &blah);

    sleep(100);

    return 0;
}
