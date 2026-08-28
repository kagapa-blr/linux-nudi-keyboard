#include "nudi_composer.hpp"

#include <cctype>

namespace nudi {
namespace {

constexpr const char* virama = "್";
constexpr const char* zwj_virama = "\xE2\x80\x8D\xE0\xB3\x8D";

struct Mapping {
    char key;
    const char* value;
};

constexpr Mapping consonants[] = {
    {'r', "ರ"}, {'s', "ಸ"}, {'d', "ದ"}, {'w', "ಡ"}, {'q', "ಟ"},
    {'t', "ತ"}, {'y', "ಯ"}, {'p', "ಪ"}, {'g', "ಗ"}, {'h', "ಹ"},
    {'j', "ಜ"}, {'l', "ಲ"}, {'k', "ಕ"}, {'z', "ಞ"}, {'x', "ಷ"},
    {'c', "ಚ"}, {'v', "ವ"}, {'b', "ಬ"}, {'n', "ನ"}, {'m', "ಮ"},
};

constexpr Mapping independent_vowels[] = {
    {'a', "ಅ"}, {'e', "ಏ"}, {'i', "ಈ"}, {'o', "ಓ"}, {'u', "ಊ"},
};

constexpr Mapping vowel_signs[] = {
    {'a', "ಾ"}, {'e', "ೆ"}, {'i', "ಿ"}, {'o', "ೊ"}, {'u', "ು"},
    {'v', "ೌ"},
};

constexpr Mapping shifted_vowel_signs[] = {
    {'a', "ಾ"}, {'e', "ೇ"}, {'i', "ೀ"}, {'o', "ೋ"}, {'u', "ೂ"},
    {'y', "ೈ"}, {'v', "ೌ"}, {'r', "ೃ"},
};

constexpr Mapping shifted_consonants[] = {
    {'q', "ಠ"}, {'w', "ಢ"}, {'t', "ಥ"}, {'p', "ಫ"}, {'s', "ಶ"},
    {'d', "ಧ"}, {'g', "ಘ"}, {'j', "ಝ"}, {'k', "ಖ"}, {'l', "ಳ"},
    {'z', "ಙ"}, {'c', "ಛ"}, {'b', "ಭ"}, {'n', "ಣ"},
};

const char* lookup(const Mapping* mappings, std::size_t size, char key) {
    for (std::size_t index = 0; index < size; ++index) {
        if (mappings[index].key == key) return mappings[index].value;
    }
    return nullptr;
}

void remove_last_utf8(std::string& text) {
    if (text.empty()) return;
    std::size_t index = text.size() - 1;
    while (index > 0 && (static_cast<unsigned char>(text[index]) & 0xC0) == 0x80) --index;
    text.erase(index);
}

std::string caps_symbol(char key, bool shifted) {
    if (shifted && key >= '0' && key <= '9') {
        const char* symbols[] = {"꣠", "꣡", "꣢", "꣣", "꣤", "꣥", "꣦", "꣧", "꣨", "꣩"};
        return symbols[key - '0'];
    }
    if (shifted && (key == '[' || key == ']')) return "ೢ";
    if (shifted && key == '{') return "ಌ";
    if (shifted && key == '}') return "ೕ";
    const Mapping symbols[] = {
        {'4', "₹"}, {'0', "ಂ"}, {'1', "̆"}, {'2', "̄"}, {',', "̧"},
        {'-', "̲"}, {'3', "͇"}, {'5', "̍"}, {'6', "̎"}, {'7', "̣"},
        {'\'', "́"}, {'8', "̇"}, {'9', "̈"}, {'.', "̤"}, {'`', "ʻ"},
    };
    const char* value = lookup(symbols, std::size(symbols), key);
    return value == nullptr ? std::string(1, shifted ? std::toupper(key) : key) : value;
}

const char* kannada_digit(char key) {
    constexpr const char* digits[] = {"೦", "೧", "೨", "೩", "೪", "೫", "೬", "೭", "೮", "೯"};
    if (key < '0' || key > '9') return nullptr;
    return digits[key - '0'];
}

}  // namespace

void Composer::reset() {
    text_.clear();
    consonant_pending_ = false;
    virama_pending_ = false;
}

bool Composer::backspace() {
    if (text_.empty()) return false;
    if (text_.size() >= std::string(zwj_virama).size() &&
        text_.compare(text_.size() - std::string(zwj_virama).size(), std::string(zwj_virama).size(), zwj_virama) == 0) {
        text_.erase(text_.size() - std::string(zwj_virama).size());
        consonant_pending_ = true;
        virama_pending_ = true;
        return true;
    }
    remove_last_utf8(text_);
    consonant_pending_ = false;
    virama_pending_ = text_.size() >= std::string(virama).size() &&
                      text_.compare(text_.size() - std::string(virama).size(), std::string(virama).size(), virama) == 0;
    return true;
}

std::string Composer::feed(char key, bool shifted, bool caps_lock, bool scroll_lock) {
    if (scroll_lock && std::isdigit(static_cast<unsigned char>(key))) return std::string(1, key);
    if (caps_lock) {
        if (key == '\'') {
            const std::string quote = quote_right_ ? "’" : "‘";
            quote_right_ = !quote_right_;
            return text_ + quote;
        }
        const std::string symbol = caps_symbol(key, shifted);
        const std::string result = text_ + symbol;
        reset();
        return result;
    }

    if (!shifted && !consonant_pending_ && !virama_pending_) {
        const char* digit = kannada_digit(key);
        if (digit != nullptr) return digit;
    }

    const char* consonant = shifted
        ? lookup(shifted_consonants, std::size(shifted_consonants), key)
        : lookup(consonants, std::size(consonants), key);
    if (consonant != nullptr) {
        if (virama_pending_) text_ += zwj_virama;
        text_ += consonant;
        consonant_pending_ = true;
        virama_pending_ = false;
        return {};
    }
    if (key == 'f' && consonant_pending_) {
        text_ += virama;
        consonant_pending_ = false;
        virama_pending_ = true;
        return {};
    }
    if (consonant_pending_) {
        const char* sign = shifted
            ? lookup(shifted_vowel_signs, std::size(shifted_vowel_signs), key)
            : lookup(vowel_signs, std::size(vowel_signs), key);
        if (sign != nullptr) {
            text_ += sign;
            consonant_pending_ = false;
            virama_pending_ = false;
            return {};
        }
    }
    const Mapping shifted_vowels[] = {
        {'a', "ಆ"}, {'e', "ಏ"}, {'i', "ಈ"}, {'o', "ಓ"}, {'u', "ಊ"},
    };
    const char* vowel = shifted
        ? lookup(shifted_vowels, std::size(shifted_vowels), key)
        : lookup(independent_vowels, std::size(independent_vowels), key);
    std::string result = text_ + (vowel != nullptr ? vowel : std::string(1, key));
    reset();
    return result;
}

}  // namespace nudi