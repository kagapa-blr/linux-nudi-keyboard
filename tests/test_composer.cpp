#include "nudi_composer.hpp"

#include <cassert>
#include <iostream>
#include <string>

namespace
{

    // ============================================================
    // Print test result
    // ============================================================

    bool check(
        const std::string &name,
        const std::string &input,
        const std::string &expected,
        const std::string &actual)
    {
        std::cout
            << "\n========================================\n"
            << "Test:     " << name << '\n'
            << "Input:    " << input << '\n'
            << "Expected: " << expected << '\n'
            << "Actual:   " << actual << '\n';

        if (expected == actual)
        {
            std::cout
                << "Result:   PASS\n"
                << "========================================\n";

            return true;
        }

        std::cout
            << "Result:   FAIL\n"
            << "========================================\n";

        return false;
        
    }

    // ============================================================
    // Stop test on failure
    // ============================================================

    bool require(
        bool result)
    {
        return result;
    }

} // namespace

int main()
{
    nudi::Composer composer;

    
        // ============================================================
        // Helper: feed Nudi reference notation
        //
        // Uppercase ASCII character = Shift + key
        // ============================================================

        auto feed_reference =
            [&composer](const std::string &input)
    {
        for (char key : input)
        {
            const bool shifted =
                key >= 'A' &&
                key <= 'Z';

            composer.feed(
                shifted
                    ? static_cast<char>(
                          key - 'A' + 'a')
                    : key,
                shifted);
        }
    };

    // ============================================================
    // Basic consonant mappings
    // ============================================================

    const std::string consonant_keys =
        "rsdwqtypghjlkzxcvbnm";

    const std::string consonants =
        "ರಸದಡಟತಯಪಗಹಜಲಕಞಷಚವಬನಮ";

    assert(
        consonants.size() ==
        consonant_keys.size() * 3);

    for (
        std::size_t index = 0;
        index < consonant_keys.size();
        ++index)
    {
        composer.reset();

        composer.feed(
            consonant_keys[index]);

        const std::string expected =
            consonants.substr(
                index * 3,
                3);

        const std::string actual =
            composer.preedit();

        const std::string input(
            1,
            consonant_keys[index]);

        if (!require(
                check(
                    "Basic consonant",
                    input,
                    expected,
                    actual)))
        {
            return 1;
        }
    }

    // ============================================================
    // Basic consonant + vowel
    //
    // k + a = ಕಾ
    // ============================================================

    composer.reset();

    composer.feed('k');
    composer.feed('a');

    if (!require(
            check(
                "Consonant + vowel",
                "ka",
                "ಕಾ",
                composer.preedit())))
    {
        return 1;
    }

    // ============================================================
    // ಕರ್ನಾಟಕ
    //
    // k r f n A q k
    //
    // Result:
    // ಕರ್ನಾಟಕ
    //
    // NOTE:
    //
    // Do NOT use final A.
    //
    // krfnAqkA =
    // ಕರ್ನಾಟಕಾ
    // ============================================================

    composer.reset();

    feed_reference(
        "krfnAqk");

    if (!require(
            check(
                "Karnataka",
                "krfnAqk",
                "ಕರ್ನಾಟಕ",
                composer.preedit())))
    {
        return 1;
    }

    // ============================================================
    // Shift consonant
    //
    // Shift + q = ಠ
    // ============================================================

    composer.reset();

    composer.feed(
        'q',
        true);

    if (!require(
            check(
                "Shift consonant",
                "Q",
                "ಠ",
                composer.preedit())))
    {
        return 1;
    }

    // ============================================================
    // Default vowel = Virama
    // ============================================================

    composer.set_default_vowel(
        nudi::Composer::DefaultVowel::Virama);

    // ============================================================
    // ಕನಕ
    //
    // kanaka
    // ============================================================

    composer.reset();

    feed_reference(
        "kanaka");

    if (!require(
            check(
                "Kanaka",
                "kanaka",
                "ಕನಕ",
                composer.preedit())))
    {
        return 1;
    }





    // ============================================================
    // Restore normal default vowel mode
    // ============================================================

    composer.set_default_vowel(
        nudi::Composer::DefaultVowel::A);

    // ============================================================
    // Scroll Lock + number
    //
    // Should pass ASCII number through.
    // ============================================================

    composer.reset();

    composer.feed('k');

    const std::string scroll_result =
        composer.feed(
            '4',
            false,
            false,
            true);

    if (!require(
            check(
                "Scroll Lock number",
                "ScrollLock+4",
                "4",
                scroll_result)))
    {
        return 1;
    }

    assert(
        composer.empty());

    // ============================================================
    // Shift independent vowels
    // ============================================================

    if (!require(
            check(
                "Independent vowel",
                "Shift+r",
                "ಋ",
                composer.feed(
                    'r',
                    true))))
    {
        return 1;
    }

    if (!require(
            check(
                "Independent vowel",
                "Shift+y",
                "ಐ",
                composer.feed(
                    'y',
                    true))))
    {
        return 1;
    }

    // ============================================================
    // Shift special sign
    //
    // Shift+x = ಼
    // ============================================================

    if (!require(
            check(
                "Nukta",
                "Shift+x",
                "಼",
                composer.feed(
                    'x',
                    true))))
    {
        return 1;
    }

    // ============================================================
    // Shift+[ = ೢ
    // ============================================================

    if (!require(
            check(
                "Special bracket",
                "Shift+[",
                "ೢ",
                composer.feed(
                    '[',
                    true))))
    {
        return 1;
    }

    // ============================================================
    // Num Lock + number
    //
    // 4 = ೪
    // ============================================================

    if (!require(
            check(
                "Kannada digit",
                "NumLock+4",
                "೪",
                composer.feed(
                    '4',
                    false,
                    false,
                    false,
                    true))))
    {
        return 1;
    }

    // ============================================================
    // Num Lock special characters
    // ============================================================

    if (!require(
            check(
                "NumLock grave",
                "NumLock+`",
                "ʻ",
                composer.feed(
                    '`',
                    false,
                    false,
                    false,
                    true))))
    {
        return 1;
    }

    if (!require(
            check(
                "NumLock Shift grave",
                "NumLock+Shift+`",
                "~",
                composer.feed(
                    '`',
                    true,
                    false,
                    false,
                    true))))
    {
        return 1;
    }

    if (!require(
            check(
                "NumLock apostrophe",
                "NumLock+'",
                "’",
                composer.feed(
                    '\'',
                    false,
                    false,
                    false,
                    true))))
    {
        return 1;
    }

    if (!require(
            check(
                "NumLock comma",
                "NumLock+,",
                ",",
                composer.feed(
                    ',',
                    false,
                    false,
                    false,
                    true))))
    {
        return 1;
    }

    // ============================================================
    // Caps Lock + Alt + Shift
    // ============================================================

    if (!require(
            check(
                "Caps+Alt+Shift+A",
                "Caps+Alt+Shift+A",
                "ೱ",
                composer.feed(
                    'a',
                    true,
                    true,
                    false,
                    false,
                    true))))
    {
        return 1;
    }

    if (!require(
            check(
                "Caps+Alt+Shift+Z",
                "Caps+Alt+Shift+Z",
                "ೲ",
                composer.feed(
                    'z',
                    true,
                    true,
                    false,
                    false,
                    true))))
    {
        return 1;
    }



    // ============================================================
    // Separator after explicit virama
    //
    // k + f + separator
    // ============================================================

    composer.reset();

    composer.feed('k');
    composer.feed('f');

    if (!require(
            check(
                "Virama separator",
                "kf + separator",
                "ಕ್\xE2\x80\x8C",
                composer.separator())))
    {
        return 1;
    }

    // ============================================================
    // Repeated virama
    //
    // k + f + f
    // ============================================================

    composer.reset();

    composer.feed('k');
    composer.feed('f');
    composer.feed('f');

    if (!require(
            check(
                "Repeated virama",
                "kff",
                "ಕ್\xE2\x80\x8C",
                composer.preedit())))
    {
        return 1;
    }

    // ============================================================
    // Standalone special signs
    // ============================================================

    composer.reset();

    if (!require(
            check(
                "Visarga",
                "Shift+h",
                "ಃ",
                composer.feed(
                    'h',
                    true))))
    {
        return 1;
    }

    if (!require(
            check(
                "Anusvara",
                "Shift+m",
                "ಂ",
                composer.feed(
                    'm',
                    true))))
    {
        return 1;
    }

    // ============================================================
    // Consonant + visarga
    //
    // k + Shift+h = ಕಃ
    // ============================================================

    composer.reset();

    composer.feed('k');
    composer.feed('h', true);

    if (!require(
            check(
                "Consonant + visarga",
                "kH",
                "ಕಃ",
                composer.preedit())))
    {
        return 1;
    }

    // ============================================================
    // Consonant + anusvara
    //
    // m + Shift+m = ಮಂ
    //
    // IMPORTANT regression test
    // ============================================================

    composer.reset();

    composer.feed('m');
    composer.feed('m', true);

    if (!require(
            check(
                "Consonant + anusvara",
                "mM",
                "ಮಂ",
                composer.preedit())))
    {
        return 1;
    }

    // ============================================================
    // Consonant + nukta
    //
    // k + Shift+x = ಕ಼
    // ============================================================

    composer.reset();

    composer.feed('k');
    composer.feed('x', true);

    if (!require(
            check(
                "Consonant + nukta",
                "kX",
                "ಕ಼",
                composer.preedit())))
    {
        return 1;
    }

    // ============================================================
    // Independent vowel
    //
    // a = ಅ
    // ============================================================

    composer.reset();

    if (!require(
            check(
                "Independent vowel",
                "a",
                "ಅ",
                composer.feed('a'))))
    {
        return 1;
    }

    // ============================================================
    // Backspace
    // ============================================================

    composer.reset();

    composer.feed('k');

    const bool backspace_result =
        composer.backspace();

    const bool backspace_pass =
        backspace_result &&
        composer.empty();

    std::cout
        << "\n========================================\n"
        << "Test:     Backspace\n"
        << "Input:    k + Backspace\n"
        << "Expected: empty\n"
        << "Actual:   "
        << (composer.empty()
                ? "empty"
                : composer.preedit())
        << '\n'
        << "Result:   "
        << (backspace_pass
                ? "PASS"
                : "FAIL")
        << "\n========================================\n";

    if (!backspace_pass)
    {
        return 1;
    }

    // ============================================================
    // Final success
    // ============================================================

    std::cout
        << "\n========================================\n"
        << "ALL COMPOSER TESTS PASSED\n"
        << "========================================\n";

    return 0;
}
