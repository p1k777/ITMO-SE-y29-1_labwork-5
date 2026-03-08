#include <cstddef>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>


#include "filepaser.hpp"
#include "defines.hpp"

#include "instruments/instrument.hpp"
#include "instruments/sampler.hpp"
#include "instruments/sine.hpp"
#include "instruments/square.hpp"
#include "instruments/triangle.hpp"

#include "effects/effect.hpp"
#include "effects/echo.hpp"
#include "effects/gain.hpp"
#include "effects/tremolo.hpp"

std::unordered_map<std::string, float> FileParser::pitches_ = {};

Errors FileParser::ParsePitches(std::ifstream& inp) {
    if (!inp.is_open()) {
        return kIoError;
    }

    std::string pitch;
    float freq;
    while (inp >> pitch) {
        inp >> freq;
        if (inp.eof()) {
            return kNotesPreParsingError;
        }

        pitches_[pitch] = freq;
    }

    return kOk;
}

namespace {
    void Strip(std::string& str) {
        size_t start = 0;
        while (str[start] == ' ' || str[start] == '\t') {
            start++;
        }
        size_t end = str.size();
        while (str[end - 1] == ' ' || str[end - 1] == '\t') {
            end--;
        }

        str = str.substr(start, end - start);
    }

    Errors GetKey(const std::string& arg, std::string& key) {
        size_t equals_sign_idx = arg.find('=');
        if (equals_sign_idx == std::string::npos) {
            return kSyntaxError;
        }

        key = arg.substr(0, equals_sign_idx);
        Strip(key);

        return kOk;
    }
    Errors GetVal(const std::string& arg, std::string& val) {
        size_t equals_sign_idx = arg.find('=');
        if (equals_sign_idx == std::string::npos) {
            return kSyntaxError;
        }

        val = arg.substr(equals_sign_idx + 1);
        Strip(val);

        return kOk;
    }

    bool IsInt(const std::string& str) {
        for (char ch : str) {
            if (!('0' <= ch && ch <= '9')) {
                return false;
            }
        }

        return true;
    }
    bool IsDouble(const std::string& str) {
        bool met_dot = false;
        for (char ch : str) {
            if (!('0' <= ch && ch <= '9') && (ch != '.' || met_dot)) {
                return false;
            }

            met_dot = met_dot || ch == '.';
        }

        return true;
    }
} // anonymous namespace

bool FileParser::TokenHasType(const std::vector<Token>::iterator& token_iter, Token::TokenType token_type) {
    return token_iter != tokens_.end() && token_iter->token_type == token_type;
}
bool FileParser::IsValue(const std::vector<Token>::iterator& token_iter) {
    return TokenHasType(token_iter, kTokInt) ||
           TokenHasType(token_iter, kTokDouble) ||
           TokenHasType(token_iter, kTokStr);
}

const std::unordered_map<std::string, instruments::Instrument*>& FileParser::GetInstruments() const {
    return instruments_;
}

Errors FileParser::ParseToken(const std::string& token) {
    if (token.empty()) {
        return kSyntaxError;
    }

    Token new_token;

    if (token == "bpm") {
        new_token.token_type = kTokBpmKw;
        new_token.val = "BPM";
    } else if (token == "instrument") {
        new_token.token_type = kTokInstrKw;
        new_token.val = "INSTR";
    } else if (token == "pattern") {
        new_token.token_type = kTokPatternKw;
        new_token.val = std::to_string(tokens_.size());
    } else if (token == "effect") {
        new_token.token_type = kTokEffectKw;
        new_token.val = "EFF";
    } else if (token == "end") {
        new_token.token_type = kTokEndKw;
        new_token.val = "END";
    } else if (token[0] == '@') {
        new_token.token_type = kTokPatternCall;
        new_token.val = token.substr(1);
        if (new_token.val.empty()) {
            return kSyntaxError;
        }
    } else if (IsInt(token)) {
        new_token.token_type = kTokInt;
        new_token.val = token;
    } else if (IsDouble(token)) {
        new_token.token_type = kTokDouble;
        new_token.val = token;
    } else if (pitches_.contains(token)) {
        new_token.token_type = kTokDouble;
        new_token.val = std::to_string(pitches_[token]);
    } else {
        new_token.token_type = kTokStr;
        new_token.val = token;
    }

    tokens_.push_back(new_token);

    return kOk;
}
Errors FileParser::ParseCfgTokens(std::ifstream& inp) {
    std::string line;
    while (std::getline(inp, line)) {
        if (line.empty()) {
            continue;
        }
        std::stringstream stream(line);

        std::string buff;
        while (stream >> buff) {
            if (buff[0] == '#') {
                break;
            }

            size_t idx = buff.find('=');
            if (idx != std::string::npos) {
                std::string tmp;
                GetKey(buff, tmp);
                ParseToken(tmp);

                GetVal(buff, tmp);
                ParseToken(tmp);
            } else {
                ParseToken(buff);
            }
        }

        if (!tokens_.empty() && tokens_[tokens_.size() - 1].token_type != kTokEndline) {
            tokens_.push_back(Token{.token_type = kTokEndline, .val = "\\n"});
        }
    }

    return kOk;
}

Errors FileParser::ParseArg(std::unordered_map<std::string, std::string>& args, std::vector<Token>::iterator& token_iter) {
    std::string key = token_iter->val;
    token_iter++;

    if (!IsValue(token_iter)) {
        return kSyntaxError;
    }

    if (pitches_.contains(token_iter->val)) {
        args[key] = pitches_[token_iter->val]; // будет приятно в виде аргумента получать сразу частоту
    } else {
        args[key] = token_iter->val;
    }
    
    token_iter++;

    return kOk;
}
Errors FileParser::ParseInstrEffect(instruments::Instrument* instr, std::vector<Token>::iterator& token_iter) {
    token_iter++;

    if (!IsValue(token_iter)) {
        return kSyntaxError;
    }
    std::string eff_type = token_iter->val;
    token_iter++;

    effects::Effect* new_eff;
    if (eff_type == "echo") {
        new_eff = new effects::Echo();
    } else if (eff_type == "gain") {
        new_eff = new effects::Gain();
    } else if (eff_type == "tremolo") {
        new_eff = new effects::Tremolo();
    } else {
        return kSyntaxError;
    }


    Errors err;
    std::unordered_map<std::string, std::string> eff_args;
    while (!TokenHasType(token_iter, kTokEndline)) {
        err = ParseArg(eff_args, token_iter);
        if ((int)err) {
            return err;
        }
    }

    return new_eff->Build(eff_args);
}
Errors FileParser::ParseInstrument(std::vector<Token>::iterator& token_iter) {
    if (!TokenHasType(token_iter, kTokInstrKw)) {
        return kSyntaxError;
    }
    token_iter++;

    if (!TokenHasType(token_iter, kTokStr)) {
        return kSyntaxError;
    }
    std::string name = token_iter->val;
    token_iter++;


    if (!TokenHasType(token_iter, kTokStr)) {
        return kSyntaxError;
    }
    std::string instr_type = token_iter->val;
    token_iter++;

    instruments::Instrument* new_instr;
    std::unordered_map<std::string, std::string> args;
    Errors err;
    if (instr_type == "sampler") {
        new_instr = new instruments::Sampler(name);
    } else if (instr_type == "sine") {
        new_instr = new instruments::Sine(name);
    } else if (instr_type == "square") {
        new_instr = new instruments::Square(name);
    } else if (instr_type == "triangle") {
        new_instr = new instruments::Triangle(name);
    } else {
        return kSyntaxError;
    }

    while (!TokenHasType(token_iter, kTokEndKw)) {
        if (TokenHasType(token_iter, kTokEndline)) {
            token_iter++;
            continue;
        }

        if (TokenHasType(token_iter, kTokEffectKw)) {
            err = ParseInstrEffect(new_instr, token_iter);
            if ((int)err) {
                return err;
            }
            continue;
        }

        if (TokenHasType(token_iter, kTokStr)) {
            err = ParseArg(args, token_iter);
            if ((int)err) {
                return err;
            }
            continue;
        }

        return kSyntaxError;
    }
    err = new_instr->Build(args);
    if ((int)err) {
        return err;
    }
    instruments_[name] = new_instr;

    token_iter++;

    return kOk;
}

Errors FileParser::ParseNote(size_t start, std::vector<Token>::iterator& token_iter, instruments::Note& note, size_t resolution) {
    if (!TokenHasType(token_iter, kTokDouble)) {
        return kSyntaxError;
    }

    note.start = start;

    note.freq = std::stod(token_iter->val);
    token_iter++;

    if (!TokenHasType(token_iter, kTokInt)) {
        return kSyntaxError;
    }
    note.duration = std::stoull(token_iter->val) * kFreq * 60 / (bpm * resolution);
    token_iter++;

    if (!TokenHasType(token_iter, kTokInt)) {
        return kSyntaxError;
    }
    note.velocity = std::stoi(token_iter->val);
    token_iter++;

    return kOk;
}

Errors FileParser::CallPattern(std::vector<Token>::iterator token_iter, size_t start_in_samples, size_t last_resolution) {
    size_t unit_size = 0;

    if (!TokenHasType(token_iter, kTokPatternKw)) {
        return kSyntaxError;
    }
    token_iter++;

    if (!TokenHasType(token_iter, kTokStr)) {
        return kSyntaxError;
    }
    token_iter++;

    if (token_iter->val != "resolution") {
        return kSyntaxError;
    }
    token_iter++;
    
    if (!TokenHasType(token_iter, kTokInt)) {
        return kSyntaxError;
    }
    int32_t resolution = std::stoi(token_iter->val) * last_resolution;
    token_iter++;
    size_t samples_in_unit = kFreq * 60 / (bpm * resolution); // для этого паттерна

    while (!TokenHasType(token_iter, kTokEndKw)) {
        if (TokenHasType(token_iter, kTokEndline)) {
            token_iter++;
            continue;
        }

        if (!TokenHasType(token_iter, kTokInt)) {
            return kSyntaxError;
        }

        size_t start = std::stoull(token_iter->val) * samples_in_unit + start_in_samples;
        token_iter++;

        Errors err;
        if (TokenHasType(token_iter, kTokPatternCall)) {
            if (!patterns_.contains(token_iter->val)) {
                return kNoSuchPattern;
            }

            err = CallPattern(tokens_.begin() + patterns_[token_iter->val], start, resolution);
            if ((int)err) {
                return err;
            }

            token_iter++;
            continue;
        }

        if (!TokenHasType(token_iter, kTokStr)) {
            return kSyntaxError;
        }
        if (!instruments_.contains(token_iter->val)) {
            return kNoSuchInstr;
        }
        instruments::Instrument* instr = instruments_[token_iter->val];
        token_iter++;


        instruments::Note note;
        err = ParseNote(start, token_iter, note, resolution);
        if ((int)err) {
            return err;
        }

        instr->ConvertToBytes(note);
    }

    return kOk;
}

Errors FileParser::ParseCfg(std::ifstream& inp) {
    if (!inp.is_open()) {
        return kIoError;
    }

    ParseCfgTokens(inp);

    if (tokens_.size() < 2 ||
        tokens_[0].token_type != Token::TokenType::kBpmKw ||
        tokens_[1].token_type != Token::TokenType::kInt) {
        return kSyntaxError;
    }

    extern int32_t bpm;
    bpm = std::stoi(tokens_[1].val);
    
    if (tokens_.size() < 3) {
        return kOk;
    }

    Errors err;
    auto token_iter = tokens_.begin();
    token_iter += 2;
    while (token_iter != tokens_.end()) {
        if (TokenHasType(token_iter, kTokEndline)) {
            token_iter++;
            continue;
        }

        if (TokenHasType(token_iter, kTokInstrKw)) {
            err = ParseInstrument(token_iter);
            if ((int)err) {
                return err;
            }

            continue;
        }

        if (TokenHasType(token_iter, kTokPatternKw)) {
            size_t val = std::stoull(token_iter->val);
            token_iter++;

            if (!TokenHasType(token_iter, kTokStr)) {
                return kSyntaxError;
            }
            std::string name = token_iter->val;
            token_iter++;

            if (name == "main") {
                token_iter -= 2;
                return CallPattern(token_iter, 0);
            }

            patterns_[name] = val;

            while (!TokenHasType(token_iter, kTokEndKw)) { token_iter++; }
            token_iter++;

            continue;
        }

        return kSyntaxError;
    }

    return kSyntaxError;
}

FileParser::~FileParser() {
    for (auto instr : instruments_) {
        delete instr.second;
    }
}
