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
            false,
            false,
            true,
            "ೱ"
        },

        {
            "CAPS-002",
            "Caps + Alt + Shift + Z",
            'z',
            true,
            true,
            false,
            false,
            true,
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