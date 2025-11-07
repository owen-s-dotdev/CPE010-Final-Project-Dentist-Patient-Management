#ifndef PATIENT_H
#define PATIENT_H

#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>

class Patient {
private:
    int patient_num;
    std::string name;
    std::string condition;
    std::chrono::system_clock::time_point appointment_time;

public:
    Patient() : patient_num(0), name(""), condition(""),
        appointment_time(std::chrono::system_clock::now()) {}

    Patient(int pNum, std::string pName, std::string pCondition,
            std::chrono::system_clock::time_point apptTime)
        : patient_num(pNum), name(pName), condition(pCondition),
          appointment_time(apptTime) {}

    std::chrono::system_clock::time_point getAppointmentTime() const {
        return appointment_time;
    }

    void printPatient() const {
        std::time_t appt = std::chrono::system_clock::to_time_t(appointment_time);
        std::cout << "Patient # " << patient_num << '\n'
                  << "Name: " << name << '\n'
                  << "Condition: " << condition << '\n'
                  << "Appointment Time: " << std::ctime(&appt);
    }

    ~Patient() {
        std::cout << "Cleared Patient List\n";
    }
};

// Sorting function declared in the header
inline void sortPatientsByAppointment(std::vector<Patient>& patients) {
    std::sort(patients.begin(), patients.end(), [](const Patient& a, const Patient& b) {
        return a.getAppointmentTime() < b.getAppointmentTime();
    });
}

#endif