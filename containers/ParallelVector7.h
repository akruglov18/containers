#pragma once

#include <mutex>
#include <list>
#include <atomic>
#include <shared_mutex>
#include "Accessor.h"

template<typename T, size_t block_size>
class ParallelVector7
{
private:
    T** data;
    std::atomic_size_t size;
    std::atomic_size_t cur_elem;
    size_t capacity;
    size_t ptr_count;
    MySharedMutex mut;

public:
    ParallelVector7() : data(nullptr), size(0), cur_elem(0), capacity(0), ptr_count(0)
    {
    }

    void push_back(const T& val)
    {
        size_t num_elem = cur_elem.fetch_add(1);
        if (num_elem >= capacity)
        {
            MyUniqueLock lock(mut);
            while (num_elem >= capacity) {
                addNewBlock();
            }
        }
        MySharedLock lock(mut);
        data[num_elem / block_size][num_elem % block_size] = val;
        size++;
    }

    T& operator[](std::size_t i)
    {
        MySharedLock lock(mut);
        return data[i / block_size][i % block_size];
    }

    const T& operator[](std::size_t i) const
    {
        MySharedLock lock(mut);
        return data[i / block_size][i % block_size];
    }

    void clear()
    {
        for (size_t i = 0; i < ptr_count; i++)
        {
            delete[] data[i];
        }
        delete[] data;
        size = 0;
        capacity = 0;
        ptr_count = 0;
    }

    size_t get_size() const
    {
        return size;
    }

    ~ParallelVector7()
    {
        clear();
    }

private:
    void addNewBlock()
    {
        T** newData = new T * [ptr_count + 1];
        for (size_t i = 0; i < ptr_count; i++)
        {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        T* newBLock = new T[block_size];
        data[ptr_count] = newBLock;
        ptr_count++;
        capacity += block_size;
    }
};
