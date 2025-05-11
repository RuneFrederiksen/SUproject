#pragma once
#include <iostream>
#include <limits>

inline int getChoice(int min, int max) {
    int choice;
    while (true) {
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ugyldigt input. Indtast et tal mellem "
                      << min << " og " << max << ": ";
            continue;
        }
        if (choice < min || choice > max) {
            std::cout << "Ugyldigt valg. Prøv igen (" << min
                      << " - " << max << "): ";
            continue;
        }
        break;
    }
    return choice;
}
