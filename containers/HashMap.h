#pragma once
#include <vector>
#include <list>
#include <mutex>
#include "MapAccessor.h"

template<typename KeyType, typename ValueType>
class HashMap
{
private:
    std::vector<std::list<std::pair<KeyType, ValueType>>> data;
    std::vector<MySharedMutex> muts;
    std::atomic_size_t count;
    MySharedMutex mutex;

    size_t Hash(KeyType key)
    {
        return std::hash<KeyType>{}(key);
    }

public:

    HashMap(size_t size) : data(size), muts(size) {

    }

    void insert(KeyType key, ValueType value)
    {
        size_t index = Hash(key) % data.size();
        UniqueAccessor lock;
        lock.initCommon(mutex);
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
        lock.initCommon(mutex);
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
        lock.initCommon(mutex);
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

    std::pair<ValueType, bool> read(KeyType key)
    {
        size_t index = Hash(key) % data.size();
        SharedAccessor lock;
        lock.initCommon(mutex);
        lock.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->first == key)
            {
                return std::make_pair(it->second, true);
            }
        }
        return std::make_pair(T{}, false);
    }

    void write(KeyType key, ValueType value)
    {
        size_t index = Hash(key) % data.size();
        SharedAccessor lock;
        lock.initCommon(mutex);
        lock.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->first == key)
            {
                it->second = value;
                return;
            }
        }
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

private:
    void rehash()
    {
        mutex.UniqueLock();
    }

};