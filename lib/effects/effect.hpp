#pragma once


#include <string>
#include <unordered_map>
#include <vector>

#include "defines.hpp"


namespace effects {
    class Effect {
    public:
        Effect() = default;

        virtual Errors Build(const std::unordered_map<std::string, std::string>&);
        virtual void operator()(std::vector<double>&) const;

        virtual ~Effect() = default;
    private:

        bool IsValidArg(const std::string& arg, const std::vector<std::string>& valid_args);
    };
} // namespace effects
