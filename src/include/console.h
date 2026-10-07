#pragma once

#include "cdlib.h"
#include "rt.h"
#include "server.h"

void Console() {
    while (true) {
        sleep(10);
        std::string msg = nux::fmt::input("> ");
    }
}