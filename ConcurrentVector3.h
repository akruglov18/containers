#pragma once
#include <mutex>
#include <shared_mutex>

template<typename T>
class Vector {
private:
    T* data;
    size_t size;
    size_t capacity;
    std::shared_mutex mutex_;
public:
    Vector() : data(nullptr), size(0), capacity(0) {

    }

    void push_back(const T& value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (size == capacity) {
            size_t new_capacity = capacity * 2 + 1;
            T* new_data = new int[new_capacity];
            for (int i = 0; i < size; i++) {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
            capacity = new_capacity;
        }

        data[size] = value;
        size++;
    }

    size_t get_size() const {
        return size;
    }

    T& operator[](size_t i) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return data[i];
    }

    ~Vector() {
        delete[] data;
    }
};
