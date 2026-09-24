#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstddef>
#include <cstring>

extern "C" {

std::size_t APS5_VABI strnlen_nid_postfix(const char* text, std::size_t limit) {
    std::size_t length = 0;
    while (length < limit && text[length] != '\0') ++length;
    return length;
}

char* APS5_VABI strncat_nid_postfix(char* destination, const char* source, std::size_t limit) {
    return std::strncat(destination, source, limit);
}

char* APS5_VABI strpbrk_nid_postfix(const char* text, const char* accept) {
    return const_cast<char*>(std::strpbrk(text, accept));
}

std::size_t APS5_VABI strcspn_nid_postfix(const char* text, const char* reject) {
    return std::strcspn(text, reject);
}

std::size_t APS5_VABI strlcat_nid_postfix(char* destination, const char* source, std::size_t capacity) {
    const auto destinationLength = strnlen_nid_postfix(destination, capacity);
    const auto sourceLength = std::strlen(source);
    if (destinationLength < capacity) {
        const auto remaining = capacity - destinationLength - 1;
        const auto count = sourceLength < remaining ? sourceLength : remaining;
        std::memcpy(destination + destinationLength, source, count);
        destination[destinationLength + count] = '\0';
    }
    return destinationLength + sourceLength;
}

char* APS5_VABI strtok_r_nid_postfix(char* text, const char* delimiters, char** state) {
    if (text == nullptr) text = *state;
    if (text == nullptr) return nullptr;
    text += std::strspn(text, delimiters);
    if (*text == '\0') {
        *state = text;
        return nullptr;
    }
    char* end = text + std::strcspn(text, delimiters);
    if (*end != '\0') *end++ = '\0';
    *state = end;
    return text;
}

char* APS5_VABI strtok_nid_postfix(char* text, const char* delimiters) {
    thread_local char* state = nullptr;
    return strtok_r_nid_postfix(text, delimiters, &state);
}

}
