#pragma once


#include <string>
#include <unordered_map>
#include <vector>


#include "effect.hpp"
#include "defines.hpp"


namespace effects {
    class Tremolo final : public Effect {
    public:
        Tremolo() = default;

        Errors Build(const std::unordered_map<std::string, std::string>&) override;

        void operator()(std::vector<double>&) const override;
    private:

        inline static const std::vector<std::string> kValidArgs = {"freq", "depth"};

        double freq_;
        double depth_;
    };
} // namespace effects

