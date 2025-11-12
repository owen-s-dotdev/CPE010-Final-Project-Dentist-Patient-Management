#pragma once
#include <string>

class Patient {
public:
    std::string name;
    int age = 0;
    std::string contact;
    std::string preferred_time;

    Patient() = default;
    Patient(std::string n, int a, std::string c): name(std::move(n)), age(a), contact(std::move(c)) {}
};
