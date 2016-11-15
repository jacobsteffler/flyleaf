#ifndef PLAYCONTROLLER_H
#define PLAYCONTROLLER_H

#include <string>

namespace controller {
    void init();
    void load(const std::string &path);
    void play();
    void pause();
}

#endif
