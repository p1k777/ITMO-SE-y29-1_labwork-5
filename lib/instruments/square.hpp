#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "defines.hpp"
#include "instrument.hpp"
#include "oscillator.hpp"
#include "oscillator.hpp"

namespace instruments {
    class Square final : public Oscillator {
    public:
        Square(const std::string& name);

        Errors Build(const std::unordered_map<std::string, std::string>&) override;
        void ConvertToBytes(const Note&) override;

    private:

        double CalcCoef(double t, double freq) const override;

        inline static const std::vector<std::string> kValidArgs = {"duty", "attack", "release"};

        uint8_t duty_;
    };
} // namespace instruments
