#include "../include/rt.h"

namespace nux::fmt {
	void print(std::string __msg) {
		unsigned long long int len = __msg.size();
		std::string msg;
		bool strong = false, strikethrough = false, underline = false, bg = false;
		unsigned int color_code = 9;
		for (unsigned long long int i = 0; i < len; i++) {
			if (__msg[i] == '$') {
				switch (__msg[++i]) {
					case 'S':
						strong = true;
						break;
					case 's':
						strikethrough = true;
						break;
					case 'u':
						underline = true;
						break;	
                    case 'r':
                        msg.append("\x1b[0m");
                        continue;
					default:
						char tmp_str[2] = {__msg[i], 0};
						if (isdigit(__msg[i])) color_code = atoi(tmp_str);
						color_code += ((strong) ? 90 : 30);
						if (__msg[i + 1] == 'b') color_code += 10;
				}
				if (__msg[i + 1] != '$') {
					msg.append("\x1b[");
					if (strong) msg += "1;";
					if (underline) msg += "4;";
					if (strikethrough) msg += "9;";
					msg += (std::to_string(color_code) + "m");
				}
			}
			else if (__msg[i] == '\\') {
				switch (__msg[++i]) {
					case '\\':
						msg.append("\\");
						break;
					case 'n':
						msg.append("\n");
						break;
					case '\'':
						msg.append("\'");
						break;
					case '\"':
						msg.append("\"");
						break;
					case 'a':
						msg.append("\a");
						break;
					case 'r':
						msg.append("\r");
						break;
					case 'v':
						msg.append("\v");
						break;
					case 't':
						msg.append("\t");
						break;
					case 'b':
						msg.append("\b");
						break;
					case '$':
						msg.append("$");
						break;
				}
			}
			else msg += __msg[i];
		}
		printf("%s", msg.c_str());
	}
	std::string input(std::string tip) {
		printf("%s", tip.c_str());
		std::string msg;
		std::cin >> msg;
		return msg;
	}
}