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
				// check the range of reading.
				// if use `this->pos + size >= this->pool_size`:
				// if the size of pool is 8, this memory's layout: |0|1|2|3|4|5|6|7|
				// if this->pos == 0, read<int>() => sizeof(int) = 4, this->pos + 4 = 4
				// and if this->pos == -4, read<int>() => sizeof(int) = 4, this->pos + 4 = 0
				// in this expression, it's valid -- but we expect it's invalid!!!
				// if use `size > this->pool_size - this->pos`
				// when this->pos == 0, read<int>() => sizeof(int) = 4, this->pool_size + this->pos = 8 - 0 = 4 (valid)
				// when this->pos == -4, read<int>() => sizeof(int) = 4, this->pool_size + this->pos = 8 - (-4) = 12 (invalid)
				// because of this, use `size > this->pool_size - this->pos` in here
				if (size > this->pool_size - this->pos) throw ErrorCode::VectorTooSmall;
				T ret;
				memcpy(&ret, (this->data + this->pos), size);
				seek(size);
				return ret;
			}
		public:
			Reader(Pool& __data, size_t __pos = 0) : pos(__pos) {
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