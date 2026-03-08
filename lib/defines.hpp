#pragma once


#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>


enum class Errors {
    kOk = 0,
    kIoError = 1,
    kWrongArgs = 2,
    kNotesPreParsingError = 3,
    kSyntaxError = 4,
    kNoSuchPitch = 5,
    kNoSuchInstr = 6,
    kNoSuchPattern = 7,
    kOtherErrors = 8,
    kLightExit = -1, // если программа завершается с отмашки пользователя
};

const Errors kOk = Errors::kOk;
const Errors kIoError = Errors::kIoError;
const Errors kWrongArgs = Errors::kWrongArgs;
const Errors kNotesPreParsingError = Errors::kNotesPreParsingError;
const Errors kSyntaxError = Errors::kSyntaxError;
const Errors kNoSuchPitch = Errors::kNoSuchPitch;
const Errors kNoSuchInstr = Errors::kNoSuchInstr;
const Errors kNoSuchPattern = Errors::kNoSuchPattern;
const Errors kOtherErrors = Errors::kOtherErrors;
const Errors kLightExit = Errors::kLightExit;


const int32_t kFreq = 44100; // частота дискретизации
const int16_t kMaxVelocity = INT16_MAX; // максимальная громкость в сэмпле
extern int32_t bpm; // bpm

namespace itmoloops_funcs {
    Errors CheckArgsValidity(const std::unordered_map<std::string, std::string>&, const std::vector<std::string>&);

    void Split(const std::string&, std::vector<std::string>&); // сплитует по "\t" и " " + удаляет их из начала строки
} // namespace itmoloops_funcs
