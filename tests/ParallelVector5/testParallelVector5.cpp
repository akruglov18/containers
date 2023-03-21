#include "Parallelvector5.h"
#include <gtest/gtest.h>
#include <vector>
#include <thread>
#include <tuple>
#include <random>

namespace {

std::vector<size_t> threads_count_suite0 = {1, 4, 6, 8, 10, 12};
std::vector<size_t> threads_count_suite1 = {1, 4, 6, 8, 10, 12};
std::vector<size_t> elem_count = {1000, 10000, 100000, 1000000};

void pushBacks(ParallelVector5<int, 1000>& v, int s, int e) {
    for (int i = s; i < e; i++) {
        v.push_back(i);
    }
}

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestParallelVector5Suite0;
TEST_P(TestParallelVector5Suite0, push_back) {
    auto params = GetParam();
    size_t threads_num = std::get<0>(params);
    size_t count_per_thread = std::get<1>(params);
    ParallelVector5<int, 1000> v;
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

INSTANTIATE_TEST_SUITE_P(/**/, TestParallelVector5Suite0, 
    testing::Combine(
        testing::ValuesIn(threads_count_suite0),
        testing::ValuesIn(elem_count)
    )
);

typedef testing::TestWithParam<std::tuple<size_t, size_t>> TestParallelVector5Suite1;
TEST_P(TestParallelVector5Suite1, read) {
    auto params = GetParam();
    size_t threads_num = std::get<0>(params);
    size_t count_per_thread = std::get<1>(params);
    ParallelVector5<int, 1000> v;
    std::vector<int> res(threads_num);
    auto func = [](ParallelVector5<int, 1000>& v, std::vector<int>& res, int num, size_t count) {
        int sum = 0;
        for (int i = 0; i < count; i++) {
            if (i < v.get_size())
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
    std::cout << "threads_num: " << threads_num << "\n";
    std::cout << "count_per_thread: " << count_per_thread << "\n";
    std::cout << "time: " << (finish - start).count() / 1e9 << "\n";
}

INSTANTIATE_TEST_SUITE_P(/**/, TestParallelVector5Suite1, 
    testing::Combine(
        testing::ValuesIn(threads_count_suite1),
        testing::ValuesIn(elem_count)
    )
);

TEST(ParallelVector5, common_test) {
    //const size_t thread_count_pb = 2;
    //const size_t thread_count_read = 4;
    //const size_t thread_count_write = 4;
    //const size_t count_action = 2000000;
    //auto func_push_back = [](ParallelVector5<int, 1000>& v, size_t count) {
    //    for (int i = 0; i < count; i++) {
    //        v.push_back(i);
    //    }
    //};
    //auto func_read = [](ParallelVector5<int, 1000>& v, int& res, size_t count) {
    //    int sum = 0;
    //    std::mt19937 rnd;
    //    for (int i = 0; i < count; i++) {
    //        if (v.get_size() > 0) {
    //            int ind = rnd() % v.get_size();
    //            sum += v[ind];
    //        }
    //    }
    //    res = sum;
    //};
    //auto func_write = [](ParallelVector5<int, 1000>& v, size_t count) {
    //    std::mt19937 rnd;
    //    for (int i = 0; i < count; i++) {
    //        if (v.get_size() > 0) {
    //            int ind = rnd() % v.get_size();
    //            v[ind] = rnd();
    //        }
    //    }
    //};

    //ParallelVector5<int, 1000> v;
    //std::vector<std::thread> threads;
    //std::size_t thread_count = thread_count_pb + thread_count_read + thread_count_write;
    //threads.reserve(thread_count);
    //std::vector<int> res(thread_count_read);

    //auto start = std::chrono::high_resolution_clock::now();
    //for (int i = 0; i < thread_count_pb; i++) {
    //    threads.push_back(std::thread(func_push_back, std::ref(v), count_action));
    //}
    //// std::cout << "thread_pb\n";
    //for (int i = 0; i < thread_count_read; i++) {
    //    threads.push_back(std::thread(func_read, std::ref(v), std::ref(res[i]), count_action));
    //}
    //// std::cout << "thread_read\n";
    //for (int i = 0; i < thread_count_write; i++) {
    //    threads.push_back(std::thread(func_write, std::ref(v), count_action));
    //}
    //// std::cout << "thread_write\n";
    //for (int i = 0; i < threads.size(); i++) {
    //    threads[i].join();
    //}
    //auto finish = std::chrono::high_resolution_clock::now();
    //std::cout << "time: " << (finish - start).count() / 1e9 << "\n";
}

}
