#pragma once


#include <string>
#include <vector>
#include <unordered_map>


#include "defines.hpp"
#include "effect.hpp"


namespace effects {
    class Echo final : public Effect {
    public:
        Echo() = default;

        Errors Build(const std::unordered_map<std::string, std::string>&) override;

        void operator()(std::vector<double>&) const override;
    private:

        inline static const std::vector<std::string> kValidArgs = {"delay", "decay"};

        double delay_;
        double decay_;
    };
} // namespace effects