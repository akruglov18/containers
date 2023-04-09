#include <gtest/gtest.h>
#include <vector>
#include <thread>
#include <tuple>
#include <tbb/tbb.h>
#include <random>
#include <numeric>
#include "Storage.h"

namespace {

std::vector<size_t> threads_count = {1, 4, 6, 8, 10, 12};
std::vector<size_t> threads_count_suite2 = {3, 6, 9, 12};
std::vector<size_t> elem_count = {1000, 10000, 100000, 1000000};

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestTbbVectorSuite0;
TEST_P(TestTbbVectorSuite0, push_back) {
    auto params = GetParam();
    const size_t threads_num = std::get<0>(params);
    const size_t count_per_thread = std::get<1>(params);
    std::vector<double> times;
    auto func_push_back = [](tbb::concurrent_vector<int>& v, size_t count) {
        for (int i = 0; i < count; i++) {
            v.push_back(i);
        }
    };
    for (int t = 0; t < Storage::RUNS; t++) {
        tbb::concurrent_vector<int> v;
        std::vector<std::thread> threads(threads_num);
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < threads_num; i++) {
            threads[i] = std::thread(func_push_back, std::ref(v), count_per_thread);
        }
        for (int i = 0; i < threads_num; i++) {
            threads[i].join();
        }
        auto finish = std::chrono::high_resolution_clock::now();
        times.push_back((finish - start).count() / 1e9);
    }
    sort(times.begin(), times.end());
    double mean_time = std::accumulate(times.begin(), times.end() - Storage::TRUNCATIONS, 0.0) / (times.size() - Storage::TRUNCATIONS);
    Storage::testRes[5][0][threads_num][count_per_thread] = mean_time;
}

INSTANTIATE_TEST_SUITE_P(/**/, TestTbbVectorSuite0, 
    testing::Combine(
        testing::ValuesIn(threads_count),
        testing::ValuesIn(elem_count)
    )
);

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestTbbVectorSuite1;
TEST_P(TestTbbVectorSuite1, read) {
    auto params = GetParam();
    const size_t threads_num = std::get<0>(params);
    const size_t count_per_thread = std::get<1>(params);
    std::vector<double> times;
    for (int t = 0; t < Storage::RUNS; t++) {
        tbb::concurrent_vector<int> v;
        std::vector<int> res(threads_num);
        auto func = [](tbb::concurrent_vector<int>& v, std::vector<int>& res, int num, size_t count) {
            int sum = 0;
            for (int i = 0; i < count; i++) {
                if (i < v.size())
                {
                    sum += v[i];
                }
            }
            res[num] = sum;
        };
        std::vector<std::thread> threads(threads_num);
        for (int i = 0; i < count_per_thread; i++) {
            v.push_back(i);
        }
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < threads_num; i++) {
            threads[i] = std::thread(func, std::ref(v), std::ref(res), i, count_per_thread);
        }
        for (int i = 0; i < threads_num; i++) {
            threads[i].join();
        }
        auto finish = std::chrono::high_resolution_clock::now();
        times.push_back((finish - start).count() / 1e9);
    }
    sort(times.begin(), times.end());
    double mean_time = std::accumulate(times.begin(), times.end() - Storage::TRUNCATIONS, 0.0) / (times.size() - Storage::TRUNCATIONS);
    Storage::testRes[5][1][threads_num][count_per_thread] = mean_time;
}

INSTANTIATE_TEST_SUITE_P(/**/, TestTbbVectorSuite1, 
    testing::Combine(
        testing::ValuesIn(threads_count),
        testing::ValuesIn(elem_count)
    )
);

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestTbbVectorSuite2;
TEST_P(TestTbbVectorSuite2, common_test) {
    auto func_push_back = [](tbb::concurrent_vector<int>& v, size_t count) {
        for (int i = 0; i < count; i++) {
            v.push_back(i);
        }
    };
    auto func_read = [](tbb::concurrent_vector<int>& v, int& res, size_t count) {
        int sum = 0;
        std::mt19937 rnd;
        for (int i = 0; i < count; i++) {
            if (v.size() > 0) {
                int ind = rnd() % v.size();
                sum += v[ind];
            }
        }
        res = sum;
    };
    auto func_write = [](tbb::concurrent_vector<int>& v, size_t count) {
        std::mt19937 rnd;
        for (int i = 0; i < count; i++) {
            if (v.size() > 0) {
                int ind = rnd() % v.size();
                v[ind] = i;
            }
        }
    };
    auto params = GetParam();
    const size_t threads_num = std::get<0>(params);
    const size_t thread_count_pb = threads_num / 3;
    const size_t thread_count_read = threads_num / 3;
    const size_t thread_count_write = threads_num / 3;
    const size_t count_per_thread = std::get<1>(params);
    std::vector<double> times;
    for (int t = 0; t < Storage::RUNS; t++) {
        tbb::concurrent_vector<int> v;
        std::vector<std::thread> threads;
        threads.reserve(threads_num);
        std::vector<int> res(thread_count_read);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < thread_count_pb; i++) {
            threads.push_back(std::thread(func_push_back, std::ref(v), count_per_thread));
        }
        for (int i = 0; i < thread_count_read; i++) {
            threads.push_back(std::thread(func_read, std::ref(v), std::ref(res[i]), count_per_thread));
        }
        for (int i = 0; i < thread_count_write; i++) {
            threads.push_back(std::thread(func_write, std::ref(v), count_per_thread));
        }
        for (int i = 0; i < threads.size(); i++) {
            threads[i].join();
        }
        auto finish = std::chrono::high_resolution_clock::now();
        times.push_back((finish - start).count() / 1e9);
    }
    sort(times.begin(), times.end());
    double mean_time = std::accumulate(times.begin(), times.end() - Storage::TRUNCATIONS, 0.0) / (times.size() - Storage::TRUNCATIONS);
    Storage::testRes[5][2][threads_num][count_per_thread] = mean_time;
}

INSTANTIATE_TEST_SUITE_P(/**/, TestTbbVectorSuite2, 
    testing::Combine(
        testing::ValuesIn(threads_count_suite2),
        testing::ValuesIn(elem_count)
    )
);

}
