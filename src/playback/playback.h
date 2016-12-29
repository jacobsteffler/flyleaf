#include <string>

namespace Playback
{
    enum format_type {
        FMT_S16_LE,
        FMT_S16_BE,
        FMT_S32_LE,
        FMT_S32_BE,
        FMT_FLOAT
    };
    
    void init();
    void open_stream(const std::string &path);
    void play(bool play);
}
