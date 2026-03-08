#include <cstddef>
#include <string>
#include <unordered_map>


#include "defines.hpp"
#include "instrument.hpp"
#include "oscillator.hpp"


instruments::Oscillator::Oscillator(const std::string& name)
    : Instrument(name)
{}

Errors instruments::Oscillator::Build(const std::unordered_map<std::string, std::string>& args) {
    return kOtherErrors;
}

double instruments::Oscillator::CalcCoef(double t, double freq) const {
    return 0;
}

void instruments::Oscillator::ConvertToBytes(const Note& note) {
    if (thread_.size() < note.start + note.duration) {
        thread_.resize(note.start + note.duration);
    }

    double duration_in_sec = (double)note.duration / kFreq;
    for (size_t idx = 0; idx < note.duration; idx++) {
        double t = (double)idx / kFreq;
        double ampl = CalcAmplitude(t, duration_in_sec, note.velocity);
        double sin = CalcCoef(t, note.freq);

        thread_[note.start + idx] += (double)ampl * (double)sin;
    }
}
