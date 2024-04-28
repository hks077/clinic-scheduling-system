#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>
#include <conio.h>
#define MAX_DOCTORS 100
#define MAX_APPOINTMENTS 100
using namespace std;

class Doctor {
public:
    string name;
    string specialization;
    int appointmentsCount;

    Doctor() : name(""), specialization(""), appointmentsCount(0) {}
};

class Appointment {
public:
    string patientName;
    string doctorName;
    string reason;
    int day;
    int month;
    int year;
    string bookingID;

    Appointment() : patientName(""), doctorName(""), reason(""), day(0), month(0), year(0), bookingID("") {}
};

class HospitalSystem {
private:
    int lastBookingID = 0;
    Doctor doctors[MAX_DOCTORS];
    Appointment appointments[MAX_APPOINTMENTS];
    int doctorCount;
    int appointmentCount;

    void generateBookingID(Appointment& appointment) {
        lastBookingID++;
        appointment.bookingID = to_string(lastBookingID);
    }

    void saveDoctorsToFile() {
        ofstream file("doctors.txt");
        if (file.is_open()) {
            for (int i = 0; i < doctorCount; ++i) {
                file << doctors[i].name << ',' << doctors[i].specialization << ',' << doctors[i].appointmentsCount << '\n';
            }
            file.close();
            cout << "Doctor details saved to file.\n";
        } else {
            cout << "Unable to open file to save doctor details.\n";
        }
    }

    void loadDoctorsFromFile() {
        ifstream file("doctors.txt");
        if (file.is_open()) {
            doctorCount = 0;
            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string name, specialization;
                int appointmentsCount;
                getline(ss, name, ',');
                getline(ss, specialization, ',');
                ss >> appointmentsCount;

                doctors[doctorCount].name = name;
                doctors[doctorCount].specialization = specialization;
                doctors[doctorCount].appointmentsCount = appointmentsCount;

                doctorCount++;
            }
            file.close();
            cout << "Doctors loaded from file.\n";
        } else {
            cout << "No existing doctor details found.\n";
        }
    }

    void saveAppointmentsToFile() {
        ofstream file("appointments.txt");
        if (file.is_open()) {
            for (int i = 0; i < appointmentCount; ++i) {
                file << appointments[i].patientName << ',' << appointments[i].doctorName << ',' << appointments[i].reason << ',' << appointments[i].day << '/' << appointments[i].month << '/' << appointments[i].year << '\n';
            }
            file.close();
            cout << "Appointment details saved to file.\n";
        } else {
            cout << "Unable to open file to save appointment details.\n";
        }
    }

    void loadAppointmentsFromFile() {
        ifstream file("appointments.txt");
        if (file.is_open()) {
            appointmentCount = 0;
            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string patientName, doctorName, reason;
                int day, month, year;

                getline(ss, patientName, ',');
                getline(ss, doctorName, ',');
                getline(ss, reason, ',');
                char delim;
                ss >> day >> delim >> month >> delim >> year;

                if (ss.fail() || delim != '/') {
                    cout << "Invalid appointment data detected in the file. Skipping appointment.\n";
                    continue;
                }

                appointments[appointmentCount].patientName = patientName;
                appointments[appointmentCount].doctorName = doctorName;
                appointments[appointmentCount].reason = reason;
                appointments[appointmentCount].day = day;
                appointments[appointmentCount].month = month;
                appointments[appointmentCount].year = year;

                appointmentCount++;
            }
            file.close();
            cout << "Appointments loaded from file.\n";
        } else {
            cout << "No existing appointment details found.\n";
        }
    }

public:
    HospitalSystem() : doctorCount(0), appointmentCount(0) {}

    void deleteAppointment() {
    if (appointmentCount == 0) {
        cout << "No appointments available to delete.\n";
        return;
    }

    cout << "List of Appointments:\n";
    for (int i = 0; i < appointmentCount; ++i) {
        cout << i + 1 << ". " << appointments[i].patientName << " - " << appointments[i].doctorName << " - " << appointments[i].day << "/" << appointments[i].month << "/" << appointments[i].year << endl;
    }

    int choice;
    cout << "Enter the number of the appointment to delete: ";
    cin >> choice;

    if (choice > 0 && choice <= appointmentCount) {
        for (int i = choice - 1; i < appointmentCount - 1; ++i) {
            appointments[i] = appointments[i + 1];
        }
        appointmentCount--;

        saveAppointmentsToFile();

        cout << "Appointment deleted successfully!\n";
    } else {
        cout << "Invalid choice. No appointment deleted.\n";
    }
}


    void cancelAppointment() {
    if (appointmentCount == 0) {
        cout << "No appointments available to cancel.\n";
        return;
    }

    string patientName;
    string bookingID; // Added variable to store the entered Booking ID
    cout << "Enter your name: ";
    cin.ignore();
    getline(cin, patientName);

    cout << "Enter Booking ID: ";
    cin >> bookingID;

    bool found = false;
    for (int i = 0; i < appointmentCount; ++i) {
        if (appointments[i].patientName == patientName && appointments[i].bookingID == bookingID) {
            cout << "Your Appointment Details: " << appointments[i].doctorName << " - " << appointments[i].day << "/" << appointments[i].month << "/" << appointments[i].year << endl;
            cout << "Do you want to cancel this appointment? (Y/N): ";
            char choice;
            cin >> choice;
            if (choice == 'Y' || choice == 'y') {
                for (int j = i; j < appointmentCount - 1; ++j) {
                    appointments[j] = appointments[j + 1];
                }
                appointmentCount--;
                cout << "Appointment cancelled successfully!\n";
                saveAppointmentsToFile();
            }
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "No appointment found under your name or with the provided Booking ID.\n";
    }
}


    void bookAppointment() {
    if (appointmentCount >= MAX_APPOINTMENTS) {
        cout << "Maximum number of appointments reached!\n";
        return;
    }

    if (doctorCount == 0) {
        cout << "No doctors available for appointments.\n";
        return;
    }

    string patientName;
    cout << "Enter Patient Name: ";
    cin.ignore();
    getline(cin, patientName);

    // Get the current date
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);
    tm* localTime = localtime(&currentTime);

    int currentDay = localTime->tm_mday;
    int currentMonth = localTime->tm_mon + 1; // Month is zero-indexed, so adding 1
    int currentYear = localTime->tm_year + 1900; // Years since 1900

    int day, month, year;
    bool validDate = false;

    do {
        cout << "Enter Appointment Date (Day Month Year): ";
        cin >> day >> month >> year;

        if (year > currentYear || (year == currentYear && month > currentMonth) || (year == currentYear && month == currentMonth && day >= currentDay)) {
            // Appointment date is today or later
            validDate = true;
        } else {
            cout << "Appointment date should be today or later. Please enter a valid date.\n";
        }
    } while (!validDate);

    cout << "Available Specializations:\n";
    // Display available specializations
    for (int i = 0; i < doctorCount; ++i) {
        cout << i + 1 << ". " << doctors[i].specialization << endl;
    }

    int chosenSpec;
    cout << "Enter your choice of specialization: ";
    cin >> chosenSpec;

    if (chosenSpec <= 0 || chosenSpec > doctorCount) {
        cout << "Invalid choice of specialization!\n";
        return;
    }

    string desiredSpecialization = doctors[chosenSpec - 1].specialization;

    cout << "Doctors Available for " << desiredSpecialization << ":\n";
    // Display available doctors for the chosen specialization
    int availableDoctorCount = 0;
    for (int i = 0; i < doctorCount; ++i) {
        if (doctors[i].specialization == desiredSpecialization) {
            cout << availableDoctorCount + 1 << ". " << doctors[i].name << endl;
            availableDoctorCount++;
        }
    }

    if (availableDoctorCount == 0) {
        cout << "No doctors available for the specified specialization.\n";
        return;
    }

    int doctorChoice;
    cout << "Enter Doctor Choice: ";
    cin >> doctorChoice;

    if (doctorChoice <= 0 || doctorChoice > availableDoctorCount) {
        cout << "Invalid Doctor Choice!\n";
        return;
    }

    Doctor selectedDoctor;
    string selectedDoctorName;
    for (int i = 0; i < doctorCount; ++i) {
        if (doctors[i].specialization == desiredSpecialization) {
            if (--doctorChoice == 0) {
                selectedDoctor = doctors[i];
                selectedDoctorName = selectedDoctor.name;
                break;
            }
        }
    }

    if (selectedDoctor.appointmentsCount >= 40) {
        cout << "Appointment limit exceeded for this doctor on this day!\n";
        return;
    }

    string reason;
    cout << "Enter Reason for Appointment: ";
    cin.ignore();
    getline(cin, reason);

    lastBookingID++;
    appointments[appointmentCount].bookingID = to_string(lastBookingID);

    appointments[appointmentCount].patientName = patientName;
    appointments[appointmentCount].doctorName = selectedDoctorName;
    appointments[appointmentCount].reason = reason;
    appointments[appointmentCount].day = day;
    appointments[appointmentCount].month = month;
    appointments[appointmentCount].year = year;

    appointmentCount++;
    selectedDoctor.appointmentsCount++;

    cout << "Appointment booked successfully! Your Booking ID is: " << appointments[appointmentCount - 1].bookingID << endl;
}


    void viewAppointments() {
    if (appointmentCount == 0) {
        cout << "No appointments available.\n";
        return;
    }

    cout << "List of Appointments:\n";
    for (int i = 0; i < appointmentCount; ++i) {
        cout << i + 1 << ". " << appointments[i].patientName << " - " << appointments[i].doctorName << " - " << appointments[i].day << "/" << appointments[i].month << "/" << appointments[i].year << endl;
    }
}


    void patientMenu() {
    int choice;

    do {
        cout << "\nPatient Menu\n";
        cout << "1. Book Appointment\n";
        cout << "2. Cancel Appointment\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                bookAppointment();
                break;
            case 2:
                cancelAppointment(); // Function to cancel appointment
                break;
            case 3:
                cout << "Exiting Patient Menu.\n";
                break;
            default:
                cout << "Invalid choice. Please enter a valid option.\n";
                break;
        }
    } while (choice != 3);
}


    void patientLogin() {
    loadAppointmentsFromFile(); // Load existing patient appointments from file
    patientMenu(); // Enter the patient menu
}


    void adminMenu() {
    int choice;

    do {
        cout << "\nAdmin Menu\n";
        cout << "1. View Doctors\n";
        cout << "2. Add Doctor\n";
        cout << "3. Remove Doctor\n";
        cout << "4. Delete Appointment\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                viewDoctors();
                break;
            case 2:
                addDoctor();
                break;
            case 3:
                removeDoctor();
                break;
            case 4:
                deleteAppointment();
                break;
            case 5:
                cout << "Exiting Admin Menu.\n";
                break;
            default:
                cout << "Invalid choice. Please enter a valid option.\n";
                break;
        }
    } while (choice != 5);
}


    void adminLogin() {
    const string ADMIN_PASSWORD = "admin123"; // Define the admin password

    string enteredPassword;

    cout << "Enter admin password: ";
    cin >> enteredPassword;

    if (enteredPassword == ADMIN_PASSWORD) {
        cout << "Login successful!\n";
        adminMenu(); // Redirect to the admin menu upon successful login
    } else {
        cout << "Incorrect password. Access denied.\n";
    }
}


    void adminLoginPassword() {
    const char RETURN = 13; // ASCII value for Enter key
    const char BACKSPACE = 8; // ASCII value for Backspace key

    string password;
    char ch = 0;

    cout << "Enter admin password: ";

    while ((ch = _getch()) != RETURN) {
        if (ch == BACKSPACE) {
            if (!password.empty()) {
                cout << "\b \b"; // Move cursor back, erase character, move cursor back again
                password.pop_back(); // Remove the last character from the password string
            }
        } else {
            password += ch;
            cout << '*'; // Print '*' for each character
        }
    }

    // Check if password is correct (Here, you would compare the password with a stored password)
    if (password == "admin123") {
        std::cout << "\nLogin successful!\n";
        // Redirect to admin menu or perform desired admin actions
    } else {
        cout << "\nIncorrect password. Access denied.\n";
    }
}

    void startSystem() {
        int choice;

        do {
            cout << "CLINIC SCHEDULING MANAGEMENT SYSTEM...\n";
            cout << "1. Admin Login\n";
            cout << "2. Patient Login\n";
            cout << "3. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    adminLogin();
                    break;
                case 2:
                    patientLogin();
                    break;
                case 3:
                    cout << "Exiting the program. Goodbye!\n";
                    break;
                default:
                    cout << "Invalid choice. Please enter a valid option.\n";
                    break;
            }
        } while (choice != 3);
    }
    void viewDoctors() {
    if (doctorCount == 0) {
        cout << "No doctors available.\n";
        return;
    }

    cout << "List of Doctors:\n";
    for (int i = 0; i < doctorCount; ++i) {
        cout << "Doctor " << i + 1 << ": " << doctors[i].name << " - " << doctors[i].specialization << " - Appointments: " << doctors[i].appointmentsCount << endl;
    }
}


    void addDoctor() {
    if (doctorCount < MAX_DOCTORS) {
        cout << "Enter Doctor Name: ";
        cin.ignore();
        getline(cin, doctors[doctorCount].name);

        cout << "Enter Doctor Specialization: ";
        getline(cin, doctors[doctorCount].specialization);

        doctors[doctorCount].appointmentsCount = 0;
        doctorCount++;

        cout << "Doctor added successfully!\n";
        saveDoctorsToFile(); // Assuming you have a function to save doctors to a file
    } else {
        cout << "Maximum doctors limit reached. Cannot add more doctors.\n";
    }
}


    void removeDoctor() {
    if (doctorCount > 0) {
        int index;
        cout << "Enter index of doctor to remove: ";
        cin >> index;

        if (index >= 1 && index <= doctorCount) {
            for (int i = index - 1; i < doctorCount - 1; ++i) {
                doctors[i] = doctors[i + 1];
            }
            doctorCount--;

            cout << "Doctor removed successfully!\n";
            saveDoctorsToFile(); // Assuming you have a function to save doctors to a file
        } else {
            cout << "Invalid index. No doctor removed.\n";
        }
    } else {
        cout << "No doctors available to remove.\n";
    }
}



};

int main() {
    HospitalSystem hospital;
    hospital.startSystem();
    return 0;
}
