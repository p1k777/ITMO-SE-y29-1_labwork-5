#define _USE_MATH_DEFINES


#include <cmath>
#include <string>
#include <unordered_map>


#include "defines.hpp"
#include "instrument.hpp"
#include "oscillator.hpp"
#include "sine.hpp"


instruments::Sine::Sine(const std::string& name)
    : Oscillator(name)
{}

Errors instruments::Sine::Build(const std::unordered_map<std::string, std::string>& args) {
    if (args.contains("attack")) {
        attack_ = std::stod(args.at("attack"));
    }

    if (args.contains("release")) {
        release_ = std::stod(args.at("release"));
    }

    return itmoloops_funcs::CheckArgsValidity(args, kValidArgs);
}

double instruments::Sine::CalcCoef(double t, double freq) const {
    return std::sin(2.0 * M_PI * freq * t);
}

void instruments::Sine::ConvertToBytes(const Note& note) {
    Oscillator::ConvertToBytes(note);
}
