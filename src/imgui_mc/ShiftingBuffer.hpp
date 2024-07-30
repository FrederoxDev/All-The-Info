#pragma once
#include <vector>
#include <algorithm>

template <typename T>
class ShiftingBuffer {
private:
	std::vector<T> mData;
	size_t mSize;

public:
	ShiftingBuffer(size_t size) : mSize(size) {
		mData.reserve(size);
	}

	void addValue(T value) {
		if (mData.size() < mSize) {
			mData.push_back(value);
		}
		else {
			// Shift all elements to the left
			std::copy(mData.begin() + 1, mData.end(), mData.begin());
			mData[mSize - 1] = value;
		}
	}

	T* data() {
		return mData.data();
	}

	size_t dataSize() {
		return mData.size();
	}

	size_t maxSize() {
		return mSize;
	}
};