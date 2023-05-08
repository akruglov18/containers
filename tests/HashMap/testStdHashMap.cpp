#include <gtest/gtest.h>
#include <random>
#include "Storage.h"
#include <thread>
#include <numeric>
#include <chrono>
#include <unordered_map>

namespace {

std::vector<size_t> threads_count_suite0 = {1};
std::vector<size_t> threads_count_suite3 = {3, 6, 9, 12};

std::vector<size_t> elem_count = {1000, 10000, 100000};

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestHashStdMapSuite0;
TEST_P(TestHashStdMapSuite0, insert) {
    auto func_insert = [](std::unordered_map<int, int>& m, size_t count, size_t keyOffset) {
        for (int i = 0; i < count; i++) {
            m.insert({keyOffset + i, i});
        }
    };
    auto params = GetParam();
    size_t threads_num = std::get<0>(params);
    size_t count_per_thread = std::get<1>(params);
    std::vector<double> times(Storage::RUNS);
    for (int t = 0; t < Storage::RUNS; t++) {
        const size_t mapSize = count_per_thread * threads_num / 10;
        std::unordered_map<int, int> m(mapSize);
        std::vector<std::thread> threads(threads_num);
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < threads_num; i++) {
            threads[i] = std::thread(func_insert, std::ref(m), count_per_thread, count_per_thread * i);
        }
        for (int i = 0; i < threads_num; i++) {
            threads[i].join();
        }
        auto finish = std::chrono::high_resolution_clock::now();
        times[t] = (finish - start).count() / 1e9;
    }
    sort(times.begin(), times.end());
    double mean_time = std::accumulate(times.begin(), times.end() - Storage::TRUNCATIONS, 0.0) / (times.size() - Storage::TRUNCATIONS);
    Storage::testRes[2][0][threads_num][count_per_thread] = mean_time;
}

INSTANTIATE_TEST_SUITE_P(/**/, TestHashStdMapSuite0, 
    testing::Combine(
        testing::ValuesIn(threads_count_suite0),
        testing::ValuesIn(elem_count)
    )
);

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestHashStdMapSuite1;
TEST_P(TestHashStdMapSuite1, erase) {
    auto func_erase = [](std::unordered_map<int, int>& m, size_t count, size_t keyOffset) {
        for (int i = 0; i < count; i++) {
            m.erase(keyOffset + i);
        }
    };
    auto fillMap = [](std::unordered_map<int, int>& m, size_t count) {
        for (int i = 0; i < count; i++) {
            m.insert({i, i});
        }
    };
    auto params = GetParam();
    size_t threads_num = std::get<0>(params);
    size_t count_per_thread = std::get<1>(params);
    std::vector<double> times(Storage::RUNS);
    for (int t = 0; t < Storage::RUNS; t++) {
        const size_t mapSize = count_per_thread * threads_num / 10;
        std::unordered_map<int, int> m(mapSize);
        fillMap(m, count_per_thread * threads_num);
        std::vector<std::thread> threads(threads_num);
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < threads_num; i++) {
            threads[i] = std::thread(func_erase, std::ref(m), count_per_thread, count_per_thread * i);
        }
        for (int i = 0; i < threads_num; i++) {
            threads[i].join();
        }
        auto finish = std::chrono::high_resolution_clock::now();
        times[t] = (finish - start).count() / 1e9;
    }
    sort(times.begin(), times.end());
    double mean_time = std::accumulate(times.begin(), times.end() - Storage::TRUNCATIONS, 0.0) / (times.size() - Storage::TRUNCATIONS);
    Storage::testRes[2][1][threads_num][count_per_thread] = mean_time;
}

INSTANTIATE_TEST_SUITE_P(/**/, TestHashStdMapSuite1, 
    testing::Combine(
        testing::ValuesIn(threads_count_suite0),
        testing::ValuesIn(elem_count)
    )
);

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestHashStdMapSuite2;
TEST_P(TestHashStdMapSuite2, read) {
    auto func_read = [](std::unordered_map<int, int>& m, int& res, size_t count, size_t mapSize) {
        int sum = 0;
        std::mt19937 rnd;
        for (int i = 0; i < count; i++) {
            int ind = rnd() % mapSize;
            auto it = m.find(ind);
            if (it != m.end())
                sum += it->second;
        }
        res = sum;
    };
    auto fillMap = [](std::unordered_map<int, int>& m, size_t count) {
        for (int i = 0; i < count; i++) {
            m.insert({i, i});
        }
    };
    auto params = GetParam();
    size_t threads_num = std::get<0>(params);
    size_t count_per_thread = std::get<1>(params);
    std::vector<double> times(Storage::RUNS);
    for (int t = 0; t < Storage::RUNS; t++) {
        const size_t mapSize = count_per_thread * threads_num / 10;
        std::unordered_map<int, int> m(mapSize);
        fillMap(m, count_per_thread * threads_num);
        std::vector<std::thread> threads(threads_num);
        std::vector<int> res(threads_num);
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < threads_num; i++) {
            threads[i] = std::thread(func_read, std::ref(m), std::ref(res[i]), count_per_thread, mapSize);
        }
        for (int i = 0; i < threads_num; i++) {
            threads[i].join();
        }
        auto finish = std::chrono::high_resolution_clock::now();
        times[t] = (finish - start).count() / 1e9;
    }
    sort(times.begin(), times.end());
    double mean_time = std::accumulate(times.begin(), times.end() - Storage::TRUNCATIONS, 0.0) / (times.size() - Storage::TRUNCATIONS);
    Storage::testRes[2][2][threads_num][count_per_thread] = mean_time;
}

INSTANTIATE_TEST_SUITE_P(/**/, TestHashStdMapSuite2, 
    testing::Combine(
        testing::ValuesIn(threads_count_suite0),
        testing::ValuesIn(elem_count)
    )
);

}
