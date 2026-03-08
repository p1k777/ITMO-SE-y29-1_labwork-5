#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>


#include "defines.hpp"
#include "instrument.hpp"
#include "oscillator.hpp"
#include "square.hpp"


instruments::Square::Square(const std::string& name)
    : Oscillator(name)
{}

Errors instruments::Square::Build(const std::unordered_map<std::string, std::string>& args) {
    if (!args.contains("duty")) {
        return kSyntaxError;
    }
    duty_ = (uint8_t)std::stoi(args.at("duty"));

    if (args.contains("attack")) {
        attack_ = std::stod(args.at("attack"));
    }

    if (args.contains("release")) {
        release_ = std::stod(args.at("release"));
    }

    return itmoloops_funcs::CheckArgsValidity(args, kValidArgs);
}

double instruments::Square::CalcCoef(double t, double freq) const {
    double phase = t * freq;
    double frac = phase - std::floor(phase);
    return (frac < (double)duty_ / 100 ? 1 : -1) * (double)duty_ / 100;
}

void instruments::Square::ConvertToBytes(const Note& note) {
    Oscillator::ConvertToBytes(note);
}
