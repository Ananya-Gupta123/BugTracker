// #include <iostream>
// #include <vector>
// #include <string>
// #include <fstream>
// #include <ctime>
#include<bits/stdc++.h>
using namespace std;

class User {
public:
    string username;
    string password;

    User(string uname, string pword) {
        username = uname;
        password = pword;
    }
};

class UserManager {
private:
    vector<User> users;
    string userFile = "users.txt";

    void loadUsers() {
        ifstream file(userFile);
        if (file.is_open()) {
            string uname, pword;
            while (file >> uname >> pword) {
                users.push_back(User(uname, pword));
            }
            file.close();
        }
    }

    void saveUsers() {
        ofstream file(userFile);
        if (file.is_open()) {
            for (User &user : users) {
                file << user.username << " " << user.password << "\n";
            }
            file.close();
        }
    }

public:
    UserManager() {
        loadUsers();
    }

    bool login(string uname, string pword) {
        for (User &user : users) {
            if (user.username == uname && user.password == pword) {
                return true;
            }
        }
        return false;
    }

    bool signUp(string uname, string pword) {
        for (User &user : users) {
            if (user.username == uname) {
                return false; // Username already exists
            }
        }
        users.push_back(User(uname, pword));
        saveUsers();
        return true;
    }
};

class Bug {
public:
    int id;
    string description;
    string status; // "Open" or "Fixed"
    string timestamp;

    Bug(int bugId, string desc, string time) {
        id = bugId;
        description = desc;
        status = "Open";
        timestamp = time;
    }
};

class BugTracker {
private:
    vector<Bug> bugs;
    int nextId;

    string getCurrentTime() {
        time_t now = time(0);
        char* dt = ctime(&now);
        string timeStr(dt);
        timeStr.pop_back(); // Remove newline character
        return timeStr;
    }

public:
    BugTracker() {
        nextId = 1;
        loadFromFile();
    }

    void addBug(string desc) {
        string time = getCurrentTime();
        Bug newBug(nextId, desc, time);
        bugs.push_back(newBug);
        nextId++;
        saveToFile();
        cout << "Bug added successfully!\n";
    }

    void viewBugs() {
        if (bugs.empty()) {
            cout << "No bugs reported.\n";
            return;
        }
        for (size_t i = 0; i < bugs.size(); i++) {
            cout << "ID: " << bugs[i].id << ", Description: " << bugs[i].description
                 << ", Status: " << bugs[i].status << ", Reported on: " << bugs[i].timestamp << "\n";
        }
    }

    void markBugFixed(int bugId) {
        for (size_t i = 0; i < bugs.size(); i++) {
            if (bugs[i].id == bugId) {
                bugs[i].status = "Fixed";
                saveToFile();
                cout << "Bug marked as fixed!\n";
                return;
            }
        }
        cout << "Bug ID not found.\n";
    }

    void saveToFile() {
        ofstream file("bugs.txt");
        if (file.is_open()) {
            for (size_t i = 0; i < bugs.size(); i++) {
                file << bugs[i].id << "|" << bugs[i].description << "|" << bugs[i].status << "|" << bugs[i].timestamp << "\n";
            }
            file.close();
        }
    }

    void loadFromFile() {
        ifstream file("bugs.txt");
        if (file.is_open()) {
            int id;
            string desc, status, time;
            while (file >> id) {
                file.ignore();
                getline(file, desc, '|');
                getline(file, status, '|');
                getline(file, time);
                Bug bug(id, desc, time);
                bug.status = status;
                bugs.push_back(bug);
                if (id >= nextId) {
                    nextId = id + 1;
                }
            }
            file.close();
        }
    }
};

int main() {
    UserManager userManager;
    string uname, pword;
    int choice;
    system("cls");
    cout<< "___________________________________\n";
    cout<< "|                                  |\n";
    cout <<"|  1. Login                        | \n";
    cout <<"|  2. Sign Up                      |\n";
    cout<< "|__________________________________|";
    cout<< "\n\n";

    cout <<" Enter choice :  ";
    cin >> choice;
    cin.ignore();
    if (choice == 1) {
        cout << "Enter username: ";
        cin >> uname;
        cout << "Enter password: ";
        cin >> pword;
        if (!userManager.login(uname, pword)) {
            cout << "Invalid credentials!\n";
            return 0;
        }
    } else if (choice == 2) {
        cout << "Enter username: ";
        cin >> uname;
        cout << "Enter password: ";
        cin >> pword;
        if (!userManager.signUp(uname, pword)) {
            cout << "Username already exists!\n";
            return 0;
        }
        cout << "Account created successfully!\n";
    } else {
        cout << "Invalid choice!\n";
        return 0;
    }

    BugTracker tracker;
    while (true) {
        cout << "1. Add Bug\n2. View Bugs\n3. Mark Bug as Fixed\n4. Exit\nEnter choice: ";
        cin >> choice;
        cin.ignore();
        if (choice == 1) {
            string desc;
            cout << "Enter bug description: ";
            getline(cin, desc);
            tracker.addBug(desc);
        } else if (choice == 2) {
            tracker.viewBugs();
        } else if (choice == 3) {
            int bugId;
            cout << "Enter bug ID to mark as fixed: ";
            cin >> bugId;
            tracker.markBugFixed(bugId);
        } else if (choice == 4) {
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }
    return 0;
}
