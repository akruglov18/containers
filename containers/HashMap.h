#pragma once
#include <vector>
#include <list>
#include <mutex>
#include "MapAccessor.h"

template<typename KeyType, typename ValueType>
class HashMap
{
private:
    std::vector<std::list<std::pair<size_t, std::pair<KeyType, ValueType>>>> data;
    std::vector<MySharedMutex> muts;
    std::atomic_size_t count;
    MySharedMutex mutex;

    size_t Hash(KeyType key)
    {
        return std::hash<KeyType>{}(key);
    }

    bool isRehashNeeded()
    {
        return count > data.size() * 10;
    }

public:

    HashMap(size_t size = 1) : data(size), muts(size) {

    }

    void insert(KeyType key, ValueType value)
    {
        UniqueAccessor accessor;
        accessor.initCommon(mutex);
        size_t hash = Hash(key);
        size_t index = hash % data.size();
        accessor.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->second.first == key)
            {
                return;
            }
        }
        count++;
        curList.push_back(std::make_pair(hash, std::make_pair(key, value)));
        if (isRehashNeeded())
        {
            rehash(accessor);
        }
    }

    void erase(KeyType key)
    {
        UniqueAccessor accessor;
        accessor.initCommon(mutex);
        size_t index = Hash(key) % data.size();
        accessor.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->second.first == key)
            {
                count--;
                curList.erase(it);
                return;
            }
        }
    }

    void erase(UniqueAccessor &accessor, KeyType key)
    {
        accessor.initCommon(mutex);
        size_t index = Hash(key) % data.size();
        accessor.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->second.first == key)
            {
                count--;
                curList.erase(it);
                return;
            }
        }
    }

    std::pair<ValueType, bool> read(KeyType key)
    {
        SharedAccessor accessor;
        accessor.initCommon(mutex);
        size_t index = Hash(key) % data.size();
        accessor.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->second.first == key)
            {
                return std::make_pair(it->second.second, true);
            }
        }
        return std::make_pair(T{}, false);
    }

    void write(KeyType key, ValueType value)
    {
        SharedAccessor accessor;
        accessor.initCommon(mutex);
        size_t index = Hash(key) % data.size();
        accessor.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->second.first == key)
            {
                it->second.second = value;
                return;
            }
        }
    }

    std::pair<KeyType, ValueType> *find(UniqueAccessor &accessor, KeyType key)
    {
        accessor.initCommon(mutex);
        size_t index = Hash(key) % data.size();
        accessor.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->second.first == key)
            {
                return &(*it->second);
            }
        }
        return nullptr;
    }

    std::pair<KeyType, ValueType> *find(SharedAccessor &accessor, KeyType key)
    {
        accessor.initCommon(mutex);
        size_t index = Hash(key) % data.size();
        accessor.init(muts[index]);
        auto& curList = data[index];
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->second.first == key)
            {
                return &(it->second);
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
    void rehash(UniqueAccessor &accessor)
    {
        accessor.release();
        mutex.UniqueLock();

        if (isRehashNeeded()) {
            std::vector<std::list<std::pair<size_t, std::pair<KeyType, ValueType>>>> newData(data.size() * 2 + 1);
            std::vector<MySharedMutex> newMuts(muts.size() * 2 + 1);
            for (int i = 0; i < data.size(); i++)
            {
                for (auto elem : data[i])
                {
                    size_t newInd = elem.first % newData.size();
                    newData[newInd].push_back(elem); 
                }
            }

            data = std::move(newData);
            muts = std::move(newMuts);
        }
        mutex.UniqueRelease();
    }

};