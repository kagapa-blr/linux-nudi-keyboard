#pragma once

#include <string>
#include <vector>

namespace nudi::tests
{

// ============================================================
// Basic reference test
//
// Example:
//     "krfnAqk" -> "ಕರ್ನಾಟಕ"
// ============================================================

struct ReferenceTestCase
{
    std::string id;
    std::string name;
    std::string input;
    std::string expected;
};


// ============================================================
// Single key test
//
// Used for Shift, Num Lock, Caps Lock, Alt, etc.
// ============================================================

struct KeyTestCase
{
    std::string id;
    std::string name;

    char key;

    bool shift = false;
    bool alt = false;
    bool ctrl = false;
    bool scroll_lock = false;
    bool num_lock = false;

    std::string expected;
};


// ============================================================
// Composition test
//
// Used when multiple explicit composer operations are needed.
// ============================================================

struct CompositionTestCase
{
    std::string id;
    std::string name;
    std::string input;
    std::string expected;
};


// ============================================================
// Test case collections
// ============================================================

const std::vector<ReferenceTestCase> &reference_test_cases();

const std::vector<KeyTestCase> &key_test_cases();

const std::vector<CompositionTestCase> &composition_test_cases();

} // namespace nudi::tests