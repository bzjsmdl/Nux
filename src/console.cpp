#include "include/console.h" 
void Console() {
    while (true) {
        sleep(10);
        std::string msg = nux::fmt::input("> ");
    }
}