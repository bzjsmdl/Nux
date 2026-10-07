#pragma once

#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stddef.h>
#include <alloca.h>

// Include Thrid-Party Library
// get more information: `include/README.md`!
#include "zconf.h"
#include "zip.h"
#include "json.hpp"


typedef struct tm tm;
typedef struct stat status;

namespace sfs = std::filesystem;

#define strequ(s, S) (strcmp(s, S) == 0)

typedef struct NuxTable {
	bool debug = false;
	bool color = isatty(fileno(stdout));
	bool LoadPlugin = false;
	bool werr = false;
	std::string plugins_path;
	std::string data_path;
	std::string ServerRootDir = getcwd(nullptr, 0);
	std::vector<std::string> plugins;
} Table;