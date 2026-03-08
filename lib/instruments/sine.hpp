#pragma once

#include <string>
#include <unordered_map>
#include <vector>



#include "defines.hpp"
#include "instrument.hpp"
#include "oscillator.hpp"
#include "oscillator.hpp"


namespace instruments {
    class Sine final : public Oscillator {
    public:
        Sine(const std::string& name);

        Errors Build(const std::unordered_map<std::string, std::string>&) override;
        void ConvertToBytes(const Note&) override;
    private:

        double CalcCoef(double t, double freq) const override;

        inline static const std::vector<std::string> kValidArgs = {"attack", "release"};
    };
} // namespace instruments
