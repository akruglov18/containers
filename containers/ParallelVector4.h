#pragma once
#include <mutex>
#include <shared_mutex>
#include <atomic>

template<typename T>
class ParallelVector4
{
private:
    T* data;
    std::atomic_size_t size;
    size_t capacity;
    std::shared_mutex mut;
public:
    ParallelVector4() : data(nullptr), size(0), capacity(0)
    {
    }

    void push_back(const T& value)
    {
        size_t num_elem = size.fetch_add(1);

        if (num_elem >= capacity)
        {
            std::unique_lock<std::shared_mutex> lock(mut);
            if (num_elem >= capacity) {
                size_t new_capacity = max(capacity * 2 + 1, num_elem + 1); // max_elem
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
        std::shared_lock<std::shared_mutex> lock(mut);
        data[num_elem] = value;
        size++;
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

    ~ParallelVector4()
    {
        delete[] data;
    }
};
