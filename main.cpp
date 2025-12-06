#include <iostream>
#include <string>
#include <limits>
#include <termios.h>
#include <unistd.h>
#include "database.h"
#include "student.h"

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

void clear_screen() {
    std::cout << "\033[2J\033[1;1H";
}

void print_banner() {
    std::cout << CYAN << BOLD << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║   ███████╗████████╗██╗   ██╗██████╗ ███████╗███╗   ██╗████████╗║
    ║   ██╔════╝╚══██╔══╝██║   ██║██╔══██╗██╔════╝████╗  ██║╚══██╔══╝║
    ║   ███████╗   ██║   ██║   ██║██║  ██║█████╗  ██╔██╗ ██║   ██║   ║
    ║   ╚════██║   ██║   ██║   ██║██║  ██║██╔══╝  ██║╚██╗██║   ██║   ║
    ║   ███████║   ██║   ╚██████╔╝██████╔╝███████╗██║ ╚████║   ██║   ║
    ║   ╚══════╝   ╚═╝    ╚═════╝ ╚═════╝ ╚══════╝╚═╝  ╚═══╝   ╚═╝   ║
    ║                                                               ║
    ║        RECORD MANAGEMENT SYSTEM (C++ Edition)                 ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << RESET << std::endl;
}

std::string get_password() {
    std::string password;
    struct termios oldt, newt;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    std::getline(std::cin, password);
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << std::endl;
    
    return password;
}

void clear_input() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int get_int_input() {
    int value;
    while (!(std::cin >> value)) {
        std::cout << RED << "Invalid input. Enter a number: " << RESET;
        clear_input();
    }
    clear_input();
    return value;
}

void view_all_students(Database& db) {
    auto students = db.get_all_students();
    
    if (students.empty()) {
        std::cout << "\n" << YELLOW << BOLD << "No students found in the database." << RESET << std::endl;
        return;
    }

    std::cout << "\n" << CYAN << "╔═══════════════════════════════════════════════════════════════════════════╗" << RESET << std::endl;
    std::cout << CYAN << "║                           All Students                                     ║" << RESET << std::endl;
    std::cout << CYAN << "╠═══════════════════════════════════════════════════════════════════════════╣" << RESET << std::endl;

    for (const auto& s : students) {
        std::cout << CYAN << "║" << RESET << std::endl;
        std::cout << CYAN << "║  " << RESET << BLUE << BOLD << "ID: " << RESET << WHITE << s.id << RESET << std::endl;
        std::cout << CYAN << "║  " << RESET << GREEN << "Name: " << RESET << WHITE << s.name << RESET << std::endl;
        std::cout << CYAN << "║  " << RESET << GREEN << "Reg No: " << RESET << WHITE << s.reg_no << RESET << std::endl;
        std::cout << CYAN << "║  " << RESET << GREEN << "Age: " << RESET << WHITE << s.age << RESET << std::endl;
        std::cout << CYAN << "║  " << RESET << GREEN << "Major: " << RESET << WHITE << s.major << RESET << std::endl;
        std::cout << CYAN << "╟───────────────────────────────────────────────────────────────────────────╢" << RESET << std::endl;
    }

    std::cout << CYAN << "╚═══════════════════════════════════════════════════════════════════════════╝" << RESET << std::endl;
    std::cout << "\n" << WHITE << BOLD << "Total: " << students.size() << " student(s)" << RESET << std::endl;
}

void add_student(Database& db) {
    std::cout << "\n" << GREEN << "┌────────────────────────────────────────┐" << RESET << std::endl;
    std::cout << GREEN << "│          Add New Student               │" << RESET << std::endl;
    std::cout << GREEN << "└────────────────────────────────────────┘" << RESET << std::endl;

    std::string name, reg_no, major;
    int age;

    std::cout << "  Full Name: ";
    std::getline(std::cin, name);

    std::cout << "  Registration Number: ";
    std::getline(std::cin, reg_no);

    std::cout << "  Age: ";
    age = get_int_input();

    std::cout << "  Major/Department: ";
    std::getline(std::cin, major);

    int id = db.add_student(name, reg_no, age, major);
    if (id > 0) {
        std::cout << "\n" << GREEN << BOLD << "Student added successfully! (ID: " << id << ")" << RESET << std::endl;
    } else {
        std::cout << "\n" << RED << BOLD << "Error adding student. Registration number may already exist." << RESET << std::endl;
    }
}

void update_student(Database& db) {
    auto students = db.get_all_students();
    
    if (students.empty()) {
        std::cout << "\n" << YELLOW << BOLD << "No students to update." << RESET << std::endl;
        return;
    }

    std::cout << "\n" << YELLOW << "┌────────────────────────────────────────┐" << RESET << std::endl;
    std::cout << YELLOW << "│          Update Student                │" << RESET << std::endl;
    std::cout << YELLOW << "└────────────────────────────────────────┘" << RESET << std::endl;

    std::cout << "\nAvailable students:\n";
    for (const auto& s : students) {
        std::cout << "  [" << s.id << "] " << s.name << " - " << s.reg_no << std::endl;
    }

    std::cout << "\nEnter Student ID to update: ";
    int id = get_int_input();

    Student* student = db.get_student(id);
    if (!student) {
        std::cout << RED << BOLD << "Student not found!" << RESET << std::endl;
        return;
    }

    std::cout << "\n" << YELLOW << "Current: " << student->name << " | " << student->reg_no 
              << " | Age: " << student->age << " | Major: " << student->major << RESET << std::endl;
    std::cout << "(Press Enter to keep current value)\n" << std::endl;

    std::string name, reg_no, major, age_str;

    std::cout << "  New Name [" << student->name << "]: ";
    std::getline(std::cin, name);
    if (name.empty()) name = student->name;

    std::cout << "  New Reg No [" << student->reg_no << "]: ";
    std::getline(std::cin, reg_no);
    if (reg_no.empty()) reg_no = student->reg_no;

    std::cout << "  New Age [" << student->age << "]: ";
    std::getline(std::cin, age_str);
    int age = age_str.empty() ? student->age : std::stoi(age_str);

    std::cout << "  New Major [" << student->major << "]: ";
    std::getline(std::cin, major);
    if (major.empty()) major = student->major;

    std::cout << "\nConfirm update? (y/n): ";
    std::string confirm;
    std::getline(std::cin, confirm);

    if (confirm == "y" || confirm == "Y") {
        if (db.update_student(id, name, reg_no, age, major)) {
            std::cout << GREEN << BOLD << "Student updated successfully!" << RESET << std::endl;
        } else {
            std::cout << RED << BOLD << "Error updating student." << RESET << std::endl;
        }
    } else {
        std::cout << YELLOW << "Update cancelled." << RESET << std::endl;
    }

    delete student;
}

void delete_student(Database& db) {
    auto students = db.get_all_students();
    
    if (students.empty()) {
        std::cout << "\n" << YELLOW << BOLD << "No students to delete." << RESET << std::endl;
        return;
    }

    std::cout << "\n" << RED << "┌────────────────────────────────────────┐" << RESET << std::endl;
    std::cout << RED << "│          Delete Student                │" << RESET << std::endl;
    std::cout << RED << "└────────────────────────────────────────┘" << RESET << std::endl;

    std::cout << "\nAvailable students:\n";
    for (const auto& s : students) {
        std::cout << "  [" << s.id << "] " << s.name << " - " << s.reg_no << std::endl;
    }

    std::cout << "\nEnter Student ID to delete: ";
    int id = get_int_input();

    Student* student = db.get_student(id);
    if (!student) {
        std::cout << RED << BOLD << "Student not found!" << RESET << std::endl;
        return;
    }

    std::cout << "\n" << RED << BOLD << "About to delete: " << student->name << " (" << student->reg_no << ")" << RESET << std::endl;
    std::cout << "Type 'YES' to confirm: ";
    
    std::string confirm;
    std::getline(std::cin, confirm);

    if (confirm == "YES") {
        if (db.delete_student(id)) {
            std::cout << GREEN << BOLD << "Student deleted successfully!" << RESET << std::endl;
        } else {
            std::cout << RED << BOLD << "Error deleting student." << RESET << std::endl;
        }
    } else {
        std::cout << YELLOW << "Deletion cancelled." << RESET << std::endl;
    }

    delete student;
}

void search_students(Database& db) {
    std::cout << "\n" << BLUE << "┌────────────────────────────────────────┐" << RESET << std::endl;
    std::cout << BLUE << "│          Search Students               │" << RESET << std::endl;
    std::cout << BLUE << "└────────────────────────────────────────┘" << RESET << std::endl;

    std::cout << "  Search (name, reg no, or major): ";
    std::string query;
    std::getline(std::cin, query);

    auto students = db.search_students(query);

    if (students.empty()) {
        std::cout << "\n" << YELLOW << BOLD << "No students found matching '" << query << "'" << RESET << std::endl;
        return;
    }

    std::cout << "\n" << WHITE << BOLD << "Found " << students.size() << " result(s) for '" << query << "':" << RESET << std::endl;

    for (const auto& s : students) {
        std::cout << "\n" << CYAN << "┌─────────────────────────────────────┐" << RESET << std::endl;
        std::cout << "  " << BLUE << BOLD << "ID: " << RESET << s.id << std::endl;
        std::cout << "  " << GREEN << "Name: " << RESET << s.name << std::endl;
        std::cout << "  " << GREEN << "Reg No: " << RESET << s.reg_no << std::endl;
        std::cout << "  " << GREEN << "Age: " << RESET << s.age << std::endl;
        std::cout << "  " << GREEN << "Major: " << RESET << s.major << std::endl;
        std::cout << CYAN << "└─────────────────────────────────────┘" << RESET << std::endl;
    }
}

void view_student_by_id(Database& db) {
    std::cout << "\n" << BLUE << "┌────────────────────────────────────────┐" << RESET << std::endl;
    std::cout << BLUE << "│          View Student by ID            │" << RESET << std::endl;
    std::cout << BLUE << "└────────────────────────────────────────┘" << RESET << std::endl;

    std::cout << "  Enter Student ID: ";
    int id = get_int_input();

    Student* student = db.get_student(id);
    if (!student) {
        std::cout << "\n" << RED << BOLD << "No student found with ID " << id << RESET << std::endl;
        return;
    }

    std::cout << "\n" << CYAN << "╔═════════════════════════════════════════╗" << RESET << std::endl;
    std::cout << CYAN << "║           Student Details               ║" << RESET << std::endl;
    std::cout << CYAN << "╠═════════════════════════════════════════╣" << RESET << std::endl;
    std::cout << CYAN << "║  " << RESET << BLUE << BOLD << "ID: " << RESET << student->id << std::endl;
    std::cout << CYAN << "║  " << RESET << GREEN << "Name: " << RESET << student->name << std::endl;
    std::cout << CYAN << "║  " << RESET << GREEN << "Reg No: " << RESET << student->reg_no << std::endl;
    std::cout << CYAN << "║  " << RESET << GREEN << "Age: " << RESET << student->age << std::endl;
    std::cout << CYAN << "║  " << RESET << GREEN << "Major: " << RESET << student->major << std::endl;
    std::cout << CYAN << "╚═════════════════════════════════════════╝" << RESET << std::endl;

    delete student;
}

void admin_menu(Database& db) {
    std::cout << "\n" << GREEN << BOLD << "Login successful! Welcome, Admin!" << RESET << std::endl;

    while (true) {
        std::cout << "\n" << MAGENTA << "╔════════════════════════════════════════╗" << RESET << std::endl;
        std::cout << MAGENTA << "║          Admin Dashboard               ║" << RESET << std::endl;
        std::cout << MAGENTA << "╚════════════════════════════════════════╝" << RESET << std::endl;

        std::cout << "\n";
        std::cout << GREEN << "  [1] View All Students" << RESET << std::endl;
        std::cout << GREEN << "  [2] Add New Student" << RESET << std::endl;
        std::cout << YELLOW << "  [3] Update Student" << RESET << std::endl;
        std::cout << RED << "  [4] Delete Student" << RESET << std::endl;
        std::cout << BLUE << "  [5] Search Students" << RESET << std::endl;
        std::cout << BLUE << "  [6] View Student by ID" << RESET << std::endl;
        std::cout << MAGENTA << "  [7] Logout" << RESET << std::endl;

        std::cout << "\nSelect an option: ";
        int choice = get_int_input();

        switch (choice) {
            case 1: view_all_students(db); break;
            case 2: add_student(db); break;
            case 3: update_student(db); break;
            case 4: delete_student(db); break;
            case 5: search_students(db); break;
            case 6: view_student_by_id(db); break;
            case 7:
                std::cout << "\n" << MAGENTA << BOLD << "Logged out successfully!" << RESET << std::endl;
                return;
            default:
                std::cout << RED << "Invalid option. Try again." << RESET << std::endl;
        }
    }
}

bool admin_login(Database& db) {
    std::cout << "\n" << CYAN << "┌────────────────────────────────────────┐" << RESET << std::endl;
    std::cout << CYAN << "│           Admin Login                  │" << RESET << std::endl;
    std::cout << CYAN << "└────────────────────────────────────────┘" << RESET << std::endl;

    std::string username, password;

    std::cout << "  Username: ";
    std::getline(std::cin, username);

    std::cout << "  Password: ";
    password = get_password();

    return db.verify_admin(username, password);
}

int main() {
    Database db("students.db");
    
    if (!db.init()) {
        std::cerr << "Failed to initialize database!" << std::endl;
        return 1;
    }

    clear_screen();
    print_banner();

    while (true) {
        std::cout << "\n" << CYAN << "╔════════════════════════════════════════╗" << RESET << std::endl;
        std::cout << CYAN << "║           Main Menu                    ║" << RESET << std::endl;
        std::cout << CYAN << "╚════════════════════════════════════════╝" << RESET << std::endl;

        std::cout << "\n";
        std::cout << "  [1] Admin Login" << std::endl;
        std::cout << "  [2] Exit" << std::endl;

        std::cout << "\nSelect an option: ";
        int choice = get_int_input();

        switch (choice) {
            case 1:
                if (admin_login(db)) {
                    admin_menu(db);
                } else {
                    std::cout << "\n" << RED << BOLD << "Invalid credentials!" << RESET << std::endl;
                }
                break;
            case 2:
                std::cout << "\n" << GREEN << BOLD << "Goodbye! Thanks for using Student Manager!" << RESET << std::endl;
                return 0;
            default:
                std::cout << RED << "Invalid option. Try again." << RESET << std::endl;
        }
    }

    return 0;
}
