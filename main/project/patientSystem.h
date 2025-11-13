#ifndef PATIENTSYSTEM_H
#define PATIENTSYSTEM_H

#include "Patient.h"
#include <vector>
#include <iostream>
#include <limits>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <sstream>
#include <cctype>

class PatientSystem {
private:
    std::vector<Patient> patientList;
    std::vector<Appointment> appointmentQueue;

    void EditPatientRecord(int idToEdit);
    void ScheduleAppointment();
    void AddNewPatientFunction();
    void PatientListFunction();
    void PatientAppointmentFunction();

public:
    PatientSystem() = default;
    void DisplayMainMenu();
};

static inline int TimeStringToMinutes(const std::string &raw) {
    std::string s = raw;
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
    if (s.empty()) return -1;

    std::string upper = s;
    for (auto &c : upper) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    bool hasAM = (upper.find("AM") != std::string::npos);
    bool hasPM = (upper.find("PM") != std::string::npos);

    if (hasAM || hasPM) {
        std::string tmp;
        for (char c : s) if (!std::isalpha(static_cast<unsigned char>(c))) tmp.push_back(c);
        s = tmp;
    }

    int hour = 0, minute = 0;
    size_t pos = s.find(':');
    if (pos == std::string::npos) pos = s.find('.');
    if (pos == std::string::npos) {
        try { hour = std::stoi(s); minute = 0; } catch (...) { return -1; }
    } else {
        std::string h = s.substr(0, pos);
        std::string m = s.substr(pos+1);
        try { hour = std::stoi(h); minute = std::stoi(m); } catch (...) { return -1; }
    }

    if (hour < 0 || minute < 0 || minute >= 60) return -1;

    if (hasAM || hasPM) {
        if (hour == 12) {
            if (hasAM) hour = 0;
        } else {
            if (hasPM) hour = (hour % 12) + 12;
        }
    }

    if (hour < 0 || hour >= 24) return -1;
    return hour * 60 + minute;
}


static inline int GenerateUnique4DigitId(const std::vector<Patient> &existing) {
    std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1000, 9999);

    std::unordered_map<int, bool> used;
    used.reserve(existing.size() * 2 + 10);
    for (const auto &p : existing) used[p.id] = true;

    for (int attempt = 0; attempt < 10000; ++attempt) {
        int cand = dist(gen);
        if (used.find(cand) == used.end()) return cand;
    }

    for (int cand = 1000; cand <= 9999; ++cand) {
        if (used.find(cand) == used.end()) return cand;
    }

    return -1;
}

inline void PatientSystem::EditPatientRecord(int idToEdit) {
    auto it = std::find_if(patientList.begin(), patientList.end(),
                           [idToEdit](const Patient& p) { return p.id == idToEdit; });

    if (it != patientList.end()) {
        std::cout << "\n--- Editing Patient ID: " << it->id << " ---\n";
        std::cout << "Current Name: " << it->name << "\n";
        std::cout << "Current Age: " << it->age << "\n";

        std::cout << "Enter new name (leave blank to keep current): ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string temp;
        std::getline(std::cin, temp);
        if (!temp.empty()) it->name = temp;

        std::cout << "Enter new age (enter 0 to keep current): ";
        int newAge = 0;
        if ((std::cin >> newAge)) {
            if (newAge > 0) it->age = newAge;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Enter new contact number (leave blank to keep current): ";
        std::string newContact;
        std::getline(std::cin, newContact);
        if (!newContact.empty()) it->contactNumber = newContact;

        std::cout << "Enter new gender (M/F/O) (leave blank to keep current): ";
        std::string newGender;
        std::getline(std::cin, newGender);
        if (!newGender.empty()) it->gender = newGender;

        std::cout << "Enter new medical history summary (leave blank to keep current): ";
        std::string newHistory;
        std::getline(std::cin, newHistory);
        if (!newHistory.empty()) it->history = newHistory;

        std::cout << "\nPatient record updated successfully!\n";
    } else {
        std::cout << "\nPatient ID " << idToEdit << " not found.\n";
    }
}

inline void PatientSystem::ScheduleAppointment() {
    std::cout << "\n--- Schedule New Appointment ---\n";
    if (patientList.empty()) {
        std::cout << "No patients available to schedule an appointment.\n";
        return;
    }

    std::cout << "Available Patients:\n";
    for (const auto& p : patientList) {
        std::cout << "ID: " << p.id << ", Name: " << p.name << ", Age: " << p.age << "\n";
    }

    int selectedId = -1;
    std::cout << "Enter Patient ID for appointment (or enter 0 to select the most recently added patient): ";
    std::string selStr;
    if (!(std::cin >> selStr)) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input.\n";
        return;
    }
    try { selectedId = std::stoi(selStr); } catch (...) { std::cout << "Invalid ID format.\n"; return; }
    if (selectedId == 0) {
        if (patientList.empty()) { std::cout << "No patients in the system to select.\n"; return; }
        selectedId = patientList.back().id;
        std::cout << "Auto-selected Patient ID: " << selectedId << "\n";
    }

    auto it = std::find_if(patientList.begin(), patientList.end(),
                           [selectedId](const Patient& p) { return p.id == selectedId; });
    if (it == patientList.end()) { std::cout << "Invalid Patient ID. Returning to appointment menu.\n"; return; }

    Appointment newApp; newApp.patientId = selectedId;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (true) {
        std::cout << "Enter preferred appointment time (e.g., 10:00 AM) or C to cancel: ";
        std::getline(std::cin, newApp.time);
        if (!newApp.time.empty() && (std::toupper(static_cast<unsigned char>(newApp.time[0])) == 'C')) {
            std::cout << "Appointment scheduling cancelled.\n"; return;
        }
        int minutes = TimeStringToMinutes(newApp.time);
        if (minutes < 0) { std::cout << "Invalid time format. Please use formats like 10:30 AM or 14:30." << "\n"; continue; }
        break;
    }

    // Fixed schedules per doctor (ranges are inclusive: [start, end])
    static const std::unordered_map<std::string, std::vector<std::pair<int,int>>> doctorSchedules = {
        {"Dr. Owen",     {{9*60, 12*60-1}, {13*60, 17*60-1}}},
        {"Dr. Escalona", {{8*60, 12*60-1}, {13*60, 16*60-1}}},
        {"Dr. Crishen",  {{10*60, 14*60-1}}},
        {"Dr. Pizzaro",  {{9*60, 11*60-1}, {14*60, 18*60-1}}},
        {"Dr. Punay",    {{8*60, 12*60-1}, {12*60+30, 15*60-1}}}
    };

    auto isWorking = [&](const std::string &doc, int minute) -> bool {
        auto itSched = doctorSchedules.find(doc);
        if (itSched == doctorSchedules.end()) return false;
        for (const auto &r : itSched->second) if (minute >= r.first && minute <= r.second) return true;
        return false;
    };

    int requestedMinutes = TimeStringToMinutes(newApp.time);
    std::vector<std::string> availableDoctors;
    for (const auto &entry : doctorSchedules) {
        const std::string doc = entry.first;
        if (!isWorking(doc, requestedMinutes)) continue;
        bool occupied = false;
        for (const auto &app : appointmentQueue) {
            if (app.doctor == doc) {
                int appMin = TimeStringToMinutes(app.time);
                if (appMin >= 0 && appMin == requestedMinutes) { occupied = true; break; }
            }
        }
        if (!occupied) availableDoctors.push_back(doc);
    }

    if (availableDoctors.empty()) { std::cout << "There are no doctors available at that time. Returning to appointment menu.\n"; return; }

    std::cout << "\nDoctors available at " << newApp.time << ":\n";
    for (size_t i = 0; i < availableDoctors.size(); ++i) std::cout << (i+1) << ". " << availableDoctors[i] << "\n";

    int docChoice = -1;
    while (true) {
        std::cout << "Enter doctor number to choose, or 0 to auto-assign the first available: ";
        if (!(std::cin >> docChoice)) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cout << "Invalid input. Please enter a number.\n"; continue; }
        if (docChoice == 0) { newApp.doctor = availableDoctors[0]; break; }
        if (docChoice >= 1 && static_cast<size_t>(docChoice) <= availableDoctors.size()) { newApp.doctor = availableDoctors[docChoice-1]; break; }
        std::cout << "Invalid selection. Try again.\n";
    }

    std::cout << "\n--- Confirmation ---\n";
    std::cout << "Patient: " << it->name << " (ID: " << it->id << ")\n";
    std::cout << "Time: " << newApp.time << "\n";
    std::cout << "Doctor: " << newApp.doctor << "\n";

    char saveChoice;
    std::cout << "Save this appointment? (Y/N): ";
    std::cin >> saveChoice;
    if (std::tolower(static_cast<unsigned char>(saveChoice)) == 'y') {
        appointmentQueue.push_back(newApp);
        std::sort(appointmentQueue.begin(), appointmentQueue.end(), [](const Appointment &a, const Appointment &b) {
            return TimeStringToMinutes(a.time) < TimeStringToMinutes(b.time);
        });
        std::cout << "Appointment saved successfully!\n";
    } else { std::cout << "Appointment cancelled.\n"; return; }

    char addAnother;
    std::cout << "Do you want to schedule another appointment? (Y/N): ";
    std::cin >> addAnother;
    if (std::tolower(static_cast<unsigned char>(addAnother)) == 'y') ScheduleAppointment();
}

inline void PatientSystem::AddNewPatientFunction() {
    std::cout << "\n\n--- Add New Patient ---\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    while (true) {
        Patient newPatient;
        std::string line;

        std::cout << "Enter Patient Name: ";
        std::getline(std::cin, newPatient.name);

        while (true) {
            std::cout << "Enter Age: ";
            std::getline(std::cin, line);
            if (line.empty()) { std::cout << "Age cannot be empty. Please enter a valid age.\n"; continue; }
            std::istringstream iss(line);
            if (iss >> newPatient.age && newPatient.age > 0) break;
            std::cout << "Invalid age input. Please enter a positive number.\n";
        }

        std::cout << "Enter Contact Number: ";
        std::getline(std::cin, newPatient.contactNumber);

        std::cout << "Enter Gender (M/F/O): ";
        std::getline(std::cin, newPatient.gender);
        std::string hxChoice;
        while (true) { std::cout << "Does the patient have a medical history to record? (Y/N): "; std::getline(std::cin, hxChoice); if (!hxChoice.empty()) break; std::cout << "Please enter Y or N.\n"; }
        if (std::tolower(static_cast<unsigned char>(hxChoice[0])) == 'y') {
            std::cout << "Enter Medical History Summary: "; std::getline(std::cin, newPatient.history);
            if (newPatient.history.empty()) newPatient.history = "(history provided but left blank)";
        } else {
            newPatient.history = "No known medical history";
        }

        if (!newPatient.name.empty() && newPatient.age > 0) {
            int generatedId = GenerateUnique4DigitId(patientList);
            if (generatedId < 0) { std::cout << "Failed to generate a unique patient ID. Please try again later.\n"; continue; }
            newPatient.id = generatedId;
            patientList.push_back(newPatient);
            std::cout << "\nPatient added to the list successfully (ID: " << newPatient.id << ").\n";
        } else { std::cout << "Patient name or age is invalid. Please try again.\n"; continue; }

        std::cout << "Do you want to add another patient? (Y/N): ";
        std::getline(std::cin, line);
        if (line.empty() || std::tolower(static_cast<unsigned char>(line[0])) != 'y') break;
    }
}

inline void PatientSystem::PatientListFunction() {
    while (true) {
        std::cout << "\n\n--- Patient List ---\n";
        if (patientList.empty()) { std::cout << "No patients recorded.\n"; }
        else {
            for (const auto& p : patientList) {
                std::cout << "ID: " << p.id << ", Name: " << p.name
                          << ", Age: " << p.age
                          << ", Contact: " << p.contactNumber
                          << ", Gender: " << p.gender
                          << ", History: " << p.history << "\n";
            }
        }

        std::cout << "\nOptions:\n";
        std::cout << "1. Edit patient record\n";
        std::cout << "2. Delete a patient record\n";
        std::cout << "3. Return to main menu\n";
        std::cout << "Select an option: ";

        int choice;
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cout << "Invalid input. Please enter a number.\n"; continue; }

        if (choice == 1) { int idToEdit; std::cout << "Enter Patient ID to Edit: "; std::cin >> idToEdit; EditPatientRecord(idToEdit); }
        else if (choice == 2) {
            int idToDelete; std::cout << "Enter Patient ID to Delete: "; std::cin >> idToDelete;
            size_t initialSize = patientList.size();
            patientList.erase(std::remove_if(patientList.begin(), patientList.end(), [idToDelete](const Patient& p){ return p.id == idToDelete; }), patientList.end());
            if (patientList.size() < initialSize) {
                appointmentQueue.erase(std::remove_if(appointmentQueue.begin(), appointmentQueue.end(), [idToDelete](const Appointment& app){ return app.patientId == idToDelete; }), appointmentQueue.end());
                std::cout << "Patient ID " << idToDelete << " deleted successfully." << "\n";
            } else { std::cout << "Patient ID " << idToDelete << " not found.\n"; }
        } else if (choice == 3) { break; }
        else { std::cout << "Invalid choice. Please try again.\n"; }
    }
}

inline void PatientSystem::PatientAppointmentFunction() {
    while (true) {
        std::cout << "\n\n--- Patient Appointment Menu ---\n";
        std::cout << "1. Display schedule\n";
        std::cout << "2. Appoint a schedule for patient\n";
        std::cout << "3. Return to main menu\n";
        std::cout << "Select an option: ";

        int choice;
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cout << "Invalid input. Please enter a number.\n"; continue; }

        if (choice == 1) {
            std::cout << "\n--- List of Scheduled Patients ---\n";
            if (appointmentQueue.empty()) std::cout << "No appointments scheduled.\n";
            else {
                for (const auto& app : appointmentQueue) {
                    auto it = std::find_if(patientList.begin(), patientList.end(), [app](const Patient& p){ return p.id == app.patientId; });
                    std::string patientName = (it != patientList.end()) ? it->name : "Unknown";
                    std::cout << "Patient: " << patientName << ", Time: " << app.time << ", Doctor: " << app.doctor << "\n";
                }
            }
            char scheduleAgain; std::cout << "Do you want to schedule a patient now? (Y/N): "; std::cin >> scheduleAgain;
            if (std::tolower(static_cast<unsigned char>(scheduleAgain)) == 'y') ScheduleAppointment(); else break;

        } else if (choice == 2) ScheduleAppointment();
        else if (choice == 3) break;
        else std::cout << "Invalid choice. Please try again.\n";
    }
}

inline void PatientSystem::DisplayMainMenu() {
    int choice;
    do {
        std::cout << "\n\n==========================\n";
        std::cout << "  Patient Management System\n";
        std::cout << "==========================\n";
        std::cout << "1. Add New Patient Info\n";
        std::cout << "2. Patient List (Edit/Delete)\n";
        std::cout << "3. Patient Appointment Schedule\n";
        std::cout << "4. Exit\n";
        std::cout << "Select an option: ";

        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cout << "Invalid input. Please enter a number.\n"; continue; }

        switch (choice) {
            case 1: AddNewPatientFunction(); break;
            case 2: PatientListFunction(); break;
            case 3: PatientAppointmentFunction(); break;
            case 4: std::cout << "Exiting system. Goodbye!\n"; break;
            default: std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);
}

#endif
