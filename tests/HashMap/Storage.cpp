#include "Storage.h"

std::vector<std::string> Storage::testNames = { "HashMap", "TbbHashMap", "StdHashMap" };
std::vector<std::string> Storage::testSuff = { "Insert", "Erase", "Read", "Common"};

std::vector<std::vector<std::map<size_t, std::map<size_t, double>>>> Storage::testRes;

void Storage::init()
{
    testRes.resize(testNames.size());
    for (auto& v : testRes) {
        v.resize(testSuff.size());
    }
}
