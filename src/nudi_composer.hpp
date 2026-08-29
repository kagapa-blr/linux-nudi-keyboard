#pragma once

#include <string>

namespace nudi {

class Composer {
public:
    const std::string& preedit() const { return text_; }
    bool empty() const { return text_.empty(); }
    void reset();
    bool backspace();
    std::string separator();
    std::string feed(char key, bool shifted = false, bool caps_lock = false,
                     bool scroll_lock = false, bool num_lock = false, bool alt = false);

private:
    bool consonant_pending_ = false;
    bool virama_pending_ = false;
    bool quote_right_ = true;
    std::string text_;
};

}  // namespace nudi