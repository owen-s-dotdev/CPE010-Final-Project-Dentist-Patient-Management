#pragma once
#include "Appointment.h"
#include <queue>
#include <vector>

class Scheduler {
public:
    Scheduler();
    void run();

private:
    std::vector<Doctor> doctors_;
    std::queue<Appointment> appointments_;

    void initSampleDoctors();
    Patient collectPatientInfo();
    bool confirm(const std::string &msg);
    Doctor selectDoctor();
    void saveAppointment(const Appointment &a);
    void displayDoctors();
};
