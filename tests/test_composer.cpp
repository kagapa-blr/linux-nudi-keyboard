#include "nudi_composer.hpp"

#include <cassert>
#include <string>

int main() {
    nudi::Composer composer;
    const std::string consonant_keys = "rsdwqtypghjlkzxcvbnm";
    const std::string consonants = "ರಸಡತಟಯಪಗಹಜಲಕಞಷಚವಬನಮ";
    for (std::size_t index = 0; index < consonant_keys.size(); ++index) {
        composer.reset();
        composer.feed(consonant_keys[index]);
        assert(composer.preedit() == consonants.substr(index * 3, 3));
    }

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
    assert(composer.feed('4', false, false, false, true) == "೪");
    composer.feed('k');
    assert(composer.feed('4', false, false, true) == "4");
    assert(composer.empty());
    assert(composer.feed('r', true) == "ಋ");
    assert(composer.feed('y', true) == "ಐ");
    assert(composer.feed('x', true) == "಼");
    assert(composer.feed('[', true) == "ೢ");
    assert(composer.feed('4', false, false, false, true) == "೪");
    assert(composer.feed('`', false, false, false, true) == "ʻ");
    assert(composer.feed('`', true, false, false, true) == "~");
    assert(composer.feed('\'', false, false, false, true) == "’");
    assert(composer.feed(',', false, false, false, true) == ",");
    assert(composer.feed('a', true, true, false, false, true) == "ೱ");
    assert(composer.feed('z', true, true, false, false, true) == "ೲ");

    composer.reset();
    composer.feed('k');
    composer.feed('f');
    composer.feed('t');
    assert(composer.preedit() == "ಕ್\xE2\x80\x8D್" "ತ");

    composer.reset();
    composer.feed('k');
    composer.feed('f');
    assert(composer.separator() == "ಕ್\xE2\x80\x8C");
    composer.feed('k');
    composer.feed('f');
    composer.feed('f');
    assert(composer.preedit() == "ಕ್\xE2\x80\x8C");

    composer.reset();
    assert(composer.feed('h', true) == "ಃ");
    assert(composer.feed('m', true) == "ಂ");
    composer.reset();
    composer.feed('k');
    composer.feed('h', true);
    assert(composer.preedit() == "ಕಃ");
    composer.reset();
    composer.feed('k');
    composer.feed('m', true);
    assert(composer.preedit() == "ಕಂ");
    composer.reset();
    composer.feed('k');
    composer.feed('x', true);
    assert(composer.preedit() == "ಕ಼");

    composer.reset();
    assert(composer.feed('a') == "ಅ");
    composer.feed('k');
    assert(composer.backspace());
    assert(composer.empty());
    return 0;
}