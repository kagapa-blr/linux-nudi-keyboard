#include "nudi_composer.hpp"

#include <cctype>
#include <cstddef>
#include <string>

namespace nudi
{
    namespace
    {

        // ============================================================
        // Unicode constants
        // ============================================================

        constexpr const char *virama = "್";
        constexpr const char *zwnj = "\xE2\x80\x8C";                   // U+200C
        constexpr const char *zwj_virama = "\xE2\x80\x8D\xE0\xB3\x8D"; // ZWJ + U+0CCD

        // ============================================================
        // Mapping
        // ============================================================

        struct Mapping
        {
            char key;
            const char *value;
        };

        // ============================================================
        // NUDI KANNADA CONSONANTS
        // ============================================================

        constexpr Mapping consonants[] = {
            {'k', "ಕ"},
            {'g', "ಗ"},
            {'c', "ಚ"},
            {'j', "ಜ"},

            {'q', "ಟ"},
            {'w', "ಡ"},

            {'t', "ತ"},
            {'d', "ದ"},

            {'p', "ಪ"},
            {'b', "ಬ"},

            {'y', "ಯ"},
            {'r', "ರ"},
            {'l', "ಲ"},
            {'v', "ವ"},

            {'s', "ಸ"},
            {'h', "ಹ"},

            {'x', "ಷ"},
            {'z', "ಞ"},

            {'n', "ನ"},
            {'m', "ಮ"},
        };

        // ============================================================
        // NUDI SHIFT + CONSONANTS
        // ============================================================

        constexpr Mapping shifted_consonants[] = {
            {'k', "ಖ"},
            {'g', "ಘ"},

            {'c', "ಛ"},
            {'j', "ಝ"},

            {'q', "ಠ"},
            {'w', "ಢ"},

            {'t', "ಥ"},
            {'d', "ಧ"},

            {'p', "ಫ"},
            {'b', "ಭ"},

            {'n', "ಣ"},

            {'l', "ಳ"},

            {'s', "ಶ"},

            {'z', "ಙ"},
        };

        // ============================================================
        // INDEPENDENT VOWELS
        // ============================================================

        constexpr Mapping independent_vowels[] = {
            {'a', "ಅ"},
            {'i', "ಇ"},
            {'u', "ಉ"},
            {'e', "ಎ"},
            {'o', "ಒ"},
        };

        // ============================================================
        // SHIFT + INDEPENDENT VOWELS
        // ============================================================

        constexpr Mapping shifted_independent_vowels[] = {
            {'a', "ಆ"},
            {'i', "ಈ"},
            {'u', "ಊ"},
            {'e', "ಏ"},
            {'o', "ಓ"},

            {'r', "ಋ"},
            {'y', "ಐ"},
            {'v', "ಔ"},
        };

        // ============================================================
        // VOWEL SIGNS - NORMAL
        // ============================================================
        //
        // Used after a consonant.
        //
        // k + a = ಕಾ
        // k + e = ಕೆ
        // k + i = ಕಿ
        // k + o = ಕೊ
        // k + u = ಕು
        //
        // ============================================================

        constexpr Mapping vowel_signs[] = {
            {'a', "ಾ"},
            {'e', "ೆ"},
            {'i', "ಿ"},
            {'o', "ೊ"},
            {'u', "ು"},
        };

        // ============================================================
        // VOWEL SIGNS - SHIFT
        // ============================================================
        //
        // k + Shift+a = ಕಾ
        // k + Shift+e = ಕೇ
        // k + Shift+i = ಕೀ
        // k + Shift+o = ಕೋ
        // k + Shift+u = ಕೂ
        // k + Shift+y = ಕೈ
        // k + Shift+v = ಕೌ
        // k + Shift+r = ಕೃ
        //
        // ============================================================

        constexpr Mapping shifted_vowel_signs[] = {
            {'a', "ಾ"},
            {'e', "ೇ"},
            {'i', "ೀ"},
            {'o', "ೋ"},
            {'u', "ೂ"},

            {'y', "ೈ"},
            {'v', "ೌ"},
            {'r', "ೃ"},
        };

        // ============================================================
        // SPECIAL SIGNS
        // ============================================================

        constexpr Mapping special_signs[] = {
            {'M', "ಂ"}, // Anusvara
            {'h', "ಃ"}, // Visarga
            {'x', "಼"},  // Nukta
        };

        // ============================================================
        // Lookup helper
        // ============================================================

        const char *lookup(
            const Mapping *mappings,
            std::size_t size,
            char key)
        {
            for (std::size_t index = 0; index < size; ++index)
            {
                if (mappings[index].key == key)
                {
                    return mappings[index].value;
                }
            }

            return nullptr;
        }

        // ============================================================
        // UTF-8 backspace helper
        // ============================================================

        void remove_last_utf8(std::string &text)
        {
            if (text.empty())
            {
                return;
            }

            std::size_t index = text.size() - 1;

            while (
                index > 0 &&
                (static_cast<unsigned char>(text[index]) & 0xC0) == 0x80)
            {
                --index;
            }

            text.erase(index);
        }

        // ============================================================
        // Kannada digits
        // ============================================================

        const char *kannada_digit(char key)
        {
            constexpr const char *digits[] = {
                "೦",
                "೧",
                "೨",
                "೩",
                "೪",
                "೫",
                "೬",
                "೭",
                "೮",
                "೯"};

            if (key < '0' || key > '9')
            {
                return nullptr;
            }

            return digits[key - '0'];
        }

        // ============================================================
        // Symbols used when Caps Lock is active
        // ============================================================

        std::string caps_symbol(char key, bool shifted)
        {
            // Shift + number row
            if (shifted && key >= '0' && key <= '9')
            {
                constexpr const char *symbols[] = {
                    "꣠", // 0
                    "꣡", // 1
                    "꣢", // 2
                    "꣣", // 3
                    "꣤", // 4
                    "꣥", // 5
                    "꣦", // 6
                    "꣧", // 7
                    "꣨", // 8
                    "꣩"  // 9
                };

                return symbols[key - '0'];
            }

            // Special bracket mappings
            if (shifted && (key == '[' || key == ']'))
            {
                return "ೢ";
            }

            if (shifted && key == '{')
            {
                return "ಌ";
            }

            if (shifted && key == '}')
            {
                return "ೕ";
            }

            constexpr Mapping symbols[] = {
                {'4', "₹"},

                {'0', "ಂ"},
                {'1', "̆"},
                {'2', "̄"},
                {',', "̧"},
                {'-', "̲"},
                {'3', "͇"},
                {'5', "̍"},
                {'6', "̎"},
                {'7', "̣"},
                {'\'', "́"},
                {'8', "̇"},
                {'9', "̈"},
                {'.', "̤"},
                {'`', "ʻ"},
            };

            const char *value = lookup(
                symbols,
                std::size(symbols),
                key);

            if (value != nullptr)
            {
                return value;
            }

            return std::string(
                1,
                shifted
                    ? static_cast<char>(
                          std::toupper(
                              static_cast<unsigned char>(key)))
                    : key);
        }

    } // namespace

    // ============================================================
    // Composer reset
    // ============================================================

    void Composer::reset()
    {
        text_.clear();

        consonant_pending_ = false;
        virama_pending_ = false;
        explicit_virama_ = false;
    }

    // ============================================================
    // Separator
    // ============================================================

    std::string Composer::separator()
    {
        // If a virama is waiting for another consonant,
        // terminate the sequence using ZWNJ.
        if (virama_pending_)
        {
            text_ += zwnj;
        }

        const std::string result = text_;

        reset();

        return result;
    }

    // ============================================================
    // Backspace
    // ============================================================

    bool Composer::backspace()
    {
        if (text_.empty())
        {
            return false;
        }

        // --------------------------------------------------------
        // Remove ZWJ + VIRAMA
        // --------------------------------------------------------

        const std::string zwjVirama(zwj_virama);

        if (
            text_.size() >= zwjVirama.size() &&
            text_.compare(
                text_.size() - zwjVirama.size(),
                zwjVirama.size(),
                zwjVirama) == 0)
        {
            text_.erase(
                text_.size() - zwjVirama.size());

            consonant_pending_ = true;
            virama_pending_ = true;
            explicit_virama_ = true;

            return true;
        }

        // --------------------------------------------------------
        // Remove ZWNJ
        // --------------------------------------------------------

        const std::string zwnjString(zwnj);

        if (
            text_.size() >= zwnjString.size() &&
            text_.compare(
                text_.size() - zwnjString.size(),
                zwnjString.size(),
                zwnjString) == 0)
        {
            text_.erase(
                text_.size() - zwnjString.size());

            consonant_pending_ = false;
            virama_pending_ = true;
            explicit_virama_ = true;

            return true;
        }

        // --------------------------------------------------------
        // Remove last UTF-8 code point
        // --------------------------------------------------------

        remove_last_utf8(text_);

        consonant_pending_ = false;

        const std::string viramaString(virama);

        virama_pending_ =
            text_.size() >= viramaString.size() &&
            text_.compare(
                text_.size() - viramaString.size(),
                viramaString.size(),
                viramaString) == 0;
        explicit_virama_ = virama_pending_;

        return true;
    }

    // ============================================================
    // Feed key
    // ============================================================

    std::string Composer::feed(
        char key,
        bool shifted,
        bool caps_lock,
        bool scroll_lock,
        bool num_lock,
        bool alt)
    {
        // ========================================================
        // Scroll Lock + number
        // ========================================================

        if (
            scroll_lock &&
            std::isdigit(
                static_cast<unsigned char>(key)))
        {
            reset();

            return std::string(1, key);
        }

        // ========================================================
        // Num Lock + number
        // ========================================================

        if (
            num_lock &&
            std::isdigit(
                static_cast<unsigned char>(key)))
        {
            reset();

            const char *digit = kannada_digit(key);

            if (digit == nullptr)
            {
                return std::string(1, key);
            }

            return digit;
        }

        // ========================================================
        // Num Lock special characters
        // ========================================================

        if (num_lock && key == '`')
        {
            reset();

            return shifted ? "~" : "ʻ";
        }

        if (num_lock && key == '\'')
        {
            reset();

            return "’";
        }

        if (num_lock && key == ',')
        {
            reset();

            return ",";
        }

        // ========================================================
        // Standalone special signs
        //
        // Shift+h -> ಃ
        // Shift+m -> ಂ
        // Shift+x -> ಼
        //
        // Only when we are NOT composing a consonant.
        // ========================================================

        if (
            shifted &&
            !consonant_pending_ &&
            !virama_pending_)
        {
            if (key == 'h')
            {
                return "ಃ";
            }

            if (key == 'm')
            {
                return "ಂ";
            }

            if (key == 'x')
            {
                return "಼";
            }

            if (key == '[' || key == ']')
            {
                return "ೢ";
            }
        }

        // ========================================================
        // Caps Lock layer
        // ========================================================

        if (caps_lock)
        {

            // Alt + Shift + A
            if (
                alt &&
                shifted &&
                key == 'a')
            {
                const std::string result = text_ + "ೱ";

                reset();

                return result;
            }

            // Alt + Shift + Z
            if (
                alt &&
                shifted &&
                key == 'z')
            {
                const std::string result = text_ + "ೲ";

                reset();

                return result;
            }

            const std::string symbol =
                caps_symbol(key, shifted);

            const std::string result =
                text_ + symbol;

            reset();

            return result;
        }

        // ========================================================
        // Standalone Kannada digits
        //
        // Normal 0-9 -> ೦-೯
        // Only when not composing.
        // ========================================================

        if (
            !shifted &&
            !consonant_pending_ &&
            !virama_pending_)
        {
            const char *digit = kannada_digit(key);

            if (digit != nullptr)
            {
                return digit;
            }
        }

        // ========================================================
        // Consonant lookup
        // ========================================================

        const char *consonant =
            shifted
                ? lookup(
                      shifted_consonants,
                      std::size(shifted_consonants),
                      key)
                : lookup(
                      consonants,
                      std::size(consonants),
                      key);

        if (consonant != nullptr)
        {

            // ----------------------------------------------------
            // Existing virama means an ottakshara/conjunct.
            //
            // Example:
            //
            // k + f + k
            //
            // previous:
            // ಕ್
            //
            // new consonant:
            // ಕ
            //
            // resulting sequence:
            // ಕ್ + ZWJ + ಕ
            //
            // ----------------------------------------------------

            if (virama_pending_)
            {
                const bool was_explicit_virama = explicit_virama_;
                text_ += consonant;
                consonant_pending_ = true;
                virama_pending_ = false;
                explicit_virama_ = false;
                if (!was_explicit_virama && default_vowel_ == DefaultVowel::Virama)
                {
                    text_ += virama;
                }
                return {};
            }

            if (default_vowel_ == DefaultVowel::Virama)
            {
                if (text_.size() >= std::string(virama).size() &&
                    text_.compare(text_.size() - std::string(virama).size(),
                                  std::string(virama).size(), virama) == 0)
                {
                    text_.erase(text_.size() - std::string(virama).size());
                }
                text_ += consonant;
                text_ += virama;
                consonant_pending_ = true;
                virama_pending_ = false;
                return {};
            }

            text_ += consonant;

            consonant_pending_ = true;
            virama_pending_ = false;

            return {};
        }

        // ========================================================
        // VIRAMA
        //
        // k + f -> ಕ್
        // ========================================================

        if (
            key == 'f' &&
            consonant_pending_)
        {
            if (shifted)
            {
                text_ += zwj_virama;

                consonant_pending_ = false;
                virama_pending_ = true;
                explicit_virama_ = true;

                return {};
            }

            if (default_vowel_ == DefaultVowel::Virama &&
                !virama_pending_ &&
                text_.size() >= std::string(virama).size() &&
                text_.compare(text_.size() - std::string(virama).size(),
                              std::string(virama).size(), virama) == 0)
            {
                virama_pending_ = true;
                explicit_virama_ = true;
                return {};
            }

            text_ += virama;

            consonant_pending_ = false;
            virama_pending_ = true;
            explicit_virama_ = true;

            return {};
        }

        if (default_vowel_ == DefaultVowel::Virama && key == 'f')
        {
            text_ += virama;
            virama_pending_ = true;
            explicit_virama_ = true;
            return {};
        }

        // ========================================================
        // Repeated VIRAMA
        //
        // k + f + f
        //
        // terminate using ZWNJ
        // ========================================================

        if (
            key == 'f' &&
            virama_pending_)
        {
            text_ += zwnj;

            reset();

            return {};
        }

        // ========================================================
        // Consonant + vowel/sign/special
        // ========================================================

        if (consonant_pending_)
        {

            // ----------------------------------------------------
            // Shift + h -> visarga
            //
            // ಕ + Shift+h -> ಕಃ
            // ----------------------------------------------------

            if (
                shifted &&
                key == 'h')
            {
                text_ += "ಃ";

                reset();

                return {};
            }

            // ----------------------------------------------------
            // Shift + m -> anusvara
            //
            // ಕ + Shift+m -> ಕಂ
            // ----------------------------------------------------

            if (
                shifted &&
                key == 'm')
            {
                text_ += "ಂ";

                consonant_pending_ = false;
                virama_pending_ = false;
                explicit_virama_ = false;

                return {};
            }

            // ----------------------------------------------------
            // Shift + x -> nukta
            //
            // ಕ + Shift+x -> ಕ಼
            // ----------------------------------------------------

            if (
                shifted &&
                key == 'x')
            {
                text_ += "಼";

                reset();

                return {};
            }

            // ----------------------------------------------------
            // Vowel sign
            // ----------------------------------------------------

            const char *sign =
                shifted
                    ? lookup(
                          shifted_vowel_signs,
                          std::size(shifted_vowel_signs),
                          key)
                    : lookup(
                          vowel_signs,
                          std::size(vowel_signs),
                          key);

            if (sign != nullptr)
            {
                if (default_vowel_ == DefaultVowel::Virama && key == 'a')
                {
                    text_.erase(text_.size() - std::string(virama).size());
                    consonant_pending_ = false;
                    virama_pending_ = false;
                    explicit_virama_ = false;
                    return {};
                }
                if (default_vowel_ == DefaultVowel::Virama &&
                    text_.size() >= std::string(virama).size() &&
                    text_.compare(text_.size() - std::string(virama).size(),
                                  std::string(virama).size(), virama) == 0)
                {
                    text_.erase(text_.size() - std::string(virama).size());
                }
                text_ += sign;

                consonant_pending_ = false;
                virama_pending_ = false;
                explicit_virama_ = false;

                return {};
            }

            if (default_vowel_ == DefaultVowel::Virama &&
                key == 'a' &&
                text_.size() >= std::string(virama).size() &&
                text_.compare(text_.size() - std::string(virama).size(),
                              std::string(virama).size(), virama) == 0)
            {
                text_.erase(text_.size() - std::string(virama).size());
                consonant_pending_ = false;
                virama_pending_ = false;
                explicit_virama_ = false;
                return {};
            }
        }

        // ========================================================
        // Independent vowel
        // ========================================================

        const char *vowel =
            shifted
                ? lookup(
                      shifted_independent_vowels,
                      std::size(shifted_independent_vowels),
                      key)
                : lookup(
                      independent_vowels,
                      std::size(independent_vowels),
                      key);

        // ========================================================
        // Unknown key / normal character
        // ========================================================

        std::string result =
            text_ +
            (vowel != nullptr
                 ? vowel
                 : std::string(1, key));

        reset();

        return result;
    }

} // namespace nudi