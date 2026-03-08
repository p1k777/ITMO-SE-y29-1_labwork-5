#include <string>
#include <unordered_map>
#include <vector>


#include "defines.hpp"
#include "effect.hpp"


void effects::Effect::operator()(std::vector<double>&) const {}

Errors effects::Effect::Build(const std::unordered_map<std::string, std::string>&) {
    return kOtherErrors; // если вызвался этот метод, то кто-то что-то сделал не так
}
