#define _USE_MATH_DEFINES


#include <cmath>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>


#include "defines.hpp"
#include "tremolo.hpp"


Errors effects::Tremolo::Build(const std::unordered_map<std::string, std::string>& args) {
    if (!args.contains("freq")) {
        return kSyntaxError;
    }
    freq_ = std::stod(args.at("freq"));

    if (!args.contains("depth")) {
        return kSyntaxError;
    }
    depth_ = std::stod(args.at("depth"));

    return itmoloops_funcs::CheckArgsValidity(args, kValidArgs);
}

void effects::Tremolo::operator()(std::vector<double>& thread) const {
    if (thread.empty()) {
        return;
    }

    for (size_t i = 0; i < thread.size(); i++) {
        thread[i] *= (1 - depth_ + depth_ * std::sin(2 * M_PI * freq_ * ((double)i / kFreq)));
    }
}

