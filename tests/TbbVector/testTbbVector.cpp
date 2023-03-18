#include <gtest/gtest.h>
#include <vector>
#include <thread>
#include <tuple>
#include <tbb/tbb.h>

namespace {

std::vector<size_t> threads_count = {1, 4, 6, 8, 10, 12};
std::vector<size_t> elem_count = {1000, 10000, 100000, 1000000};

void pushBacks(tbb::concurrent_vector<int>& v, int s, int e) {
    for (int i = s; i < e; i++) {
        v.push_back(i);
    }
}

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestStdVector;
TEST_P(TestStdVector, push_back) {
    auto params = GetParam();
    size_t threads_num = std::get<0>(params);
    size_t count_per_thread = std::get<1>(params);
    tbb::concurrent_vector<int> v;
    std::vector<std::thread> threads(threads_num);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < threads_num; i++) {
        threads[i] = std::thread(pushBacks, std::ref(v), 0, (int)count_per_thread);
    }
    for (int i = 0; i < threads_num; i++) {
        threads[i].join();
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "threads_num: " << threads_num << "\n";
    std::cout << "count_per_thread: " << count_per_thread << "\n";
    std::cout << "time: " << (finish - start).count() / 1e9 << "\n";
}

INSTANTIATE_TEST_SUITE_P(/**/, TestStdVector, 
    testing::Combine(
        testing::ValuesIn(threads_count),
        testing::ValuesIn(elem_count)
    )
);


}
