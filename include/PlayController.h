#ifndef PLAYCONTROLLER_H
#define PLAYCONTROLLER_H

#include <string>

class PlayController
{
    public:

    PlayController();
    PlayController(const PlayController &p) = delete;
    ~PlayController();

    void load(const std::string &path);
    void play();
    void pause();
};

#endif
