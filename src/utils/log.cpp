#include "../include/rt.h"

extern Table* t;
static std::mutex LogMutex;

namespace nux {
	namespace log {
		void debug(std::string __msg) {
			std::lock_guard<std::mutex> lock(LogMutex);
			time_t __time = time(nullptr);
			tm* _time = localtime(&__time);
			std::string time_str;
			if (_time) {
				std::string sec = std::to_string(_time->tm_sec),\
							min = std::to_string(_time->tm_min), \
							hour = std::to_string(_time->tm_hour);
				if (sec.length() == 1) sec.insert(sec.begin(), '0');
				if (min.length() == 1) min.insert(min.begin(), '0');
				if (hour.length() == 1) hour.insert(hour.begin(), '0');
				time_str += std::to_string(_time->tm_year + 1900) + '-' + std::to_string(_time->tm_mon + 1) + '-'\
				+ std::to_string(_time->tm_mday) + ' ' + hour + ':' + min + ':' + sec;
			}
			else {
				time_str += "XXXX-X-X XX:XX:XX";
			}
			std::string msg = "$3$S" + time_str + "$6 [DUBUG]$r " + __msg + "\n";
			if (!t->color) {
				for (size_t i = 0; i < msg.size(); i++) {
					if (msg[i] == '$' && ((i != 0 && msg[i - 1] != '\\') || i == 0)) {
						msg.erase(msg.begin() + i, msg.begin() + i + 2);
						i--;
					}
				}
			}
			nux::fmt::print(msg);
		}
		void info(std::string __msg) {
			std::lock_guard<std::mutex> lock(LogMutex);
			time_t __time = time(nullptr);
			tm* _time = localtime(&__time);
			std::string time_str;
			if (_time) {
				std::string sec = std::to_string(_time->tm_sec),\
							min = std::to_string(_time->tm_min), \
							hour = std::to_string(_time->tm_hour);
				if (sec.length() == 1) sec.insert(sec.begin(), '0');
				if (min.length() == 1) min.insert(min.begin(), '0');
				if (hour.length() == 1) hour.insert(hour.begin(), '0');
				time_str += std::to_string(_time->tm_year + 1900) + '-' + std::to_string(_time->tm_mon + 1) + '-'\
				+ std::to_string(_time->tm_mday) + ' ' + hour + ':' + min + ':' + sec;
			}
			else {
				time_str += "XXXX-X-X XX:XX:XX";
			}
			std::string msg = "$3$S" + time_str + "$r$4 [INFO]$r " + __msg + "\n";
			if (!t->color) {
				for (size_t i = 0; i < msg.size(); i++) {
					if (msg[i] == '$' && ((i != 0 && msg[i - 1] != '\\') || i == 0)) {
						msg.erase(msg.begin() + i, msg.begin() + i + 2);
						i--;
					}
				}
			}
			nux::fmt::print(msg);
		}
		void error(std::string __msg) {
			std::lock_guard<std::mutex> lock(LogMutex);
			time_t __time = time(nullptr);
			tm* _time = localtime(&__time);
			std::string time_str;
			if (_time) {
				std::string sec = std::to_string(_time->tm_sec),\
							min = std::to_string(_time->tm_min), \
							hour = std::to_string(_time->tm_hour);
				if (sec.length() == 1) sec.insert(sec.begin(), '0');
				if (min.length() == 1) min.insert(min.begin(), '0');
				if (hour.length() == 1) hour.insert(hour.begin(), '0');
				time_str += std::to_string(_time->tm_year + 1900) + '-' + std::to_string(_time->tm_mon + 1) + '-'\
				+ std::to_string(_time->tm_mday) + ' ' + hour + ':' + min + ':' + sec;
			}
			else {
				time_str += "XXXX-X-X XX:XX:XX";
			}
			std::string msg = "$3$S" + time_str + "$r$1 [ERROR]$r " + __msg + "\n";
			if (!t->color) {
				for (size_t i = 0; i < msg.size(); i++) {
					if (msg[i] == '$' && ((i != 0 && msg[i - 1] != '\\') || i == 0)) {
						msg.erase(msg.begin() + i, msg.begin() + i + 2);
						i--;
					}
				}
			}
			nux::fmt::print(msg);
		}
		void warning(std::string __msg) {
			std::lock_guard<std::mutex> lock(LogMutex);
			time_t __time = time(nullptr);
			tm* _time = localtime(&__time);
			std::string time_str;
			if (_time) {
				std::string sec = std::to_string(_time->tm_sec),\
							min = std::to_string(_time->tm_min), \
							hour = std::to_string(_time->tm_hour);
				if (sec.length() == 1) sec.insert(sec.begin(), '0');
				if (min.length() == 1) min.insert(min.begin(), '0');
				if (hour.length() == 1) hour.insert(hour.begin(), '0');
				time_str += std::to_string(_time->tm_year + 1900) + '-' + std::to_string(_time->tm_mon + 1) + '-'\
				+ std::to_string(_time->tm_mday) + ' ' + hour + ':' + min + ':' + sec;
			}
			else {
				time_str += "XXXX-X-X XX:XX:XX";
			}
			std::string msg = "$3$S" + time_str + "$r$5 [WARNING]$r " + __msg + "\n";
			if (!t->color) {
				for (size_t i = 0; i < msg.size(); i++) {
					if (msg[i] == '$' && ((i != 0 && msg[i - 1] != '\\') || i == 0)) {
						msg.erase(msg.begin() + i, msg.begin() + i + 2);
						i--;
					}
				}
			}
			nux::fmt::print(msg);
		}
	}
}