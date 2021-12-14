#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <cstdint>

template <typename T>
class MemoryPool;

template <>
class MemoryPool<void>
{
    uint8_t *_data;
    size_t _size;

public:
    MemoryPool(size_t size, size_t alignment, size_t capacity)
    {
        _size = size + size % alignment;
        _data = new uint8_t[_size * capacity];
    }

    ~MemoryPool()
    {
        delete[] _data;
    }

    inline void *get(size_t index)
    {
        return _data + index * _size;
    }
};

template <typename T>
class MemoryPool: public MemoryPool<void>
{
public:
    MemoryPool(size_t capacity);

    inline T *get(size_t index);
};

template <typename T>
inline MemoryPool<T>::MemoryPool(size_t capacity)
    : MemoryPool<void>(sizeof(T), alignof(T), capacity) {}

template <typename T>
inline T *MemoryPool<T>::get(size_t index) {
    return static_cast<T *>(MemoryPool<void>::get(index));
}

#endif /* MEMORY_POOL_HPP */
