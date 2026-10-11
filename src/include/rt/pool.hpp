#pragma once

#include "cdlib.h"

namespace nux {
    class Pool {
		private:
		void* ptr = nullptr;
		size_t len = 1024;
		size_t used = 0;

		public:
		Pool(size_t N) : len(N) {
			this->ptr = aligned_alloc(64, this->len);
		}

		Pool() {
			this->ptr = malloc(this->len);
		}

		void* allocate(size_t __size) {
			// yoiu can go to https://github.com/bzjsmdl/Nux/blob/main/src/include/rt/reader.hpp#L17 if you want to know why i write `__size > this->len - this->used`
			if (__size > this->len - this->used) return nullptr;
			void* ret = (char*)this->ptr + this->used;
			this->used += __size;
			return ret;
		}

		void* data() {
			return this->ptr;
		}

		size_t size() {
			return this->len;
		}

		~Pool() {
			free(ptr);
		}
	};
}