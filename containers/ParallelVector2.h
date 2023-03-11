#pragma once
#include <mutex>
#include <shared_mutex>

template<typename T>
class ParallelVector2
{
private:
    T* data;
    size_t size;
    size_t capacity;
    std::shared_mutex mut;
public:
    ParallelVector2() : data(nullptr), size(0), capacity(0)
    {
    }

    void push_back(const T& value)
    {
        std::unique_lock<std::shared_mutex> lock(mut);
        if (size == capacity)
        {
            size_t new_capacity = capacity * 2 + 1;
            T* new_data = new int[new_capacity];
            for (int i = 0; i < size; i++)
            {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
            capacity = new_capacity;
        }

        data[size] = value;
        size++;
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
    
    void write(size_t index, const T& value)
    {
        std::shared_lock<std::shared_mutex> lock(mut);
        data[index] = value;
    }

    T read(size_t index)
    {
        std::shared_lock<std::shared_mutex> lock(mut);
        return data[index];
    }

    ~ParallelVector2()
    {
        delete[] data;
    }
};
