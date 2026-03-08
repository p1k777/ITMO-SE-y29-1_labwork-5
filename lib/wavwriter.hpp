#include <algorithm>
#include <cstdint>
#include <fstream>
#include <vector>


#include "defines.hpp"
#include "instruments/instrument.hpp"


class WavWriter {
public:
    WavWriter() = default;
    void WriteHeader(std::fstream&);
    void WriteThread(instruments::Instrument&, std::fstream&);
private:

    // нормально работает и для int32_t и для uint32_t
    void WriteInt32(std::fstream& out, int32_t num);
    // нормально работает и для int16_t и для uint16_t
    void WriteInt16(std::fstream& out, int16_t num);

    uint32_t ReadUint32(std::fstream& inp);
    uint32_t ReadUint16(std::fstream& inp);

    inline static const uint32_t kHeaderSize = 44;
};
