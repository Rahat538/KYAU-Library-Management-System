#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <limits>
using namespace std;

// Constants
const int MAX_BOOKS = 100;
const int MAX_STUDENTS = 100;
const int MAX_NAME_LEN = 50;

// Book structure
struct Book {
    char title[MAX_NAME_LEN];
    char author[MAX_NAME_LEN];
    int isbn;
    bool available;
    int borrowed_by_roll;
};

// Student structure
struct Student {
    char name[MAX_NAME_LEN];
    int roll;
};

// Global arrays
Book books[MAX_BOOKS];
Student students[MAX_STUDENTS];
int book_count = 0;
int student_count = 0;

// Function prototypes
void load_books();
void save_books();
void load_students();
void save_students();
void add_book();
void view_books();
void create_account();
void view_students();
void view_borrowed_books();
void borrow_book(int roll);
void return_book(int roll);
void clear_input_buffer();

int find_student(int roll);
int find_book(int isbn);

int main() {
    cout << "==============================\n";
    cout << " Welcome to KYAU Library System\n";
    cout << "==============================\n\n";

    // Silent loading of data
    ifstream book_check("books.txt");
    if (book_check) {
        book_check.close();
        load_books();
    }

    ifstream student_check("students.txt");
    if (student_check) {
        student_check.close();
        load_students();
    }

    int choice;
    while (true) {
        cout << "\nMain Menu:\n";
        cout << "1. Admin Login\n";
        cout << "2. Student Login\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            clear_input_buffer();
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 0) {
            cout << "Thank you for using KYAU Library. Goodbye!\n";
            break;
        }

        if (choice == 1) {
            string password;
            cout << "Enter Admin Password (or type 'back' to go back): ";
            cin >> password;

            if (password == "back") continue;

            if (password == "kyau25") {
                int admin_choice;
                while (true) {
                    cout << "\n--- Admin Menu ---\n";
                    cout << "1. Add Book\n";
                    cout << "2. View All Books\n";
                    cout << "3. View Students\n";
                    cout << "4. View Borrowed Books\n";
                    cout << "5. Go Back\n";
                    cout << "Enter your choice: ";

                    if (!(cin >> admin_choice)) {
                        clear_input_buffer();
                        cout << "Invalid input. Please enter a number.\n";
                        continue;
                    }

                    if (admin_choice == 5) break;

                    switch (admin_choice) {
                        case 1: add_book(); break;
                        case 2: view_books(); break;
                        case 3: view_students(); break;
                        case 4: view_borrowed_books(); break;
                        default: cout << "Invalid option.\n"; break;
                    }
                }
            } else {
                cout << "Wrong password!\n";
            }
        } else if (choice == 2) {
            int roll;
            cout << "Enter your Roll Number (or 0 to go back): ";

            if (!(cin >> roll)) {
                clear_input_buffer();
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }

            if (roll == 0) continue;

            int index = find_student(roll);

            if (index == -1) {
                cout << "Student not found. Do you want to create an account?\n1. Yes\n2. No\nChoice: ";
                int create;

                if (!(cin >> create)) {
                    clear_input_buffer();
                    cout << "Invalid input. Please enter a number.\n";
                    continue;
                }

                if (create == 1) {
                    create_account();
                    index = student_count - 1; // Get the index of newly created account
                } else {
                    continue;
                }
            }

            // Personalized welcome message
            cout << "\n=================================\n";
            cout << " Welcome to KYAU Library, " << students[index].name << " (" << students[index].roll << ")";
            cout << "\n=================================\n";

            int student_choice;
            while (true) {
                cout << "\n--- Student Menu ---\n";
                cout << "1. View Available Books\n";
                cout << "2. Borrow Book\n";
                cout << "3. Return Book\n";
                cout << "4. Go Back\n";
                cout << "Enter your choice: ";

                if (!(cin >> student_choice)) {
                    clear_input_buffer();
                    cout << "Invalid input. Please enter a number.\n";
                    continue;
                }

                if (student_choice == 4) break;

                switch (student_choice) {
                    case 1: view_books(); break;
                    case 2: borrow_book(students[index].roll); break;
                    case 3: return_book(students[index].roll); break;
                    default: cout << "Invalid option.\n"; break;
                }
            }
        } else {
            cout << "Invalid choice.\n";
        }
    }

    save_books();
    save_students();

    return 0;
}

void clear_input_buffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void load_books() {
    ifstream fin("books.txt");
    if (!fin) {
        return;
    }

    book_count = 0;
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        // The format is: "1. Title by Author (Status)"
        size_t dot_pos = line.find('.');
        if (dot_pos == string::npos) continue;

        // Extract title and author
        size_t by_pos = line.find(" by ", dot_pos);
        if (by_pos == string::npos) continue;

        size_t status_pos = line.find(" (", by_pos);
        if (status_pos == string::npos) continue;

        string title = line.substr(dot_pos + 2, by_pos - (dot_pos + 2));
        string author = line.substr(by_pos + 4, status_pos - (by_pos + 4));

        // Extract status
        string status = line.substr(status_pos + 2);
        status = status.substr(0, status.length() - 1); // remove closing )

        // Set book properties
        strncpy(books[book_count].title, title.c_str(), MAX_NAME_LEN - 1);
        books[book_count].title[MAX_NAME_LEN - 1] = '\0';

        strncpy(books[book_count].author, author.c_str(), MAX_NAME_LEN - 1);
        books[book_count].author[MAX_NAME_LEN - 1] = '\0';

        books[book_count].isbn = book_count + 1000; // Starting ISBN from 1000
        books[book_count].available = (status.find("Available") != string::npos);
        books[book_count].borrowed_by_roll = -1;

        if (!books[book_count].available) {
            size_t borrowed_pos = status.find("Borrowed by: ");
            if (borrowed_pos != string::npos) {
                string borrower = status.substr(borrowed_pos + 13);
                // Find the student by name
                for (int i = 0; i < student_count; i++) {
                    if (strcmp(students[i].name, borrower.c_str()) == 0) {
                        books[book_count].borrowed_by_roll = students[i].roll;
                        break;
                    }
                }
            }
        }

        book_count++;
        if (book_count >= MAX_BOOKS) break;
    }
    fin.close();
}

void save_books() {
    ofstream fout("books.txt");
    if (!fout) {
        cout << "Error saving book data!\n";
        return;
    }

    for (int i = 0; i < book_count; i++) {
        fout << i+1 << ". " << books[i].title << " by " << books[i].author;

        if (books[i].available) {
            fout << " (Available)";
        } else {
            int sidx = find_student(books[i].borrowed_by_roll);
            if (sidx != -1) {
                fout << " (Borrowed by: " << students[sidx].name << ")";
            } else {
                fout << " (Borrowed by: Unknown)";
            }
        }

        if (i < book_count - 1) fout << "\n";
    }
    fout.close();
}

void load_students() {
    ifstream fin("students.txt");
    if (!fin) {
        return;
    }

    student_count = 0;
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        // The format is: "1. Name[ID]"
        size_t dot_pos = line.find('.');
        if (dot_pos == string::npos) continue;

        size_t bracket_pos = line.find('[', dot_pos);
        if (bracket_pos == string::npos) continue;

        size_t close_bracket = line.find(']', bracket_pos);
        if (close_bracket == string::npos) continue;

        string name = line.substr(dot_pos + 2, bracket_pos - (dot_pos + 2));
        string roll_str = line.substr(bracket_pos + 1, close_bracket - (bracket_pos + 1));

        // Set student properties
        strncpy(students[student_count].name, name.c_str(), MAX_NAME_LEN - 1);
        students[student_count].name[MAX_NAME_LEN - 1] = '\0';

        students[student_count].roll = stoi(roll_str);
        student_count++;
        if (student_count >= MAX_STUDENTS) break;
    }
    fin.close();
}

void save_students() {
    ofstream fout("students.txt");
    if (!fout) {
        cout << "Error saving student data!\n";
        return;
    }

    for (int i = 0; i < student_count; i++) {
        fout << i+1 << ". " << students[i].name << "[" << students[i].roll << "]";
        if (i < student_count - 1) fout << "\n";
    }
    fout.close();
}

void add_book() {
    if (book_count >= MAX_BOOKS) {
        cout << "Book limit reached!\n";
        return;
    }

    cout << "Enter Book ISBN: ";
    if (!(cin >> books[book_count].isbn)) {
        clear_input_buffer();
        cout << "Invalid ISBN. Please enter a number.\n";
        return;
    }

    // Check if ISBN already exists
    if (find_book(books[book_count].isbn) != -1) {
        cout << "A book with this ISBN already exists!\n";
        return;
    }

    cin.ignore();
    cout << "Enter Book Title: ";
    cin.getline(books[book_count].title, MAX_NAME_LEN);

    cout << "Enter Book Author: ";
    cin.getline(books[book_count].author, MAX_NAME_LEN);

    books[book_count].available = true;
    books[book_count].borrowed_by_roll = -1;
    book_count++;

    cout << "Book added successfully!\n";
    save_books();
}

void view_books() {
    if (book_count == 0) {
        cout << "No books in the library.\n";
        return;
    }

    cout << "\n--- List of Books ---\n";
    for (int i = 0; i < book_count; i++) {
        cout << "ISBN: " << books[i].isbn << " - " << books[i].title << " by " << books[i].author;

        if (books[i].available) {
            cout << " (Available)";
        } else {
            int sidx = find_student(books[i].borrowed_by_roll);
            if (sidx != -1) {
                cout << " (Borrowed by: " << students[sidx].name << ")";
            } else {
                cout << " (Borrowed by: Unknown)";
            }
        }
        cout << "\n";
    }
}

void create_account() {
    if (student_count >= MAX_STUDENTS) {
        cout << "Student limit reached!\n";
        return;
    }

    cout << "Enter Roll Number: ";
    if (!(cin >> students[student_count].roll)) {
        clear_input_buffer();
        cout << "Invalid roll number. Please enter a number.\n";
        return;
    }

    // Check if roll number already exists
    if (find_student(students[student_count].roll) != -1) {
        cout << "A student with this roll number already exists!\n";
        return;
    }

    cin.ignore();
    cout << "Enter Name: ";
    cin.getline(students[student_count].name, MAX_NAME_LEN);

    student_count++;

    cout << "Account created successfully!\n";
    save_students();
}

void view_students() {
    if (student_count == 0) {
        cout << "No students registered.\n";
        return;
    }

    cout << "\n--- List of Students ---\n";
    for (int i = 0; i < student_count; i++) {
        cout << students[i].name << "[" << students[i].roll << "]\n";
        cout << "   Borrowed Books: ";

        bool has_books = false;
        for (int j = 0; j < book_count; j++) {
            if (!books[j].available && books[j].borrowed_by_roll == students[i].roll) {
                if (has_books) cout << ", ";
                cout << books[j].title;
                has_books = true;
            }
        }

        if (!has_books) cout << "None";
        cout << "\n";
    }
}

void view_borrowed_books() {
    bool any_borrowed = false;

    cout << "\n--- Borrowed Books ---\n";
    for (int i = 0; i < book_count; i++) {
        if (!books[i].available) {
            cout << "ISBN: " << books[i].isbn << " - " << books[i].title << " by " << books[i].author;

            int sidx = find_student(books[i].borrowed_by_roll);
            if (sidx != -1) {
                cout << " (Borrowed by: " << students[sidx].name << "[" << students[sidx].roll << "])\n";
            } else {
                cout << " (Borrowed by: Unknown)\n";
            }

            any_borrowed = true;
        }
    }

    if (!any_borrowed) {
        cout << "No books are currently borrowed.\n";
    }
}

void borrow_book(int roll) {
    int index = find_student(roll);
    if (index == -1) {
        cout << "Student not found!\n";
        return;
    }

    bool any_available = false;
    cout << "\n--- Available Books ---\n";
    for (int i = 0; i < book_count; i++) {
        if (books[i].available) {
            cout << "ISBN: " << books[i].isbn << " - " << books[i].title << " by " << books[i].author << "\n";
            any_available = true;
        }
    }

    if (!any_available) {
        cout << "No books available to borrow.\n";
        return;
    }

    int isbn;
    cout << "Enter ISBN of the book you want to borrow (or 0 to go back): ";
    if (!(cin >> isbn)) {
        clear_input_buffer();
        cout << "Invalid ISBN. Please enter a number.\n";
        return;
    }

    if (isbn == 0) return;

    int book_idx = find_book(isbn);
    if (book_idx == -1) {
        cout << "Book not found!\n";
        return;
    }

    if (!books[book_idx].available) {
        cout << "Sorry, this book is not available right now.\n";
        return;
    }

    books[book_idx].available = false;
    books[book_idx].borrowed_by_roll = roll;
    cout << "Book issued successfully!\n";
    save_books();
}

void return_book(int roll) {
    bool any_borrowed = false;
    cout << "\n--- Books Borrowed by You ---\n";

    for (int i = 0; i < book_count; i++) {
        if (!books[i].available && books[i].borrowed_by_roll == roll) {
            cout << "ISBN: " << books[i].isbn << " - " << books[i].title << " by " << books[i].author << "\n";
            any_borrowed = true;
        }
    }

    if (!any_borrowed) {
        cout << "You haven't borrowed any books.\n";
        return;
    }

    int isbn;
    cout << "Enter ISBN of the book you want to return (or 0 to go back): ";
    if (!(cin >> isbn)) {
        clear_input_buffer();
        cout << "Invalid ISBN. Please enter a number.\n";
        return;
    }

    if (isbn == 0) return;

    int book_idx = find_book(isbn);
    if (book_idx == -1) {
        cout << "Book not found!\n";
        return;
    }

    if (books[book_idx].available || books[book_idx].borrowed_by_roll != roll) {
        cout << "This book isn't currently borrowed by you.\n";
        return;
    }

    books[book_idx].available = true;
    books[book_idx].borrowed_by_roll = -1;
    cout << "Book returned successfully!\n";
    save_books();
}

int find_student(int roll) {
    for (int i = 0; i < student_count; i++) {
        if (students[i].roll == roll) return i;
    }
    return -1;
}

int find_book(int isbn) {
    for (int i = 0; i < book_count; i++) {
        if (books[i].isbn == isbn) return i;
    }
    return -1;
}
