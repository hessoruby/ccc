#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include "student.h"

class Database {
private:
    sqlite3* db;
    std::string db_path;

public:
    Database(const std::string& path);
    ~Database();

    bool init();
    bool verify_admin(const std::string& username, const std::string& password);
    
    int add_student(const std::string& name, const std::string& reg_no, int age, const std::string& major);
    std::vector<Student> get_all_students();
    Student* get_student(int id);
    bool update_student(int id, const std::string& name, const std::string& reg_no, int age, const std::string& major);
    bool delete_student(int id);
    std::vector<Student> search_students(const std::string& query);
};

#endif // DATABASE_H
