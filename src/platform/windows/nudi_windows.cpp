#include "nudi_composer.hpp"

#include <iostream>
#include <string>

int main()
{
    nudi::Composer composer;

    std::cout << "Nudi Windows console runner\n";
    std::cout << "Press keys to test the shared composer logic. Press q to quit.\n";

    while (true)
    {
        char key = '\0';
        std::cout << "> ";

        if (!(std::cin >> key))
        {
            break;
        }

        if (key == 'q' || key == 'Q')
        {
            break;
        }

        const std::string committed = composer.feed(key);

        std::cout << "committed: " << committed << '\n';
        std::cout << "preedit:   " << composer.preedit() << '\n';
    }

    return 0;
}
