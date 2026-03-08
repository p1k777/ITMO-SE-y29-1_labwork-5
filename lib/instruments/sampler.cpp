#define _USE_MATH_DEFINES


#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <ios>
#include <string>
#include <unordered_map>


#include "defines.hpp"
#include "instrument.hpp"
#include "sampler.hpp"


instruments::Sampler::Sampler(const std::string& name)
    : Instrument(name)
    , root_(0)
    , loop_start_(0)
    , loop_end_(0)
{}

Errors instruments::Sampler::Build(const std::unordered_map<std::string, std::string>& args) {
    if (!args.contains("sample")) {
        return kSyntaxError;
    }

    std::ifstream sample_file(args.at("sample"), std::ios::binary);
    if (!sample_file.is_open()) {
        return kIoError;
    }

    Errors err;
    err = ReadSample(sample_file); 
    if ((int)err) {
        return err;
    }

    if (!args.contains("root")) {
        return kSyntaxError;
    }
    root_ = std::stod(args.at("root"));

    if (args.contains("loop")) {
        const std::string& loop_arg = args.at("loop");
        size_t comma_pos = loop_arg.find(',');

        if (comma_pos == std::string::npos) {
            return kSyntaxError;
        }

        loop_start_ = std::stoul(loop_arg.substr(0, comma_pos));
        loop_end_ = std::stoul(loop_arg.substr(comma_pos + 1));
    }

    if (args.contains("attack")) {
        attack_ = std::stod(args.at("attack"));
    }

    if (args.contains("release")) {
        release_ = std::stod(args.at("release"));
    }

    return itmoloops_funcs::CheckArgsValidity(args, kValidArgs);
}

namespace {
    uint16_t ToUint16(char buff[]) {
        return buff[0] + (buff[1] << 8);
    }
    uint32_t ToUint32(char buff[]) {
        return ToUint16(buff) + (ToUint16(buff + 2) << 16);
    }

    uint32_t ReadUint32(std::ifstream& inp) {
        char buff[4];
        inp.read(buff, 4);
        return ToUint32(buff);
    }

    uint16_t ReadUint16(std::ifstream& inp) {
        char buff[2];
        inp.read(buff, 2);
        return ToUint16(buff);
    }
} // anonymous namespace

Errors instruments::Sampler::ReadSample(std::ifstream& inp) {
    inp.seekg(40);
    size_t bytes_count = ReadUint32(inp);

    for (size_t i = 0; i < bytes_count / 2; i++) {
        sample_.push_back(ReadUint16(inp));
    }

    return kOk;
}

namespace {
    double Sinc(double x) {
        return x ? std::sin(M_PI * x) / (M_PI * x) : 1;
    }
    double Window(double x, size_t radius) {
        return std::abs(x) <= radius ? 0.5 * (1 + std::cos(M_PI * x / radius)) : 0;
    }
} // anonymous namespace
double instruments::Sampler::RestoredFuncVal(double t, double freq) const {
    double pos = t * kFreq;

    size_t int_pos = pos;
    if (int_pos >= sample_.size()) {
        return 0;
    }

    size_t start = std::max(kRestoringWindowRadius, (double)int_pos) - kRestoringWindowRadius;
    size_t end = std::min(int_pos + kRestoringWindowRadius, (double)sample_.size());

    double sum1 = 0;
    double sum2 = 0;
    for (size_t i = start; i < end; i++) {
        double x = (double)i - pos;
        double sinc = Sinc(x);
        double wind = Window(x, kRestoringWindowRadius);

        sum1 += sample_[i] * sinc * wind;
        sum2 += sinc * wind;
    }

    return sum2 ?  sum1 / sum2 : 0.;
}
double instruments::Sampler::CalcCoef(double t, double freq) const {
    double sample_val = RestoredFuncVal(t, freq);

    return sample_val / (double)kMaxVelocity;
}

void instruments::Sampler::ConvertToBytes(const Note& note) {
    if (thread_.size() < note.start + note.duration) {
        thread_.resize(note.start + note.duration);
    }

    double rate = note.freq / root_;
    double sample_t = 0;
    double duration_in_sec = (double)note.duration / kFreq;
    for (size_t idx = 0; idx < note.duration; idx++) {
        double t = (double)idx / kFreq;
        double ampl = CalcAmplitude(t, duration_in_sec, note.velocity);

        if (loop_end_ > loop_start_) {
            double loop_start_s = (double)loop_start_ / kFreq;
            double loop_len_s = (double)(loop_end_ - loop_start_) / kFreq;
            if (sample_t >= loop_start_s + loop_len_s) {
                sample_t = loop_start_s + std::fmod(sample_t - loop_start_s, loop_len_s);
            }
        }
        double sin = CalcCoef(sample_t, note.freq);
        thread_[note.start + idx] += ampl * sin;

        sample_t += rate / kFreq;
    }
}

