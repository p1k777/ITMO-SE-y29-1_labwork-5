#pragma once


#include <fstream>
#include <string>
#include <unordered_map>


#include "defines.hpp"
#include "instruments/instrument.hpp"


class FileParser final {
public:
    FileParser() = default;

    Errors ParseCfg(std::ifstream&);
    Errors ParsePitches(std::ifstream&);

    const float GetPitch(const std::string& pitch_notation);

    const std::unordered_map<std::string, instruments::Instrument*>& GetInstruments() const;

    ~FileParser();

private:

    struct Token {
        enum class TokenType : int8_t {
            kInt = 0,
            kDouble = 1,
            kStr = 2,
            kBpmKw = 3,
            kInstrKw = 4,
            kPatternKw = 5,
            kPatternCall = 6,
            kEffectKw = 7,
            kEndline = 8,
            kEndKw = 9,
        };

        TokenType token_type;
        std::string val;
    };

    bool IsValue(const std::vector<Token>::iterator&);
    bool TokenHasType(const std::vector<Token>::iterator&, Token::TokenType token_type);

    Errors ParseToken(const std::string&);
    Errors ParseCfgTokens(std::ifstream&);
    
    Errors ParseArg(std::unordered_map<std::string, std::string>& args, std::vector<Token>::iterator&);
    Errors ParseInstrEffect(instruments::Instrument* instr, std::vector<Token>::iterator&);
    Errors ParseInstrument(std::vector<Token>::iterator&);

    Errors ParseNote(size_t start, std::vector<Token>::iterator&, instruments::Note&, size_t resolution);

    Errors CallPattern(std::vector<Token>::iterator, size_t start_in_samples, size_t last_resolution=1);
    Errors ParseMainPattern(std::vector<Token>::iterator&);


    std::unordered_map<std::string, size_t> patterns_;
    static std::unordered_map<std::string, float> pitches_;
    std::unordered_map<std::string, instruments::Instrument*> instruments_;

    std::vector<Token> tokens_;
    inline static const Token::TokenType kTokInt = Token::TokenType::kInt;
    inline static const Token::TokenType kTokDouble = Token::TokenType::kDouble;
    inline static const Token::TokenType kTokStr = Token::TokenType::kStr;
    inline static const Token::TokenType kTokBpmKw = Token::TokenType::kBpmKw;
    inline static const Token::TokenType kTokInstrKw = Token::TokenType::kInstrKw;
    inline static const Token::TokenType kTokPatternKw = Token::TokenType::kPatternKw;
    inline static const Token::TokenType kTokPatternCall = Token::TokenType::kPatternCall;
    inline static const Token::TokenType kTokEffectKw = Token::TokenType::kEffectKw;
    inline static const Token::TokenType kTokEndline = Token::TokenType::kEndline;
    inline static const Token::TokenType kTokEndKw = Token::TokenType::kEndKw;
};