#pragma once
#include <atomic>
#include <mutex>

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

struct MySharedMutex {
public:
    std::atomic<int> counter;
    bool flag = false;
    std::mutex mut;
};

class MyUniqueLock {
private:
    MySharedMutex &mut;
public:
    MyUniqueLock(MySharedMutex &mutex) : mut(mutex) {
        mut.mut.lock();
        mut.flag = true;
        while (mut.counter != 0) {}
    }
    
    ~MyUniqueLock() {
        mut.flag = false;
        mut.mut.unlock();
    }
};

class MySharedLock {
private:
    MySharedMutex &mut;

public:
    MySharedLock(MySharedMutex &mutex) : mut(mutex) {
        while (true)
        {
            while (mut.flag) {}
            mut.counter++;
            if (mut.flag)
            {
                mut.counter--;
            }
            else
            {
                break;
            }
        }
    }

    ~MySharedLock() {
        mut.counter--;
    }
};
