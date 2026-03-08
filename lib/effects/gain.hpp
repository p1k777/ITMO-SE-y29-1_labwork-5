#pragma once


#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>


#include "effect.hpp"
#include "defines.hpp"


namespace effects {
    class Gain final : public Effect {
    public:
        Gain() = default;

        Errors Build(const std::unordered_map<std::string, std::string>&) override;

        void operator()(std::vector<double>&) const override;

    private:

        inline static const std::vector<std::string> kValidArgs = {"gain"};

        double gain_;
    };
} // namespace effects
