//
// Created by harshavardhan on 21/6/24.
//


#include "InputOutputTypesForSokets.h"

std::ostream& operator<<(std::ostream& os, SocketDataType e) {
    switch (e) {
        case SocketDataType::MIDI: return os << "MIDI";
        case SocketDataType::AudioBufferFloat: return os << "AudioBuffer";
        case SocketDataType::Floating: return os << "Floating Point";
        case SocketDataType::NULLType: return os << "None";
    }
    return os;
}
