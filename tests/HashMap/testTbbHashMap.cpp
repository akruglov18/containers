#include <gtest/gtest.h>
#include <tbb/tbb.h>
#include <random>
#include "Storage.h"
#include <thread>
#include <numeric>
#include <chrono>

namespace {

std::vector<size_t> threads_count_suite0 = {1, 4, 6, 8, 10, 12};
std::vector<size_t> threads_count_suite3 = {3, 6, 9, 12};

std::vector<size_t> elem_count = {1000, 10000, 100000};

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestTbbHashMapSuite0;
TEST_P(TestTbbHashMapSuite0, insert) {
    auto func_insert = [](tbb::concurrent_hash_map<int, int>& m, size_t count, size_t keyOffset) {
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
        tbb::concurrent_hash_map<int, int> m;
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
    Storage::testRes[1][0][threads_num][count_per_thread] = mean_time;
}

INSTANTIATE_TEST_SUITE_P(/**/, TestTbbHashMapSuite0, 
    testing::Combine(
        testing::ValuesIn(threads_count_suite0),
        testing::ValuesIn(elem_count)
    )
);

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestTbbHashMapSuite1;
TEST_P(TestTbbHashMapSuite1, erase) {
    auto func_erase = [](tbb::concurrent_hash_map<int, int>& m, size_t count, size_t keyOffset) {
        for (int i = 0; i < count; i++) {
            m.erase(keyOffset + i);
        }
    };
    auto fillMap = [](tbb::concurrent_hash_map<int, int>& m, size_t count) {
        tbb::concurrent_hash_map<int,int>::accessor accessor;
        for (int i = 0; i < count; i++) {
            m.insert(accessor, {i, i});
        }
    };
    auto params = GetParam();
    size_t threads_num = std::get<0>(params);
    size_t count_per_thread = std::get<1>(params);
    std::vector<double> times(Storage::RUNS);
    for (int t = 0; t < Storage::RUNS; t++) {
        const size_t mapSize = count_per_thread * threads_num / 10;
        tbb::concurrent_hash_map<int, int> m;
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
    Storage::testRes[1][1][threads_num][count_per_thread] = mean_time;
}

INSTANTIATE_TEST_SUITE_P(/**/, TestTbbHashMapSuite1, 
    testing::Combine(
        testing::ValuesIn(threads_count_suite0),
        testing::ValuesIn(elem_count)
    )
);

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestTbbHashMapSuite2;
TEST_P(TestTbbHashMapSuite2, read) {
    auto func_read = [](tbb::concurrent_hash_map<int, int>& m, int& res, size_t count, size_t mapSize) {
        int sum = 0;
        std::mt19937 rnd;
        for (int i = 0; i < count; i++) {
            int ind = rnd() % mapSize;
            tbb::concurrent_hash_map<int, int>::const_accessor accessor;
            m.find(accessor, ind);
            sum += accessor->second;
        }
        res = sum;
    };
    auto fillMap = [](tbb::concurrent_hash_map<int, int>& m, size_t count) {
        tbb::concurrent_hash_map<int, int>::accessor accessor;
        for (int i = 0; i < count; i++) {
            m.insert(accessor, {i, i});
        }
    };
    auto params = GetParam();
    size_t threads_num = std::get<0>(params);
    size_t count_per_thread = std::get<1>(params);
    std::vector<double> times(Storage::RUNS);
    for (int t = 0; t < Storage::RUNS; t++) {
        const size_t mapSize = count_per_thread * threads_num / 10;
        tbb::concurrent_hash_map<int, int> m;
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
    Storage::testRes[1][2][threads_num][count_per_thread] = mean_time;
}

INSTANTIATE_TEST_SUITE_P(/**/, TestTbbHashMapSuite2, 
    testing::Combine(
        testing::ValuesIn(threads_count_suite0),
        testing::ValuesIn(elem_count)
    )
);

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestTbbHashMapSuite3;
TEST_P(TestTbbHashMapSuite3, common) {
    auto func_read = [](tbb::concurrent_hash_map<int, int>& m, int& res, size_t count, size_t mapSize) {
        int sum = 0;
        std::mt19937 rnd;
        for (int i = 0; i < count; i++) {
            int ind = rnd() % mapSize;
            tbb::concurrent_hash_map<int, int>::const_accessor accessor;
            if (m.find(accessor, ind))
            {
                sum += accessor->second;
            }
        }
        res = sum;
    };
    auto func_insert = [](tbb::concurrent_hash_map<int, int>& m, size_t count, size_t keyOffset) {
        for (int i = 0; i < count; i++) {
            m.insert({keyOffset + i, i});
        }
    };
    auto func_erase = [](tbb::concurrent_hash_map<int, int>& m, size_t count, size_t keyOffset) {
        for (int i = 0; i < count; i++) {
            m.erase(keyOffset + i);
        }
    };
    auto fillMap = [](tbb::concurrent_hash_map<int, int>& m, size_t count) {
        tbb::concurrent_hash_map<int, int>::accessor accessor;
        for (int i = 0; i < count; i++) {
            m.insert(accessor, {i, i});
        }
    };
        auto params = GetParam();
    const size_t threads_num = std::get<0>(params);
    const size_t thread_count_pb = threads_num / 3;
    const size_t thread_count_read = threads_num / 3;
    const size_t thread_count_write = threads_num / 3;
    const size_t count_per_thread = std::get<1>(params);
    std::vector<double> times(Storage::RUNS);
    for (int t = 0; t < Storage::RUNS; t++) {
        const size_t mapSize = count_per_thread * threads_num / 10;
        tbb::concurrent_hash_map<int, int> m;
        std::vector<std::thread> threads;
        threads.reserve(threads_num);
        std::vector<int> res(thread_count_read);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < thread_count_pb; i++) {
            threads.push_back(std::thread(func_insert, std::ref(m), count_per_thread, i * count_per_thread));
        }
        for (int i = 0; i < thread_count_read; i++) {
            threads.push_back(std::thread(func_read, std::ref(m), std::ref(res[i]), count_per_thread, mapSize));
        }
        for (int i = 0; i < thread_count_write; i++) {
            threads.push_back(std::thread(func_erase, std::ref(m), count_per_thread, i * count_per_thread));
        }
        for (int i = 0; i < threads.size(); i++) {
            threads[i].join();
        }
        auto finish = std::chrono::high_resolution_clock::now();
        times[t] = (finish - start).count() / 1e9;
    }
    sort(times.begin(), times.end());
    double mean_time = std::accumulate(times.begin(), times.end() - Storage::TRUNCATIONS, 0.0) / (times.size() - Storage::TRUNCATIONS);
    Storage::testRes[1][3][threads_num][count_per_thread] = mean_time;
}


INSTANTIATE_TEST_SUITE_P(/**/, TestTbbHashMapSuite3, 
    testing::Combine(
        testing::ValuesIn(threads_count_suite3),
        testing::ValuesIn(elem_count)
    )
);

}
