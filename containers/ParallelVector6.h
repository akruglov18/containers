#pragma once
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <algorithm>
#include "Accessor.h"

template<typename T>
class ParallelVector6
{
private:
    T* data;
    std::atomic_size_t size;
    size_t capacity;
    MySharedMutex mut;
public:
    ParallelVector6() : data(nullptr), size(0), capacity(0)
    {
    }

    void push_back(const T& value)
    {
        size_t num_elem = size.fetch_add(1);

        if (num_elem >= capacity)
        {
            MyUniqueLock lock(mut);
            if (num_elem >= capacity) {
                size_t new_capacity = std::max(capacity * 2 + 1, num_elem + 1); // max_elem
                T* new_data = new int[new_capacity];
                for (int i = 0; i < capacity; i++)
                {
                    new_data[i] = data[i];
                }
                delete[] data;
                data = new_data;
                capacity = new_capacity;
            }
        }
        MySharedLock lock(mut);
        data[num_elem] = value;
        size++;
    }

    void write(size_t index, const T& value)
    {
        MySharedLock lock(mut);
        data[index] = value;
    }

    T read(size_t index)
    {
        MySharedLock lock(mut);
        return data[index];
    }

    size_t get_size() const
    {
        return size;
    }

    T& operator[](size_t i)
    {
        return data[i];
    }

    const T& operator[](size_t i) const
    {
        return data[i];
    }

    ~ParallelVector6()
    {
        delete[] data;
    }
};
