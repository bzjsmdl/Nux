#pragma once

#include "cdlib.h"
#include "rt/pool.hpp"

namespace nux {
	namespace log {
		void debug(std::string __msg);
		void info(std::string __msg);
		void error(std::string __msg);
		void warning(std::string __msg);
	}
	namespace fs {
		inline bool exists(std::string p) {
			status st;
			return stat(p.c_str(), &st) == 0;
		}
		inline bool isdir(std::string p) {
			status st;
			return (stat(p.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) ? true : false;
		}
		inline size_t flen(FILE* f) {
			size_t nfp = ftell(f);
			fseek(f, 0, SEEK_END);
			size_t len = ftell(f);
			fseek(f, nfp, SEEK_SET);
			return len;
		}
	}
	namespace fmt {
		/*	$ + command_charater
			-- Color Text --
				Black	0
				Red		1
				Green	2
				Yellow	3
				Blue	4
				Purple	5
				Cyan	6
				White	7
				Default	9
			---- Contorl Text ----
				Strong      	S
			StrikeThrough   	s
				Underline   	u
			Reset All States	r
		*/
		void print(std::string __msg);
		std::string input(std::string tip);
	}
}