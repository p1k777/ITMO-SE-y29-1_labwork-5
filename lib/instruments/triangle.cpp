#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>


#include "defines.hpp"
#include "instrument.hpp"
#include "oscillator.hpp"
#include "triangle.hpp"


instruments::Triangle::Triangle(const std::string& name)
    : Oscillator(name)
{}

Errors instruments::Triangle::Build(const std::unordered_map<std::string, std::string>& args) {
    if (args.contains("attack")) {
        attack_ = std::stod(args.at("attack"));
    }

    if (args.contains("release")) {
        release_ = std::stod(args.at("release"));
    }

    return itmoloops_funcs::CheckArgsValidity(args, kValidArgs);
}

double instruments::Triangle::CalcCoef(double t, double freq) const {
    return 1 - 4 * std::abs(t * freq - (int64_t)(t * freq) - 0.5);
}

void instruments::Triangle::ConvertToBytes(const Note& note) {
    Oscillator::ConvertToBytes(note);
}
