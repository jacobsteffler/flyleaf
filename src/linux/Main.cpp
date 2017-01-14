#include "Audio.h"

#include <cstdint>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[])
{
    AudioDecoder *d;
    
    d = new AudioDecoder();
    delete d;

    d = new AudioDecoder();
    if(argc > 1) d->open_file(argv[1]);
    else d->open_file("https://www2.iis.fraunhofer.de/AAC/ChID-BLITS-EBU-Narration.mp4");

    if(argc > 1) d->open_file(argv[1]);
    else d->open_file("https://www2.iis.fraunhofer.de/AAC/ChID-BLITS-EBU-Narration.mp4");

    delete d;

    AudioResampler *r;

    r = new AudioResampler();
    delete r;

    r = new AudioResampler();
    if(argc > 1) r->open_file(argv[1], AV_CH_LAYOUT_7POINT1, AV_SAMPLE_FMT_FLT, 48000);
    else r->open_file("https://www2.iis.fraunhofer.de/AAC/ChID-BLITS-EBU-Narration.mp4", AV_CH_LAYOUT_7POINT1, AV_SAMPLE_FMT_FLT, 48000);

    if(argc > 1) r->open_file(argv[1], AV_CH_LAYOUT_7POINT1, AV_SAMPLE_FMT_FLT, 48000);
    else r->open_file("https://www2.iis.fraunhofer.de/AAC/ChID-BLITS-EBU-Narration.mp4", AV_CH_LAYOUT_7POINT1, AV_SAMPLE_FMT_FLT, 48000);

    delete r;

    Audio *a;
    
    a = new Audio();
    delete a;

    a = new Audio();
    if(argc > 1) a->open_file(argv[1], AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_FLT, 48000);
    else a->open_file("https://www2.iis.fraunhofer.de/AAC/ChID-BLITS-EBU-Narration.mp4", AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_FLT, 48000);

    std::ofstream out("audio.bin", std::ofstream::binary);

    float *samples = new float[48000*2];
    int count;
    while((count = a->get_samples((uint8_t **) &samples, 48000)) > 0)
    {
        out.write((char *) samples, count * sizeof(float) * 2);
    }

    delete a;
    delete [] samples;

    return 0;
}
