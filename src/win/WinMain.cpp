#include <string>

long PlayAudioStream(const std::string &filename);

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        PlayAudioStream("https://www2.iis.fraunhofer.de/AAC/ChID-BLITS-EBU-Narration.mp4");
        //PlayAudioStream("C:\\Users\\Jacob\\Desktop\\vo.m4a");
    }
    else
    {
        for(int i = 1; i < argc; ++i)
        {
            PlayAudioStream(argv[i]);
        }
    }

    return 0;
}
