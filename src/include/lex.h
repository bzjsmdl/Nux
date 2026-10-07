#pragma once

#include "cdlib.h"
namespace lex {
	typedef struct Token {
		std::string txt;
		unsigned long long int line;
		unsigned long long int column;
	} Token;

    // clear text for java properties
    void clear_text_jp(char*& text, unsigned long long int length) {
		// Clear Text
		unsigned long long int line = 1, ls = 0;
		for (unsigned long long int i = 0; i < length; i++) {
			if (text[i] == '\n') {
				line++;
				ls = i;
			}
			if (memcmp("#", text + i, 2) == 0) {
				for (; i < length && text[i] != '\n'; i++) text[i] = 0;
			}
		}
	}

    // scanner for java properties
	std::vector<Token*> scanner_jp(char*& text, unsigned long long int length) {
		bool error = false;
		std::vector<Token*> tokens;
		char quote = 0;
		unsigned long long int start = 0, ls = 0, line = 1;
		bool str = false, nl = false, xdigit = false, digit = false;
		char buf[2] = {0};
		char* ptr = nullptr;
		Token* tok;
		for (unsigned long long int i = 0; i < length; i++) {
			buf[0] = text[i]; buf[1] = text[i + 1];
			if (buf[0] == '\n') {
				line ++;
				ls = i + 1;
				goto GetToken;
			}
			if (buf[0] < '!') goto clearNC;
			else if (isalpha(buf[0])) {
				if (isalnum(buf[1]) || buf[1] == '-') continue;
				else goto GetToken;
			}
			else if (isdigit(buf[0]) || (buf[0] == '.' && !xdigit && digit)) {
				char chr = buf[1] & 0b11011111;
				digit = true;
				if (isdigit(buf[1]) || buf[1] == '.' || (chr <= 'F' && chr >= 'A') || (buf[0] != '.' && chr == 'X')) {
					if ((chr <= 'F' && chr >= 'A') || chr == 'X') xdigit = true;
					continue;
				}
				else goto GetToken;
			}
			else if (ispunct(buf[0])) {
				if (ispunct(buf[1]) && buf[1] != '-') continue;
				else goto GetToken;
			}
			else continue;
			GetToken:
				ptr = new char[i - start + 2]{0};
				strncpy(ptr, text + start, i - start + 1);
				tok->line = line;
				tok->column = start - ls + 1;
				tok->txt = ptr;
				tokens.push_back(std::move(tok));
				delete ptr;
			clearNC:
				start = i + 1;
				digit = false;
				continue;
			s:
				str = !str;
				if (!str) goto GetToken;
				else quote = buf[0];
		}
		if (error) tokens.clear();
		return tokens;
	}
}