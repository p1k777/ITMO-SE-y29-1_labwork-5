#pragma once


#include "defines.hpp"
#include "instrument.hpp"
#include "oscillator.hpp"
#include "oscillator.hpp"


namespace instruments {
    class Triangle final : public Oscillator {
    public:
        Triangle(const std::string& name);

        Errors Build(const std::unordered_map<std::string, std::string>&) override;
        void ConvertToBytes(const Note&) override;

    private:

        double CalcCoef(double t, double freq) const override;

        inline static const std::vector<std::string> kValidArgs = {"attack", "release"};

    };
} // namespace instruments