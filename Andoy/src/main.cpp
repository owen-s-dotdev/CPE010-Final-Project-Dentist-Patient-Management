#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <limits>

// ============================================================================
// PATIENT CLASS
// ============================================================================
class Patient {
public:
    std::string name;
    int age = 0;
    std::string contact;
    std::string preferred_time;

    Patient() = default;
    Patient(std::string n, int a, std::string c): name(std::move(n)), age(a), contact(std::move(c)) {}
};

// ============================================================================
// DOCTOR CLASS
// ============================================================================
class Doctor {
public:
    int id = 0;
    std::string name;
    std::string specialty;
    std::string available_times;
};

// ============================================================================
// APPOINTMENT CLASS
// ============================================================================
class Appointment {
public:
    Patient patient;
    Doctor doctor;
    std::string time;
};

// ============================================================================
// SCHEDULER CLASS
// ============================================================================
class Scheduler {
private:
    std::vector<Doctor> doctors_;
    std::queue<Appointment> appointments_;

    void initSampleDoctors() {
        Doctor doc1;
        doc1.id = 1;
        doc1.name = "Dr. Smith";
        doc1.specialty = "General";
        doc1.available_times = "09:00-12:00";
        doctors_.push_back(doc1);

        Doctor doc2;
        doc2.id = 2;
        doc2.name = "Dr. Lee";
        doc2.specialty = "Pediatrics";
        doc2.available_times = "10:00-14:00";
        doctors_.push_back(doc2);

        Doctor doc3;
        doc3.id = 3;
        doc3.name = "Dr. Patel";
        doc3.specialty = "Dermatology";
        doc3.available_times = "13:00-17:00";
        doctors_.push_back(doc3);
    }

    bool confirm(const std::string &msg) {
        std::string resp;
        while (true) {
            std::cout << msg << " (y/n): ";
            if (!std::getline(std::cin, resp)) return false;
            if (resp == "y" || resp == "Y") return true;
            if (resp == "n" || resp == "N") return false;
            std::cout << "Please enter y or n.\n";
        }
    }

    Patient collectPatientInfo() {
        Patient p;
        std::cout << "Collecting the info of patient (Input)\n";
        std::cout << "Name: ";
        std::getline(std::cin, p.name);
        std::string ageStr;
        std::cout << "Age: ";
        std::getline(std::cin, ageStr);
        try { p.age = std::stoi(ageStr); } catch(...) { p.age = 0; }
        std::cout << "Contact: ";
        std::getline(std::cin, p.contact);
        return p;
    }

    void displayDoctors() {
        std::cout << "Available doctors:\n";
        for (const auto &d : doctors_) {
            std::cout << "[" << d.id << "] " << d.name << " (" << d.specialty << ") - " << d.available_times << "\n";
        }
    }

    Doctor selectDoctor() {
        displayDoctors();
        std::string sel;
        while (true) {
            std::cout << "Enter doctor id to select (or empty to cancel): ";
            std::getline(std::cin, sel);
            if (sel.empty()) break;
            try {
                int id = std::stoi(sel);
                for (const auto &d : doctors_) if (d.id == id) return d;
            } catch(...) {}
            std::cout << "Invalid id.\n";
        }
        return Doctor();
    }

    void saveAppointment(const Appointment &a) {
        appointments_.push(a);
    }

public:
    Scheduler() {
        initSampleDoctors();
    }

    void run() {
        while (true) {
            std::cout << "\n========== APPOINTMENT SCHEDULER ==========\n";
            std::cout << "1. Add new patient appointment\n";
            std::cout << "2. View saved appointments\n";
            std::cout << "3. Back to main menu\n";
            std::string choice;
            std::cout << "Enter choice (1-3): ";
            std::getline(std::cin, choice);

            if (choice == "1") {
                Patient p = collectPatientInfo();

                if (!confirm("Save patient information?")) {
                    std::cout << "Patient information not saved.\n";
                    continue;
                }

                std::cout << "Input the available time of the patient (e.g. 14:00): ";
                std::getline(std::cin, p.preferred_time);

                displayDoctors();

                if (!confirm("Select a doctor?")) {
                    std::cout << "No doctor selected. Returning to menu.\n";
                    continue;
                }

                Doctor d = selectDoctor();
                if (d.id == 0) {
                    std::cout << "No valid doctor selected. Returning to menu.\n";
                    continue;
                }

                std::cout << "Confirmation:\n";
                std::cout << "Patient: " << p.name << ", Age: " << p.age << ", Contact: " << p.contact << ", Preferred time: " << p.preferred_time << "\n";
                std::cout << "Doctor: " << d.name << " (" << d.specialty << ")\n";

                if (!confirm("Save this appointment?")) {
                    std::cout << "Cancel appointment.\n";
                    continue;
                }

                Appointment a{p, d, p.preferred_time};
                saveAppointment(a);

                std::cout << "Appointment was saved successfully!\n";

            } else if (choice == "2") {
                if (appointments_.empty()) {
                    std::cout << "\nNo appointments saved yet.\n";
                } else {
                    std::cout << "\n========== SAVED APPOINTMENTS ==========\n";
                    int idx = 1;
                    std::queue<Appointment> temp = appointments_;
                    while (!temp.empty()) {
                        auto &ap = temp.front();
                        std::cout << idx++ << ") Patient: " << ap.patient.name << " | Doctor: " << ap.doctor.name 
                                  << " (" << ap.doctor.specialty << ") | Time: " << ap.time << "\n";
                        temp.pop();
                    }
                    std::cout << "Total: " << appointments_.size() << " appointment(s)\n";
                }

            } else if (choice == "3") {
                break;

            } else {
                std::cout << "Invalid choice. Please enter 1, 2, or 3.\n";
            }
        }
    }

    int getAppointmentCount() {
        return appointments_.size();
    }
};

// ============================================================================
// DENTAL MANAGER CLASS
// ============================================================================
class DentalManager {
private:
    Scheduler scheduler;
    std::vector<Patient> patients;
    std::vector<Doctor> doctors;
    std::queue<Appointment> appointments;

public:
    DentalManager() {
        initializeDentists();
    }

    void initializeDentists() {
        // Initialize sample dentists
        Doctor doc1;
        doc1.id = 1;
        doc1.name = "Dr. Smith";
        doc1.specialty = "General Dentistry";
        doc1.available_times = "09:00-12:00";
        doctors.push_back(doc1);

        Doctor doc2;
        doc2.id = 2;
        doc2.name = "Dr. Lee";
        doc2.specialty = "Orthodontics";
        doc2.available_times = "10:00-14:00";
        doctors.push_back(doc2);

        Doctor doc3;
        doc3.id = 3;
        doc3.name = "Dr. Patel";
        doc3.specialty = "Periodontics";
        doc3.available_times = "13:00-17:00";
        doctors.push_back(doc3);
    }

    void displayMainMenu() {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════╗\n";
        std::cout << "║      DENTAL MANAGER SYSTEM             ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        std::cout << "1. Manage Appointments\n";
        std::cout << "2. View All Patients\n";
        std::cout << "3. View All Doctors\n";
        std::cout << "4. View All Appointments\n";
        std::cout << "5. Add New Patient\n";
        std::cout << "6. Exit System\n";
        std::cout << "════════════════════════════════════════\n";
        std::cout << "Enter your choice (1-6): ";
    }

    void manageAppointments() {
        scheduler.run();
    }

    void viewAllPatients() {
        if (patients.empty()) {
            std::cout << "\n[INFO] No patients registered yet.\n";
            return;
        }
        std::cout << "\n╔════════════════════════════════════════╗\n";
        std::cout << "║           REGISTERED PATIENTS          ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        int idx = 1;
        for (const auto &p : patients) {
            std::cout << idx++ << ") Name: " << p.name 
                      << " | Age: " << p.age 
                      << " | Contact: " << p.contact << "\n";
        }
    }

    void viewAllDoctors() {
        std::cout << "\n╔════════════════════════════════════════╗\n";
        std::cout << "║        AVAILABLE DENTAL DOCTORS        ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        for (const auto &d : doctors) {
            std::cout << "[ID: " << d.id << "] " << d.name 
                      << " (" << d.specialty << ")\n";
            std::cout << "    Available: " << d.available_times << "\n";
        }
    }

    void viewAllAppointments() {
        std::cout << "\n╔════════════════════════════════════════╗\n";
        std::cout << "║        SCHEDULED APPOINTMENTS          ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        if (appointments.empty()) {
            std::cout << "[INFO] No appointments scheduled yet.\n";
            return;
        }
        int idx = 1;
        std::queue<Appointment> temp = appointments;
        while (!temp.empty()) {
            auto &ap = temp.front();
            std::cout << idx++ << ") Patient: " << ap.patient.name 
                      << " | Doctor: " << ap.doctor.name 
                      << " | Time: " << ap.time << "\n";
            temp.pop();
        }
        std::cout << "Total Appointments: " << appointments.size() << "\n";
    }

    void addNewPatient() {
        std::cout << "\n╔════════════════════════════════════════╗\n";
        std::cout << "║        ADD NEW PATIENT RECORD          ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        
        Patient newPatient;
        
        std::cout << "Patient Name: ";
        std::getline(std::cin, newPatient.name);
        
        std::string ageInput;
        std::cout << "Patient Age: ";
        std::getline(std::cin, ageInput);
        try {
            newPatient.age = std::stoi(ageInput);
        } catch (...) {
            newPatient.age = 0;
        }
        
        std::cout << "Contact Number: ";
        std::getline(std::cin, newPatient.contact);
        
        patients.push_back(newPatient);
        std::cout << "[SUCCESS] Patient record added successfully!\n";
    }

    void displayStatistics() {
        std::cout << "\n╔════════════════════════════════════════╗\n";
        std::cout << "║          SYSTEM STATISTICS             ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        std::cout << "Total Patients Registered: " << patients.size() << "\n";
        std::cout << "Total Dentists Available: " << doctors.size() << "\n";
        std::cout << "Total Appointments Scheduled: " << appointments.size() << "\n";
    }

    void run() {
        std::string choice;
        bool running = true;

        while (running) {
            displayMainMenu();
            std::getline(std::cin, choice);

            if (choice == "1") {
                manageAppointments();
            } 
            else if (choice == "2") {
                viewAllPatients();
            } 
            else if (choice == "3") {
                viewAllDoctors();
            } 
            else if (choice == "4") {
                viewAllAppointments();
            } 
            else if (choice == "5") {
                addNewPatient();
            } 
            else if (choice == "6") {
                displayStatistics();
                std::cout << "\n[INFO] Thank you for using Dental Manager System!\n";
                std::cout << "Exiting program. Goodbye!\n";
                running = false;
            } 
            else {
                std::cout << "[ERROR] Invalid choice. Please enter 1-6.\n";
            }
        }
    }
};

int main() {
    DentalManager dentalSystem;
    dentalSystem.run();
    return 0;
}
