#pragma once


#include <cstdint>
#include <fstream>
#include <string>
#include <unordered_map>

#include "defines.hpp"
#include "instrument.hpp"


namespace instruments {
    class Sampler final : public Instrument {
    public:
        Sampler(const std::string& name);

        Errors Build(const std::unordered_map<std::string, std::string>&) override;
        void ConvertToBytes(const Note&) override;

    private:

        struct SampleFormat {
            uint16_t audio_format;
            uint16_t num_channels;
            uint32_t sample_rate;
            uint32_t byte_rate;
            uint16_t block_align;
            uint16_t bits_per_sample;
        };

        Errors ReadSample(std::ifstream& inp);

        double RestoredFuncVal(double t, double freq) const;
        double CalcCoef(double t, double freq) const override;

        inline static const std::vector<std::string> kValidArgs = {"sample", "root", "loop", "attack", "release"};
        inline static const double kRestoringWindowRadius = 16.0;

        std::vector<int16_t> sample_;
        double root_;
        uint32_t loop_start_;
        uint32_t loop_end_;
    };
} // namespace instruments
