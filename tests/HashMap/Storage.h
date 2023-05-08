#pragma once
#include <vector>
#include <string>
#include <map>

class Storage {
public:
    static constexpr int RUNS = 1;
    static constexpr int TRUNCATIONS = 0;
    static std::vector<std::string> testNames;
    static std::vector<std::string> testSuff;
    static std::vector<std::vector<std::map<size_t, std::map<size_t, double>>>> testRes;

    static void init();
};
