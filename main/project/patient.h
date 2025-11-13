#ifndef PATIENT_H
#define PATIENT_H

#include <string>

class Patient {
public:
    int id;
    std::string name;
    int age;
    std::string contactNumber;
    std::string gender;
    std::string history;
};

class Appointment {
public:
    int patientId;
    std::string time;
    std::string doctor;
};

#endif 
