#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Function prototypes
void addBook();
void viewAllBooks();
void about();
void registerMember();
void adminPanel();
void viewAllMembers();

// Function definition
void viewAllMembers() {
    ifstream in("members.txt");
    if (!in) {
        cout << "\nNo members registered yet!\n";
        return;
    }
    cout << "\nALL REGISTERED MEMBERS\n";
    string memberEntry;
    bool isEmpty = true;

    while (getline(in, memberEntry)) {
        cout << memberEntry << endl;
        isEmpty = false;
    }

    if (isEmpty) {
        cout << "No members found in the system!\n";
    }
    in.close();
}

void adminPanel() {
    while (true) {
        cout << endl << "ADMIN MENU" << endl;
        cout << "1. Add a new book" << endl;
        cout << "2. View all books" << endl;
        cout << "3. View all members" << endl;  // New option
        cout << "4. Back to Main Menu" << endl;
        cout << "Enter your choice : ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                addBook();
                break;
            case 2:
                viewAllBooks();
                break;
            case 3:
                viewAllMembers();
                break;
            case 4:
                return; // Return to main menu
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    }
}

void registerMember() {
    cout << "\nSTUDENT REGISTRATION\n";
    string studentName, studentId;

    cout << "Enter your name: ";
    cin.ignore();
    getline(cin, studentName);

    cout << "Enter your ID: ";
    getline(cin, studentId);

    ofstream out("members.txt", ios::app);
    if (out) {
        out << studentName << "(ID:" << studentId << ")" << endl;
        cout << "\nRegistration successful! Welcome to KYAU Library, " << studentName << "!\n";
    } else {
        cout << "Error saving member information!\n";
    }
    out.close();
}

void addBook() {
    ofstream out("books.txt", ios::app);
    if (!out) {
        cout << "Error opening file!" << endl;
        return;
    }
    cout << "\nADD NEW BOOK" << endl;
    string bookName, authorName;
    cout << "Book name: ";
    cin.ignore();
    getline(cin, bookName);
    cout << "Author name: ";
    getline(cin, authorName);

    // Count existing books to get the next number
    ifstream in("books.txt");
    int count = 0;
    string line;
    while (getline(in, line)) {
        count++;
    }
    in.close();

    // Save book and author names
    out << (count+1) << ". " << bookName << " by " << authorName << endl;
    cout << "\nBook added successfully as: " << (count+1) << ". " << bookName << " by " << authorName << endl;
    out.close();
}

void viewAllBooks() {
    ifstream in("books.txt");
    if (!in) {
        cout << "\nNo books found in the library!\n";
        return;
    }
    cout << "\nALL BOOKS IN LIBRARY\n";
    string bookEntry;
    bool isEmpty = true;

    while (getline(in, bookEntry)) {
        cout << bookEntry << endl;
        isEmpty = false;
    }

    if (isEmpty) {
        cout << "The library is empty!\n";
    }
    in.close();
}

void about() {
    cout << "\n>>> ABOUT OUR TEAM <<<\n";
    cout << "This Library Management System was developed by: ";
    cout << "Team CodeRaiders from KYAU\n\n";
    cout << "Team Members:\n";
    cout << "1. Rahat\n";
    cout << "2. Rafi\n";
    cout << "3. Anu\n";
    cout << "4. Sojib\n";
    cout << "5. Shila\n";
    cout << "----------------------------------------\n";
    cout << "Innovating for better library experiences\n";
    cout << "Contact: coderaiders@kyau.edu\n";
    cout << "� 2025 Team CodeRaiders - All Rights Reserved\n";
    cout << "----------------------------------------\n";
}

// Main funtion
int main() {
    while (true) {
        cout << ">>> WELCOME TO KYAU LIBRARY <<<\n\n";
        cout << "1. Login as Admin\n";
        cout << "2. Be a Member\n";
        cout << "3. About Us\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";

        int mainChoice;
        cin >> mainChoice;

        switch (mainChoice) {
            case 1: {
                string pass;
                cout << "\nADMIN LOGIN\n";
                cout << "Enter Password: ";
                cin >> pass;

                if (pass == "kyau25") {
                    cout << "Login successful!\n";
                    adminPanel();
                } else {
                    cout << "Wrong password! Access denied.\n";
                }
                break;
            }
            case 2:
                registerMember();
                break;
            case 3:
                about();
                break;
            case 4:
                cout << "Goodbye! Thank you for using KYAU Library.\n";
                return 0;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    }
    return 0;
}
