#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>


#include "defines.hpp"
#include "echo.hpp"


Errors effects::Echo::Build(const std::unordered_map<std::string, std::string>& args) {
    if (!args.contains("delay")) {
        return kSyntaxError;
    }
    delay_ = std::stod(args.at("delay"));

    if (!args.contains("decay")) {
        return kSyntaxError;
    }
    decay_ = std::stod(args.at("decay"));

    return itmoloops_funcs::CheckArgsValidity(args, kValidArgs);
}

void effects::Echo::operator()(std::vector<double>& thread) const {
    size_t delay_in_samples = delay_ * kFreq;
    if (delay_in_samples == 0 || thread.empty()) {
        return;
    }
    for (size_t i = thread.size(); i-- > delay_in_samples;) {
        thread[i] += decay_ * thread[i - delay_in_samples];
    }
}
