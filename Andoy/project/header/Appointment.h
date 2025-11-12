#pragma once
#include "Patient.h"
#include "Doctor.h"
#include <string>

class Appointment {
public:
    Patient patient;
    Doctor doctor;
    std::string time;
};

