#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>


#include "defines.hpp"


int32_t bpm = 0;


namespace {
    bool IsValidArg(const std::string& arg, const std::vector<std::string>& valid_args) {
        for (const std::string& val : valid_args) {
            if (arg == val) {
                return true;
            }
        }

        return false;
    }
} // anonymous namespace

Errors itmoloops_funcs::CheckArgsValidity(const std::unordered_map<std::string, std::string>& args, const std::vector<std::string>& valid_args) {
    for (const auto pair : args) {
        if (!IsValidArg(pair.first, valid_args)) {
            return kSyntaxError;
        }
    }

    return kOk;
}

void itmoloops_funcs::Split(const std::string& str, std::vector<std::string>& target) {
    std::stringstream stream(str);
    std::string word;
    while (stream >> word) {
        target.push_back(word);
    }
}
