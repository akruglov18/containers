#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <algorithm>
#include <bit>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <string>

template<typename T>
class ConcurrentVector
{
private:
    static constexpr int BLOCK_NUM = 64;
    std::atomic<T*> blocks[BLOCK_NUM];
    std::atomic<size_t> _size;
    std::atomic<size_t> _capacity;

public:
    ConcurrentVector() : _size(0), _capacity(0) {
        for (size_t i = 1; i < BLOCK_NUM; i++) {
            blocks[i] = nullptr;
        }
        blocks[0] = new T[1];
        _capacity = 1;
    }

    size_t size() const {
        return std::min(_size.load(std::memory_order_relaxed), _capacity.load(std::memory_order_relaxed));
    }

    T& operator[](size_t ind) {
        size_t block_num = std::bit_width(ind);
        size_t offset = 0;
        if (block_num > 0) {
            offset = (1ll << (block_num - 1));
        }
        T* block = blocks[block_num].load(std::memory_order_acquire);
        return block[ind - offset];
    }

    void push_back(const T& val) {
        size_t my_index = _size.fetch_add(1, std::memory_order_relaxed);

        while (my_index >= _capacity.load(std::memory_order_relaxed)) {
            size_t block_num = std::bit_width(_capacity.load(std::memory_order_relaxed));
            size_t new_block_size = (1ll << (block_num - 1));
            T* new_block = new T[new_block_size];
            T* old_block = nullptr;
            if (blocks[block_num].compare_exchange_strong(old_block, new_block, std::memory_order_relaxed)) {
                _capacity.fetch_add(new_block_size, std::memory_order_release);
            } else {
                delete[] new_block;
            }
        }

        this->operator[](my_index) = val;
    }

    void clear() {
        for (size_t i = 0; i < BLOCK_NUM; i++) {
            delete[] blocks[i];
        }
    }

    ~ConcurrentVector() {
        clear();
    }
};