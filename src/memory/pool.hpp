#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <cstddef>
#include <cstdint>

namespace Memory
{
    template <typename T> class Pool;

    template <> class Pool<void>
    {
        uint8_t* _data;
        size_t _size;

    public:
        Pool(size_t size, size_t alignment, size_t capacity);
        ~Pool();

        void resize(size_t capacity);

        inline void* get(size_t index)
        {
            return _data + index * _size;
        }
    };

    template <typename T> class Pool : public Pool<void>
    {
    public:
        Pool(size_t capacity);

        inline T* get(size_t index);
    };

    template <typename T> inline Pool<T>::Pool(size_t capacity) : Pool<void>(sizeof(T), alignof(T), capacity)
    {
    }

    template <typename T> inline T* Pool<T>::get(size_t index)
    {
        return static_cast<T*>(Pool<void>::get(index));
    }
} // namespace Memory
#endif /* MEMORY_POOL_HPP */
