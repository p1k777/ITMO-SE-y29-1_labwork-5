#include <string>
#include <unordered_map>
#include <vector>


#include "gain.hpp"
#include "defines.hpp"


Errors effects::Gain::Build(const std::unordered_map<std::string, std::string>& args) {
    if (!args.contains("gain")) {
        return kSyntaxError;
    }

    gain_ = std::stod(args.at("gain"));

    return itmoloops_funcs::CheckArgsValidity(args, kValidArgs);
}


void effects::Gain::operator()(std::vector<double>& thread) const {
    for (double& sample : thread) {
        sample *= gain_;
    }
}
