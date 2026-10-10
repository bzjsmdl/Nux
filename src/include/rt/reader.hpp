#pragma once

#include "cdlib.h"
#include "pool.hpp"
#include "../error.h"

namespace nux {
	class Reader {
		private:
			const uint8_t* data;
			size_t pool_size;
			size_t pos;

			template <typename T>
			T read() {
				size_t size = sizeof(T);
				if (this->pos + size >= pool_size) throw ErrorCode.VectorTooSmall;
				T ret = *(T*)(this->data + this->pos);
				return ret;
			}
		public:
			Reader(Pool __data, size_t __pos = 0) : pos(__pos) {
				this->pool_size = __data.size();
				this->data = (const uint8_t*)__data.data();
			}

			void seek(signed long int offset) {
				this->pos += offset;
			}

			size_t tell() {
				return this->pos;
			}

			uint8_t read_u8() {
				return read<uint8_t>();
			}

			int read_int() {
				return read<int>();
			}
	};
}