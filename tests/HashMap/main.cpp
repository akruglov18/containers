#include <gtest/gtest.h>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include "Storage.h"

int main(int argc, char** argv)
{
    Storage::init();


    ::testing::InitGoogleTest(&argc, argv);
    int res = RUN_ALL_TESTS();
    if (res != 0) {
        return res;
    }
    std::vector<size_t> elems_value = {1000, 10000, 100000, 1000000};

    auto print_table = [&elems_value](const std::map<size_t, std::map<size_t, double>> &table,
                                      std::ofstream &out) {
        out << "   ";
        for (auto x : elems_value)
        {
            out << std::setw(8) << x << " ";
        }
        out << "\n";
        for (auto& row : table)
        {
            out << std::setw(2) << row.first << " ";
            for (auto& value : row.second) {
                out << std::fixed << std::setprecision(5) << std::setw(8) << value.second << " ";
            }
            out << "\n";
        }
    };

    auto print_table_ratio = [&elems_value](const std::map<size_t, std::map<size_t, double>> &table1,
                                            const std::map<size_t, std::map<size_t, double>> &table2,
                                            std::ofstream &out) {
        out << "   ";
        for (auto x : elems_value)
        {
            out << std::setw(8) << x << " ";
        }
        out << "\n";
        for (auto& row : table1)
        {
            out << std::setw(2) << row.first << " ";
            for (auto& value : row.second) {
                out << std::fixed << std::setprecision(5) << std::setw(8) << value.second / table2.at(row.first).at(value.first) << " ";
            }
            out << "\n";
        }
    };
    auto print_table_ratio_reverse = [&elems_value](const std::map<size_t, std::map<size_t, double>> &table1,
                                            const std::map<size_t, std::map<size_t, double>> &table2,
                                            std::ofstream &out) {
        out << "   ";
        for (auto x : elems_value)
        {
            out << std::setw(8) << x << " ";
        }
        out << "\n";
        for (auto& row : table1)
        {
            out << std::setw(2) << row.first << " ";
            for (auto& value : row.second) {
                out << std::fixed << std::setprecision(5) << std::setw(8) << table2.at(row.first).at(value.first) / value.second  << " ";
            }
            out << "\n";
        }
    };

    std::string folder = "res";
    std::filesystem::create_directory(folder);
    folder += '/';
    for (int i = 0; i < Storage::testRes.size(); i++) {
        for (int j = 0; j < Storage::testSuff.size(); j++) {
            std::string fileName = folder + Storage::testNames[i] + Storage::testSuff[j] + ".txt";
            std::cout << fileName << std::endl;
            std::ofstream out(fileName);
            auto& curRes = Storage::testRes[i][j];
            print_table(curRes, out);
        }
    }

    folder = "compare";
    std::filesystem::create_directory(folder);
    folder += '/';
    for (int i = 0; i < Storage::testRes.size() - 1; i++) {
        for (int k = i + 1; k < Storage::testRes.size() - 1; k++) {
            for (int j = 0; j < Storage::testSuff.size(); j++) {
                std::string fileName = folder + Storage::testNames[i] + "_" + 
                                       Storage::testNames[k] + Storage::testSuff[j] + ".txt";
                std::cout << fileName << std::endl;
                std::ofstream out(fileName);
                auto& curRes = Storage::testRes[i][j];
                auto& otherRes = Storage::testRes[k][j];
                print_table_ratio(curRes, otherRes, out);
                out << "\n";
                print_table_ratio(otherRes, curRes, out);
            }
        }
    }
    for (int i = 0; i < Storage::testRes.size() - 1; i++) {
        for (int j = 0; j < Storage::testSuff.size() - 1; j++) {
            std::string fileName = folder + Storage::testNames[i] + "_" + 
                                    Storage::testNames[2] + Storage::testSuff[j] + ".txt";
            std::cout << fileName << std::endl;
            std::ofstream out(fileName);
            auto& curRes = Storage::testRes[i][j];
            auto& stdRes = Storage::testRes[2][j];
            print_table_ratio_reverse(stdRes, curRes, out);
        }
    }
    return 0;
}
