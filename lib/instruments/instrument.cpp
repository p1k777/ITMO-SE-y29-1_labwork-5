#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>


#include "instrument.hpp"
#include "defines.hpp"
#include "../effects/effect.hpp"


instruments::Instrument::Instrument(const std::string& name)
    : name_(name)
    , attack_(0)
    , release_(0)
{}

Errors instruments::Instrument::Build(const std::unordered_map<std::string, std::string>& args) {
    return kOtherErrors; // если вызвался этот метод, то кто-то что-то сделал не так
}

void instruments::Instrument::ConvertToBytes(const Note&) {}

double instruments::Instrument::CalcAmplitude(double t, double duration_in_sec, int8_t velocity) const {
    double def = (double)velocity / 100 * (double)kMaxVelocity;

    if (t < attack_ && attack_) {
        return std::min(def, def * t / attack_);
    }

    

    if (t > duration_in_sec - release_ && release_) {
        return std::min(def, def * (duration_in_sec - t) / release_);
    }
    
    return def;
}

double instruments::Instrument::CalcCoef(double t, double freq) const {
    return 0;
}

void instruments::Instrument::AddEffect(effects::Effect* effect_ptr) {
    effects_.push_back(effect_ptr);
}

void instruments::Instrument::ApplyEffects() {
    for (effects::Effect* eff : effects_) {
        (*eff)(thread_);
    }
}

const std::vector<double>& instruments::Instrument::GetThread() {
    ApplyEffects();
    return thread_;
}

instruments::Instrument::~Instrument() {
    for (effects::Effect* eff : effects_) {
        delete eff;
    }
}
