#pragma once
#include <vector>
#include <list>
#include <mutex>

template<typename KeyType, typename ValueType>
class HashMap
{
private:
    std::vector<std::list<std::pair<KeyType, ValueType>>> data;
    std::vector<std::mutex> muts;

    size_t Hash(ValueType val)  {
        return abs(val);
    }
public:

    HashMap(size_t size) : data(size), muts(size) {

    }

    void insert(KeyType key, ValueType value)
    {
        size_t index = Hash(key) % data.size();
        auto& curList = data[index];
        std::lock_guard<std::mutex> lock(muts[index]);
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->first == key)
            {
                return;
            }
        }
        curList.push_back(std::make_pair(key, value));

    }

    void erase(KeyType key)
    {
        size_t index = Hash(key) % data.size();
        auto& curList = data[index];
        std::lock_guard<std::mutex> lock(muts[index]);
        for (auto it = curList.begin(); it != curList.end(); it++)
        {
            if (it->first == key)
            {
                curList.erase(it);
                return;
            }
        }
    }

    ValueType read(KeyType key)
    {
        size_t index = Hash(key) % data.size();
        auto& curList = data[index];
        std::lock_guard<std::mutex> lock(muts[index]);
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

};