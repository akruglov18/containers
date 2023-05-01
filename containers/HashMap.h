#pragma once
#include <vector>
#include <list>
#include <mutex>
#include "Accessor.h"

class UniqueAccessor
{
private:
    MySharedMutex *mut;
    bool wasInited = false;

public:
    UniqueAccessor() {}

    void init(MySharedMutex &lockMut)
    {
        if (!wasInited)
        {
            mut = &lockMut;
            lockMut.mut.lock();
            lockMut.flag = true;
            while (lockMut.counter != 0) {}
            wasInited = true;
        }
    }
    
    ~UniqueAccessor() {
        if (wasInited)
        {
            mut->flag = false;
            mut->mut.unlock();
        }
    }
};

class SharedAccessor
{
private:
    MySharedMutex *mut;
    bool wasInited = false;

public:
    SharedAccessor() {}

    void init(MySharedMutex &lockMut)
    {
        if (wasInited) return;
        
        while (true)
        {
            while (lockMut.flag) {}
            lockMut.counter++;
            if (lockMut.flag)
            {
                lockMut.counter--;
            }
            else
            {
                break;
            }
        }
        mut = &lockMut;
        wasInited = true;
    }

    ~SharedAccessor() {
        if (wasInited)
        {
            mut->counter--;
        }
    }   
};

template<typename KeyType, typename ValueType>
class HashMap
{
private:
    std::vector<std::list<std::pair<KeyType, ValueType>>> data;
    std::vector<MySharedMutex> muts;
    std::atomic_size_t count;

    size_t Hash(ValueType val)  {
        return abs(val);
    }

    friend class UniqueAccessor;
    friend class SharedAccessor;
public:

    HashMap(size_t size) : data(size), muts(size) {

    }

    void insert(KeyType key, ValueType value)
    {
        size_t index = Hash(key) % data.size();
        UniqueAccessor lock;
        lock.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->first == key)
            {
                return;
            }
        }
        count++;
        curList.push_back(std::make_pair(key, value));
    }

    void insert(UniqueAccessor &lock, KeyType key, ValueType value)
    {
        size_t index = Hash(key) % data.size();
        lock.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->first == key)
            {
                return;
            }
        }
        count++;
        curList.push_back(std::make_pair(key, value));
    }

    void erase(KeyType key)
    {
        size_t index = Hash(key) % data.size();
        UniqueAccessor lock;
        lock.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->first == key)
            {
                count--;
                curList.erase(it);
                return;
            }
        }
    }

    void erase(UniqueAccessor &lock, KeyType key)
    {
        size_t index = Hash(key) % data.size();
        lock.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->first == key)
            {
                count--;
                curList.erase(it);
                return;
            }
        }
    }

    ValueType read(KeyType key)
    {
        size_t index = Hash(key) % data.size();
        SharedAccessor lock;
        lock.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->first == key)
            {
                return it->second;
            }
        }
        curList.push_back(std::make_pair(key, ValueType{}));
        return curList.back().second;
    }

    std::pair<KeyType, ValueType> *find(UniqueAccessor &accessor, KeyType key)
    {
        size_t index = Hash(key) % data.size();
        accessor.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->first == key)
            {
                return &(*it);
            }
        }
        return nullptr;
    }

    std::pair<KeyType, ValueType> *find(SharedAccessor &accessor, KeyType key)
    {
        size_t index = Hash(key) % data.size();
        accessor.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->first == key)
            {
                return &(*it);
            }
        }
        return nullptr;
    }

    size_t getSize() const {
        return count;
    }

    void clear() {
        for (size_t i = 0; i < data.size(); i++) {
            data[i].clear();
        }
        count = 0;
    }

};