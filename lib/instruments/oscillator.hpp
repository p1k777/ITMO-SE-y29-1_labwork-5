#pragma once


#include <string>
#include <unordered_map>


#include "defines.hpp"
#include "instrument.hpp"


namespace instruments {
    class Oscillator : public Instrument {
    public:
        Oscillator(const std::string& name);

        virtual Errors Build(const std::unordered_map<std::string, std::string>& args) override;
        virtual void ConvertToBytes(const Note&) override;
    private:
        
        double CalcCoef(double t, double freq) const override;
    };
} // namespace instruments


