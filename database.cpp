#include "database.h"
#include <iostream>

Database::Database(const std::string& path) : db(nullptr), db_path(path) {}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}

bool Database::init() {
    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Create admins table
    const char* sql_admins = R"(
        CREATE TABLE IF NOT EXISTS admins (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL
        );
    )";

    char* err_msg = nullptr;
    rc = sqlite3_exec(db, sql_admins, nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }

    // Create students table
    const char* sql_students = R"(
        CREATE TABLE IF NOT EXISTS students (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            reg_no TEXT UNIQUE NOT NULL,
            age INTEGER NOT NULL,
            major TEXT NOT NULL
        );
    )";

    rc = sqlite3_exec(db, sql_students, nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }

    // Check if default admin exists
    const char* sql_count = "SELECT COUNT(*) FROM admins;";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql_count, -1, &stmt, nullptr);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);

        if (count == 0) {
            const char* sql_insert = "INSERT INTO admins (username, password_hash) VALUES ('admin', 'admin123');";
            rc = sqlite3_exec(db, sql_insert, nullptr, nullptr, &err_msg);
            if (rc != SQLITE_OK) {
                std::cerr << "SQL error: " << err_msg << std::endl;
                sqlite3_free(err_msg);
            } else {
                std::cout << "Default admin created: username='admin', password='admin123'" << std::endl;
            }
        }
    } else {
        sqlite3_finalize(stmt);
    }

    return true;
}

bool Database::verify_admin(const std::string& username, const std::string& password) {
    const char* sql = "SELECT password_hash FROM admins WHERE username = ?;";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string stored_password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
        return stored_password == password;
    }

    sqlite3_finalize(stmt);
    return false;
}

int Database::add_student(const std::string& name, const std::string& reg_no, int age, const std::string& major) {
    const char* sql = "INSERT INTO students (name, reg_no, age, major) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, reg_no.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, age);
    sqlite3_bind_text(stmt, 4, major.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        return -1;
    }

    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

std::vector<Student> Database::get_all_students() {
    std::vector<Student> students;
    const char* sql = "SELECT id, name, reg_no, age, major FROM students ORDER BY id;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return students;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Student s;
        s.id = sqlite3_column_int(stmt, 0);
        s.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        s.reg_no = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        s.age = sqlite3_column_int(stmt, 3);
        s.major = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        students.push_back(s);
    }

    sqlite3_finalize(stmt);
    return students;
}

Student* Database::get_student(int id) {
    const char* sql = "SELECT id, name, reg_no, age, major FROM students WHERE id = ?;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return nullptr;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Student* s = new Student();
        s->id = sqlite3_column_int(stmt, 0);
        s->name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        s->reg_no = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        s->age = sqlite3_column_int(stmt, 3);
        s->major = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        sqlite3_finalize(stmt);
        return s;
    }

    sqlite3_finalize(stmt);
    return nullptr;
}

bool Database::update_student(int id, const std::string& name, const std::string& reg_no, int age, const std::string& major) {
    const char* sql = "UPDATE students SET name = ?, reg_no = ?, age = ?, major = ? WHERE id = ?;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, reg_no.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, age);
    sqlite3_bind_text(stmt, 4, major.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool Database::delete_student(int id) {
    const char* sql = "DELETE FROM students WHERE id = ?;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE && sqlite3_changes(db) > 0;
}

std::vector<Student> Database::search_students(const std::string& query) {
    std::vector<Student> students;
    const char* sql = "SELECT id, name, reg_no, age, major FROM students WHERE name LIKE ? OR reg_no LIKE ? OR major LIKE ? ORDER BY id;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return students;
    }

    std::string search = "%" + query + "%";
    sqlite3_bind_text(stmt, 1, search.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, search.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, search.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Student s;
        s.id = sqlite3_column_int(stmt, 0);
        s.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        s.reg_no = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        s.age = sqlite3_column_int(stmt, 3);
        s.major = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        students.push_back(s);
    }

    sqlite3_finalize(stmt);
    return students;
}
