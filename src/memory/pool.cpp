#include <cstring>
#include "pool.hpp"

using namespace Memory;

Pool<void>::Pool(size_t size, size_t alignment, size_t capacity)
{
    _size = size + size % alignment;
    _data = new uint8_t[_size * capacity];
}

Pool<void>::~Pool()
{
    delete[] _data;
}

void Pool<void>::resize(size_t capacity)
{
    size_t newCapacity = capacity * 2;
    uint8_t *newData = new uint8_t[_size * newCapacity];
    std::memcpy(newData, _data, _size * capacity);
    delete[] _data;
    _data = newData;
}
