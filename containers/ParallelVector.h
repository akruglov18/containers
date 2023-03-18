#pragma once
#include <mutex>
#include <atomic>

struct Accessor
{
    std::atomic<int>& counter;
    Accessor(std::atomic<int>& counter) : counter(counter)
    {
    }

    ~Accessor()
    {
        counter--;
    }
};

template<typename T>
class ParallelVector
{
private:
    T* data;
    size_t size;
    size_t capacity;
    bool flag;
    std::atomic<int> cnt;
    std::mutex growth;
public:
    ParallelVector() : data(nullptr), size(0), capacity(0), flag(false), cnt(0)
    {
    }

    void push_back(const T& value)
    {
        growth.lock();

        if (size == capacity)
        {
            flag = true;
            while (cnt != 0) {}
            size_t new_capacity = capacity * 2 + 1;
            T* new_data = new int[new_capacity];
            for (int i = 0; i < size; i++)
            {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
            capacity = new_capacity;
            flag = false;
        }

        data[size] = value;
        size++;

        growth.unlock();
    }

    size_t get_size() const
    {
        return size;
    }

    T& operator[](size_t i)
    {
        return data[i];
    }

    void write(size_t index, const T& value)
    {
        Accessor accessor(cnt);
        while (true)
        {
            while (flag) {}
            cnt++;
            if (flag)
            {
                cnt--;
            }
            else
            {
                break;
            }
        }
        data[index] = value;
    }

    T read(size_t index)
    {
        Accessor accessor(cnt);
        while (true)
        {
            while (flag) {}
            cnt++;
            if (flag)
            {
                cnt--;
            }
            else
            {
                break;
            }
        }
        return data[index];
    }

    ~ParallelVector()
    {
        delete[] data;
    }
};
