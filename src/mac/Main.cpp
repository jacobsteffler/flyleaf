#include "Playback.h"

#include <iostream>
#include <string>

int main(int argc, const char *argv[])
{
    Playback::init();
    Playback::open_stream(argc > 1 ? argv[1] : "/Users/jacob/Desktop/ascend.m4a");
    Playback::play(true);

    bool playing = true;
    char in;
    std::string path;

    do
    {
        std::cout << "Enter an option: ";
        std::cin >> in;

        if(in == 'q') break;

        if(in == 'p') Playback::play(playing = !playing);
        else if(in == 'o')
        {
            std::cin >> path;
            std::cout << path;
            Playback::open_stream(path);
        }
    }
    while(in != 'q');

    return 0;
}
