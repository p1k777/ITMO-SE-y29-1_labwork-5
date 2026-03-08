#include <iostream>
#include <fstream>

#include "defines.hpp"
#include "filepaser.hpp"
#include "wavwriter.hpp"



Errors ParsePaths(int argc, char* argv[], std::ifstream& in, std::fstream& out) {
    if (argc != 3) {
        return kWrongArgs;
    }

    if (in.is_open()) {
        in.close();
    }

    if (out.is_open()) {
        out.close();
    }

    in.open(argv[1]);
    if (!in.is_open()) {
        return kIoError;
    }

    out.open(argv[2], std::ios::in);
    if (out.is_open()) {
        out.close();

        std::cerr << "[WARNING]: File \"" << argv[2] << "\" already exists. Rewrite it?: ";
        char res;
        std::cin >> res;

        if (res != 'y' && res != 'Y') {
            in.close();

            return kLightExit;
        }
    }

    out.open(argv[2], std::ios::out);
    out.close();

    out.open(argv[2], std::ios::in | std::ios::out | std::ios::binary);

    return kOk;
}


int main(int argc, char* argv[]) {
    std::ifstream input_path;
    std::fstream output_path;

    Errors err;

    // открываем рабочие файлы
    err = ParsePaths(argc, argv, input_path, output_path);
    if (err == kLightExit) {
        return 0;
    }
    if ((int)err) {
        return (int)err;
    }

    WavWriter writer;
    writer.WriteHeader(output_path);

    // парсим ноты из файла
    std::ifstream pitches("notes.txt");
    FileParser file_parser;
    err = file_parser.ParsePitches(pitches);
    if ((int)err) {
        return (int)err;
    }
    pitches.close();

    err = file_parser.ParseCfg(input_path);

    if ((int)err) {
        return (int)err;
    }

    for (auto instr : file_parser.GetInstruments()) {
        writer.WriteThread(*instr.second, output_path);
    }

    return 0;
}
