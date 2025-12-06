# Student Record Management System (C++)

A simple command-line student record management system built in C++ with an interactive CLI.

## Screenshots

| Login Screen | Student List |
|:------------:|:------------:|
| ![Login](screenshots/login.png) | ![Students](screenshots/students.png) |

## Features

- Admin authentication with username/password
- Add, view, update, and delete student records
- Search students by name, registration number, or major
- SQLite database for persistent storage
- Interactive menu system

## Requirements

- C++17 compiler (g++ or clang++)
- SQLite3 development libraries
- Make

### Install Dependencies (Ubuntu/Debian)

```bash
sudo apt install build-essential libsqlite3-dev
```

### Install Dependencies (Arch Linux)

```bash
sudo pacman -S base-devel sqlite
```

## Build

```bash
cd cpp
make
```

## Usage

```bash
./student_manager
```

### Default Admin Credentials

- Username: `admin`
- Password: `admin123`

## Project Structure

```
cpp/
  main.cpp      - Application entry point and CLI
  database.h    - Database class header
  database.cpp  - SQLite database operations
  student.h     - Student struct definition
  Makefile      - Build configuration
```

## Database

The application automatically creates a `students.db` SQLite database file on first run. Student records include:

- ID (auto-generated)
- Name
- Registration Number
- Age
- Major

## License

MIT
