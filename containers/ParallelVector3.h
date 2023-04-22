#pragma once

#include <mutex>
#include <list>
#include <atomic>
#include <shared_mutex>

template<typename T, size_t block_size>
class ParallelVector3
{
private:
    T** data;
    size_t size;
    size_t capacity;
    size_t ptr_count;
    std::shared_mutex mut;

public:
    ParallelVector3() : data(nullptr), size(0), capacity(0), ptr_count(0)
    {
    }

    void push_back(const T& val)
    {
        std::unique_lock<std::shared_mutex> lock(mut);
        if (size == capacity) {
            addNewBlock();
        }
        data[size / block_size][size % block_size] = val;
        size++;
    }

    T& operator[](std::size_t i)
    {
        std::shared_lock<std::shared_mutex> lock(mut);
        return data[i / block_size][i % block_size];
    }

    const T& operator[](std::size_t i) const
    {
        std::shared_lock<std::shared_mutex> lock(mut);
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

    ~ParallelVector3()
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
        T* newBlock = new T[block_size];
        data[ptr_count] = newBlock;
        ptr_count++;
        capacity += block_size;
    }
};
