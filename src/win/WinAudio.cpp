//-----------------------------------------------------------
// Play an audio stream on the default audio rendering
// device. The PlayAudioStream function allocates a shared
// buffer big enough to hold one second of PCM audio data.
// The function uses this buffer to stream data to the
// rendering device. The inner loop runs every 1/2 second.
//-----------------------------------------------------------

#include "Audio.h"

#include <mmdeviceapi.h>
#include <Audioclient.h>

AVSampleFormat WinFmt(WAVEFORMATEX *fmt)
{
    WAVEFORMATEXTENSIBLE *xfmt;

    if(fmt->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        xfmt = (WAVEFORMATEXTENSIBLE *) fmt;

        if(xfmt->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
        {
            if(fmt->wBitsPerSample == 32) return AV_SAMPLE_FMT_FLT;
            if(fmt->wBitsPerSample == 64) return AV_SAMPLE_FMT_DBL;
            throw 12;
        }

        if(xfmt->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
        {
            if(fmt->wBitsPerSample == 8) return AV_SAMPLE_FMT_U8;
            if(fmt->wBitsPerSample == 16) return AV_SAMPLE_FMT_S16;
            if(fmt->wBitsPerSample == 32) return AV_SAMPLE_FMT_S32;
            if(fmt->wBitsPerSample == 64) return AV_SAMPLE_FMT_S64;
            throw 13;
        }
    }

    if(fmt->wFormatTag != WAVE_FORMAT_PCM) throw 10;
    if(fmt->wBitsPerSample != 8 && fmt->wBitsPerSample != 16) throw 11;

    if(fmt->wBitsPerSample == 8) return AV_SAMPLE_FMT_U8;
    else return AV_SAMPLE_FMT_S16;
}

Audio *WinAllocAudio(const std::string &filename, WAVEFORMATEX *fmt)
{
    return new Audio(filename, AV_CH_LAYOUT_STEREO, WinFmt(fmt), fmt->nSamplesPerSec);
}

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

HRESULT PlayAudioStream(const std::string &filename)
{
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioRenderClient *pRenderClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE *pData;
    DWORD flags = 0;
    Audio *pMySource;

    CoInitialize(nullptr);

    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

    hr = pEnumerator->GetDefaultAudioEndpoint(
                        eRender, eConsole, &pDevice);
    EXIT_ON_ERROR(hr)

    hr = pDevice->Activate(
                    IID_IAudioClient, CLSCTX_ALL,
                    NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         0,
                         hnsRequestedDuration,
                         0,
                         pwfx,
                         NULL);
    EXIT_ON_ERROR(hr)

    // Tell the audio source which format to use.
    //hr = pMySource->SetFormat(pwfx);
    pMySource = WinAllocAudio(filename, pwfx);
    //EXIT_ON_ERROR(hr)

    // Get the actual size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService(
                         IID_IAudioRenderClient,
                         (void**)&pRenderClient);
    EXIT_ON_ERROR(hr)

    // Grab the entire buffer for the initial fill operation.
    hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
    EXIT_ON_ERROR(hr)

    // Load the initial data into the shared buffer.
    uint8_t **buf = (uint8_t **) &pData;
    hr = pMySource->get_samples(buf, bufferFrameCount);
    if(hr == 0) flags = AUDCLNT_BUFFERFLAGS_SILENT;
    else
    {
        flags = 0;
        if(hr < bufferFrameCount) memset(pData + hr, 0, bufferFrameCount - hr);
    }
    EXIT_ON_ERROR(hr)

    hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
    EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                        bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();  // Start playing.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
    while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {
        // Sleep for half the buffer duration.
        Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));

        // See how much buffer space is available.
        hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
        EXIT_ON_ERROR(hr)

        numFramesAvailable = bufferFrameCount - numFramesPadding;

        // Grab all the available space in the shared buffer.
        hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
        EXIT_ON_ERROR(hr)

        // Get next 1/2-second of data from the audio source.
        hr = pMySource->get_samples(&pData, numFramesAvailable);
        if(hr == 0) flags = AUDCLNT_BUFFERFLAGS_SILENT;
        else
        {
            flags = 0;
            if(hr < numFramesAvailable)
            {
                memset(pData + hr * 2, 0, 2 * (bufferFrameCount - hr));
            }
        }
        EXIT_ON_ERROR(hr)

        hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
        EXIT_ON_ERROR(hr)
    }

    // Wait for last data in buffer to play before stopping.
    Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));

    hr = pAudioClient->Stop();  // Stop playing.
    EXIT_ON_ERROR(hr)

Exit:
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pRenderClient)

    //if(pMySource) delete pMySource;

    return hr;
}
