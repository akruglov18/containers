#include "Storage.h"

std::vector<std::string> Storage::testNames = { "ParallelVector", "ParallelVector2", "ParallelVector3", 
                                                "ParallelVector4", "ParallelVector5", "ParallelVector6",
                                                "ParallelVector7", "TbbVector" };
std::vector<std::string> Storage::testSuff = { "PushBack", "Read", "Common"};

std::vector<std::vector<std::map<size_t, std::map<size_t, double>>>> Storage::testRes;

void Storage::init()
{
    testRes.resize(testNames.size());
    for (auto& v : testRes) {
        v.resize(testSuff.size());
    }
}
