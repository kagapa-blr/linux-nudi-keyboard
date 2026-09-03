#include "nudi_test_cases.hpp"

namespace nudi::tests
{

// ============================================================
// Reference / word tests
// ============================================================

const std::vector<ReferenceTestCase> &reference_test_cases()
{
    static const std::vector<ReferenceTestCase> tests =
    {
        {
            "WORD-001",
            "Karnataka",
            "krfnAqk",
            "ಕರ್ನಾಟಕ"
        },

        {
            "WORD-002",
            "kanaka",
            "knk",
            "ಕನಕ"
        },

        {
            "WORD-003",
            "rF conjunct",
            "rFk",
            "ರ‍್ಕ"
        },

        {
            "WORD-004",
            "ordinary r conjunct",
            "rfk",
            "ರ್ಕ"
        },

        {
            "WORD-005",
            "rF conjunct after vowel",
            "arFk",
            "ಅರ‍್ಕ"
        },

        {
            "WORD-006",
            "ordinary r conjunct after vowel",
            "arfk",
            "ಅರ್ಕ"
        },

        {
            "WORD-007",
            "mantri",
            "mMtfri",
            "ಮಂತ್ರಿ"
        },

  

    };

    return tests;
}


// ============================================================
// Single key tests
// ============================================================

const std::vector<KeyTestCase> &key_test_cases()
{
    static const std::vector<KeyTestCase> tests =
    {
        // ----------------------------------------------------
        // Shift mappings
        // ----------------------------------------------------

        // {
        //     "KEY-001",
        //     "Shift consonant",
        //     'q',
        //     true,
        //     false,
        //     false,
        //     false,
        //     false,
        //     "ಠ"
        // },

 
        // ----------------------------------------------------
        // Caps + Alt + Shift
        // ----------------------------------------------------

        {
            "CAPS-001",
            "Caps + Alt + Shift + A",
            'a',
            true,
            true,
            true,
            false,
            false,
            false,
            "ೱ"
        },

        {
            "CAPS-002",
            "Caps + Alt + Shift + Z",
            'z',
            true,
            true,
            true,
            false,
            false,
            false,
            "ೲ"
        }
    };

    return tests;
}


// ============================================================
// Composition tests
// ============================================================

const std::vector<CompositionTestCase> &composition_test_cases()
{
    static const std::vector<CompositionTestCase> tests =
    {
        {
            "COMP-001",
            "Consonant + vowel",
            "ka",
            "ಕಾ"
        },


        {
            "COMP-005",
            "Repeated virama",
            "kf",
            "ಕ್"
        }
    };

    return tests;
}

} // namespace nudi::tests