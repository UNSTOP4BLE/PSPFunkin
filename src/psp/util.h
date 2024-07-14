
#pragma once

#include <cstddef>
#include <cstdint>

template<typename T> static inline T *allocateAligned(size_t length, size_t alignment) {
	auto ptr    = new T[length + alignment * 2 / sizeof(T)];
	auto offset = reinterpret_cast<uintptr_t>(ptr) % alignment;

	if (offset)
		ptr = reinterpret_cast<T *>(ptr + alignment - offset);

	return ptr;
}

static inline int roundToPowerOf2(int value) {
	int result = 1;

	while (result < value)
		result *= 2;

	return result;
}
