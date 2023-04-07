#include <gtest/gtest.h>
#include <vector>
#include <thread>
#include <tuple>
#include <random>
#include <numeric>

namespace {

std::vector<size_t> threads_count = {1};
std::vector<size_t> elem_count = {1000, 10000, 100000, 1000000};

constexpr int RUNS = 10;
constexpr int TRUNCATIONS = 2;

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestStdVectorSuite0;
TEST_P(TestStdVectorSuite0, push_back) {
    auto params = GetParam();
    const size_t threads_num = std::get<0>(params);
    const size_t count_per_thread = std::get<1>(params);
    std::vector<double> times;
    auto func_push_back = [](std::vector<int>& v, size_t count) {
        for (int i = 0; i < count; i++) {
            v.push_back(i);
        }
    };
    for (int t = 0; t < RUNS; t++) {
        std::vector<int> v;
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
    double mean_time = std::accumulate(times.begin(), times.end() - TRUNCATIONS, 0.0) / (times.size() - TRUNCATIONS);
    std::cout << "threads_num: " << threads_num << "\n";
    std::cout << "count_per_thread: " << count_per_thread << "\n";
    std::cout << "time: " << mean_time << "\n";
}

INSTANTIATE_TEST_SUITE_P(/**/, TestStdVectorSuite0, 
    testing::Combine(
        testing::ValuesIn(threads_count),
        testing::ValuesIn(elem_count)
    )
);

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestStdVectorSuite1;
TEST_P(TestStdVectorSuite1, read) {
    auto params = GetParam();
    const size_t threads_num = std::get<0>(params);
    const size_t count_per_thread = std::get<1>(params);
    std::vector<double> times;
    for (int t = 0; t < RUNS; t++) {
        std::vector<int> v;
        std::vector<int> res(threads_num);
        auto func = [](std::vector<int>& v, std::vector<int>& res, int num, size_t count) {
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
    double mean_time = std::accumulate(times.begin(), times.end() - TRUNCATIONS, 0.0) / (times.size() - TRUNCATIONS);
    std::cout << "threads_num: " << threads_num << "\n";
    std::cout << "count_per_thread: " << count_per_thread << "\n";
    std::cout << "time: " << mean_time << "\n";
}

INSTANTIATE_TEST_SUITE_P(/**/, TestStdVectorSuite1, 
    testing::Combine(
        testing::ValuesIn(threads_count),
        testing::ValuesIn(elem_count)
    )
);


}
