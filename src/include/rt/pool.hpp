#pragma once

#include "cdlib.h"

namespace nux {
    class Pool {
		private:
		alignas(64) void* ptr = nullptr;
		size_t pool_size = 1024;
		size_t used = 0;

		public:
		Pool(size_t N) : pool_size(N) {
			this->ptr = malloc(this->pool_size);
		}

		Pool() {
			this->ptr = malloc(this->pool_size);
		}

		void* allocate(size_t __size) {
			if (this->used + __size >= this->pool_size) return nullptr;
			void* ret = (char*)this->ptr + this->used;
			this->used += __size;
			return ret;
		}

		void* data() {
			return this->ptr;
		}

		size_t size() {
			return this->pool_size;
		}

		~Pool() {
			free(ptr);
		}
	};
}