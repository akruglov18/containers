#include <gtest/gtest.h>
#include <vector>
#include <thread>
#include <tuple>
#include <tbb/tbb.h>
#include <random>
#include <numeric>

namespace {

std::vector<size_t> threads_count = {1, 4, 6, 8, 10, 12};
std::vector<size_t> elem_count = {1000, 10000, 100000, 1000000};

constexpr int RUNS = 10;
constexpr int TRUNCATIONS = 2;

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
    for (int t = 0; t < RUNS; t++) {
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
    double mean_time = std::accumulate(times.begin(), times.end() - TRUNCATIONS, 0.0) / (times.size() - TRUNCATIONS);
    std::cout << "threads_num: " << threads_num << "\n";
    std::cout << "count_per_thread: " << count_per_thread << "\n";
    std::cout << "time: " << mean_time << "\n";
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
    for (int t = 0; t < RUNS; t++) {
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
    double mean_time = std::accumulate(times.begin(), times.end() - TRUNCATIONS, 0.0) / (times.size() - TRUNCATIONS);
    std::cout << "threads_num: " << threads_num << "\n";
    std::cout << "count_per_thread: " << count_per_thread << "\n";
    std::cout << "time: " << mean_time << "\n";
}

INSTANTIATE_TEST_SUITE_P(/**/, TestTbbVectorSuite1, 
    testing::Combine(
        testing::ValuesIn(threads_count),
        testing::ValuesIn(elem_count)
    )
);

TEST(concurrent_vector, common_test) {
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
    const size_t thread_count_pb = 2;
    const size_t thread_count_read = 4;
    const size_t thread_count_write = 4;
    const size_t count_action = 2000000;
    const std::size_t thread_count = thread_count_pb + thread_count_read + thread_count_write;
    std::vector<double> times;
    for (int t = 0; t < RUNS; t++) {
        tbb::concurrent_vector<int> v;
        std::vector<std::thread> threads;
        threads.reserve(thread_count);
        std::vector<int> res(thread_count_read);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < thread_count_pb; i++) {
            threads.push_back(std::thread(func_push_back, std::ref(v), count_action));
        }
        for (int i = 0; i < thread_count_read; i++) {
            threads.push_back(std::thread(func_read, std::ref(v), std::ref(res[i]), count_action));
        }
        for (int i = 0; i < thread_count_write; i++) {
            threads.push_back(std::thread(func_write, std::ref(v), count_action));
        }
        for (int i = 0; i < threads.size(); i++) {
            threads[i].join();
        }
        auto finish = std::chrono::high_resolution_clock::now();
        times.push_back((finish - start).count() / 1e9);
    }
    sort(times.begin(), times.end());
    double mean_time = std::accumulate(times.begin(), times.end() - TRUNCATIONS, 0.0) / (times.size() - TRUNCATIONS);
    std::cout << "threads_num: " << thread_count << "\n";
    std::cout << "count_per_thread: " << count_action << "\n";
    std::cout << "time: " << mean_time << "\n";
}

}
