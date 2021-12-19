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
