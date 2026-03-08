#pragma once


#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>


#include "defines.hpp"
#include "../effects/effect.hpp"


namespace instruments {

    struct Note {
        size_t start;
        double freq;
        size_t duration;
        int8_t velocity;
    };

    class Instrument {
    public:
        Instrument(const std::string& name);

        Instrument(const Instrument&) = delete;
        Instrument& operator=(const Instrument&) = delete;

        virtual Errors Build(const std::unordered_map<std::string, std::string>& args);
        virtual void ConvertToBytes(const Note&);

        void AddEffect(effects::Effect*);

        const std::vector<double>& GetThread();

        virtual ~Instrument();

    protected:

        void ApplyEffects();

        double CalcAmplitude(double t, double duration_in_sec, int8_t velocity) const;

        virtual double CalcCoef(double t, double freq) const;
        
        std::string name_;
        double attack_;
        double release_;
        std::vector<double> thread_; // дорожка конкретного инструмента
        std::vector<effects::Effect*> effects_;
    };
} // namespace instruments
