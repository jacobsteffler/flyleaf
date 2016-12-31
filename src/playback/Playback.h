#include <string>

namespace Playback
{
    void init();
    void open_stream(const std::string &path);
    void play(bool play);
}
