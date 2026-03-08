#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ios>
#include <vector>


#include "defines.hpp"
#include "wavwriter.hpp"
#include "instruments/instrument.hpp"


void WavWriter::WriteInt32(std::fstream& out, int32_t num) {
    uint8_t buff[4]{0};
    for (int i = 0; i < 4; i++) {
        buff[i] = (uint8_t)(((uint32_t)num >> (8 * i)) & 0xff);
    }
    out.write(reinterpret_cast<char*>(buff), 4);
}
void WavWriter::WriteInt16(std::fstream& out, int16_t num) {
    uint8_t buff[2] = {(uint8_t)((uint16_t)num & 0xff), (uint8_t)((uint16_t)num >> 8)};
    out.write(reinterpret_cast<char*>(buff), 2);
}

uint32_t WavWriter::ReadUint32(std::fstream& inp) {
    uint8_t buff[4];
    inp.read(reinterpret_cast<char*>(buff), 4);

    uint32_t res = 0;
    for (int i = 0; i < 4; i++) {
        res += buff[i] << (8*i);
    }

    return res;
}
uint32_t WavWriter::ReadUint16(std::fstream& inp) {
    uint8_t buff[2];
    inp.read(reinterpret_cast<char*>(buff), 2);

    uint32_t res = 0;
    for (int i = 0; i < 2; i++) {
        res += buff[i] << (8*i);
    }

    return res;
}

void WavWriter::WriteHeader(std::fstream& file) {
    file.seekp(0, std::ios::beg);

    file.write("RIFF", 4);
    WriteInt32(file, 36); // размер файла

    file.write("WAVE", 4);
    file.write("fmt ", 4);
    
    // размер fmt чанка
    WriteInt32(file, 16);

    // тип звука
    WriteInt16(file, 1);

    // количество каналов
    WriteInt16(file, 1);

    // частота дискретизации + byterate
    WriteInt32(file, 44100);
    WriteInt32(file, 88200);

    // размер фрейма
    WriteInt16(file, 2);

    // глубина кодирования
    WriteInt16(file, 16);

    file.write("data", 4);
    WriteInt32(file, 0); // размер музыки
}

void WavWriter::WriteThread(instruments::Instrument& instr, std::fstream& file) {
    const std::vector<double>& thread = instr.GetThread();

    file.clear();
    file.seekg(40, std::ios::beg);
    uint32_t old_size = ReadUint32(file);
    if (!file) {
        file.clear();
        old_size = 0;
    }
    uint32_t size = std::max((uint32_t)thread.size() * 2, old_size);

    file.seekp(40, std::ios::beg); // тут начинается количество байт с информацией
    WriteInt32(file, size);

    file.seekp(4, std::ios::beg); // тут начинается количество байт с информацией
    WriteInt32(file, size + kHeaderSize - 8);

    size_t old_samples = old_size / 2;
    size_t total_samples = size / 2;
    size_t blocks_size_in_samples = 4096;
    std::vector<int16_t> mix;
    mix.reserve(blocks_size_in_samples);

    for (size_t offset = 0; offset < total_samples; offset += blocks_size_in_samples) {
        size_t block_len = std::min(blocks_size_in_samples, total_samples - offset);
        mix.clear();

        file.clear();
        file.seekg(44 + offset * 2, std::ios::beg);
        for (size_t i = 0; i < block_len; i++) {
            size_t idx = offset + i;
            int16_t sample = idx < old_samples ? (int16_t)ReadUint16(file) : 0;
            double sum = (double)sample;
            if (idx < thread.size()) {
                sum += thread[idx];
            }
            if (sum > kMaxVelocity) {
                sum = kMaxVelocity;
            } else if (sum < -kMaxVelocity) {
                sum = -kMaxVelocity;
            }
            mix.push_back((int16_t)sum);
        }

        file.clear();
        file.seekp(44 + offset * 2, std::ios::beg);
        for (size_t i = 0; i < block_len; i++) {
            WriteInt16(file, mix[i]);
        }
    }
}
