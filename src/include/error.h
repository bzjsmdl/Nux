#pragma once

#include "cdlib.h"
#include "rt.h"

extern Table* t;

#define ALLOCATE_MEMORY_FAIL(debug_msg) {\
    nux::log::error("Server failed when it tries to allocate memory");\
    if (t->debug) nux::log::debug(debug_msg);\
}

#define CANNOT_OPEN_FILE(file, debug_msg) {\
    nux::log::error((std::string)"Server failed when it tries to open file " + file);\
    if (t->debug) nux::log::debug(debug_msg);\
}

#define CPPRT_THROW_EXCEPTION_FAIL(msg) {\
    nux::log::error((std::string)"Server failed because " + msg);\
}

#define CONFIG_PARSE_WARN(debug_msg) {\
    nux::log::warning((std::string)"Server threw a warning when it tried to parse config file");\
    if (t->debug) nux::log::debug(debug_msg);\
}

#define CONFIG_PARSE_FAIL(debug_msg) {\
    nux::log::error("Server failed when it tried to parse config file");\
    if (t->debug) nux::log::debug(debug_msg);\
}

enum ErrorCode {    // rt throw error
	NoError,
    VectorTooSmall
};