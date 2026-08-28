#include "nudi_composer.hpp"

#include <cassert>

int main() {
    nudi::Composer composer;
    composer.feed('k');
    composer.feed('a');
    assert(composer.preedit() == "ಕಾ");

    composer.reset();
    composer.feed('q', true);
    assert(composer.preedit() == "ಠ");

    composer.reset();
    assert(composer.feed('4', false, true) == "₹");
    assert(composer.feed('a', false, true) == "a");
    assert(composer.feed('4') == "೪");
    assert(composer.feed('4', false, false, true) == "4");

    composer.reset();
    composer.feed('k');
    composer.feed('f');
    composer.feed('t');
    assert(composer.preedit() == "ಕ್\xE2\x80\x8D್" "ತ");

    composer.reset();
    assert(composer.feed('a') == "ಅ");
    composer.feed('k');
    assert(composer.backspace());
    assert(composer.empty());
    return 0;
}