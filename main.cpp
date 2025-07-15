#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include<conio.h>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "#Ananya_gupta123";
const string dbname = "bugtrackerdb";

class Database {
public:
    sql::Driver* driver = nullptr;
    sql::Connection* con = nullptr;

    Database() {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
        initialize();
    }

    void initialize() {
        sql::Statement* stmt = con->createStatement();
        stmt->execute("CREATE DATABASE IF NOT EXISTS " + dbname);
        con->setSchema(dbname);

        stmt->execute("CREATE TABLE IF NOT EXISTS users (username VARCHAR(50) PRIMARY KEY, password VARCHAR(50))");

        stmt->execute("CREATE TABLE IF NOT EXISTS bugs ("
            "id INT AUTO_INCREMENT PRIMARY KEY,"
            "description TEXT,"
            "status VARCHAR(10),"
            "timestamp VARCHAR(100))");

        delete stmt;
    }

    ~Database() {
        delete con;
    }
};

class UserManager {
    Database* db;

public:
    UserManager(Database* database) : db(database) {}

    bool login(const string& uname, const string& pword) {
        sql::PreparedStatement* pstmt = db->con->prepareStatement(
            "SELECT * FROM users WHERE username=? AND password=?"
        );
        pstmt->setString(1, uname);
        pstmt->setString(2, pword);
        sql::ResultSet* res = pstmt->executeQuery();
        bool success = res->next();
        delete res;
        delete pstmt;
        return success;
    }

    bool signUp(const string& uname, const string& pword) {
        // Check if username exists
        sql::PreparedStatement* checkStmt = db->con->prepareStatement(
            "SELECT * FROM users WHERE username=?"
        );
        checkStmt->setString(1, uname);
        sql::ResultSet* res = checkStmt->executeQuery();
        if (res->next()) {
            delete res;
            delete checkStmt;
            return false; // Username already exists
        }
        delete res;
        delete checkStmt;

        sql::PreparedStatement* pstmt = db->con->prepareStatement(
            "INSERT INTO users(username, password) VALUES(?, ?)"
        );
        pstmt->setString(1, uname);
        pstmt->setString(2, pword);
        pstmt->execute();
        delete pstmt;
        return true;
    }
};

class BugTracker {
    Database* db;

    string getCurrentTime() {
        time_t now = time(nullptr);
        char buf[100];
        ctime_s(buf, sizeof(buf), &now);
        string timeStr(buf);
        timeStr.pop_back();
        return timeStr;
    }


public:
    BugTracker(Database* database) : db(database) {}

    void addBug(const string& desc) {
        sql::PreparedStatement* pstmt = db->con->prepareStatement(
            "INSERT INTO bugs(description, status, timestamp) VALUES (?, 'Open', ?)"
        );
        pstmt->setString(1, desc);
        pstmt->setString(2, getCurrentTime());
        pstmt->execute();
        delete pstmt;
        cout << "Bug added successfully!\n";
    }

    void viewBugs() {
        sql::Statement* stmt = db->con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT * FROM bugs");

        if (!res->next()) {
            cout << "No bugs reported.\n";
        }
        else {
            res->beforeFirst();
            while (res->next()) {
                cout << "ID: " << res->getInt("id") << endl;
                cout << "Description: " << res->getString("description") << endl;
                cout << "Status: " << res->getString("status") << endl;
                cout << "Reported on: " << res->getString("timestamp") << "\n";
            }
        }

        delete res;
        delete stmt;
    }


    void markBugFixed(int bugId) {
        sql::PreparedStatement* pstmt = db->con->prepareStatement(
            "UPDATE bugs SET status='Fixed' WHERE id=?"
        );
        pstmt->setInt(1, bugId);
        int updated = pstmt->executeUpdate();
        delete pstmt;

        if (updated)
            cout << "Bug marked as fixed!\n";
        else
            cout << "Bug ID not found.\n";
    }
};


string getHiddenPassword() {
    string password;
    char ch;
    while ((ch = _getch()) != '\r') {  // '\r' is Enter key
        if (ch == '\b') {  // Handle backspace
            if (!password.empty()) {
                cout << "\b \b";
                password.pop_back();
            }
        }
        else {
            password.push_back(ch);
            cout << '*';
        }
    }
    cout << endl;
    return password;
}


int main() {
    Database db;
    UserManager userManager(&db);
    string uname, pword;
    int choice;

    system("cls");
    cout << "___________________________________\n";
    cout << "|                                  |\n";
    cout << "|  1. Login                        |\n";
    cout << "|  2. Sign Up                      |\n";
    cout << "|__________________________________|\n\n";

    cout << "Enter choice: ";
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        cout << "Enter username: ";
        cin >> uname;
        cout << "Enter password: ";
        pword = getHiddenPassword();
        if (!userManager.login(uname, pword)) {
            cout << "Invalid credentials!\n";
            return 0;
        }
    }
    else if (choice == 2) {
        cout << "Enter username: ";
        cin >> uname;
        cout << "Enter password: ";
        cin >> pword;
        if (!userManager.signUp(uname, pword)) {
            cout << "Username already exists!\n";
            return 0;
        }
        cout << "Account created successfully!\n";
    }
    else {
        cout << "Invalid choice!\n";
        return 0;
    }

    BugTracker tracker(&db);
    while (true) {
        //system("cls");  // Clear screen at the start of each loop
        cout << "\n===== Bug Tracker Menu =====\n";
        cout << "1. Add Bug\n2. View Bugs\n3. Mark Bug as Fixed\n4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            system("cls");
            string desc;
            cout << "Enter bug description: ";
            getline(cin, desc);
            tracker.addBug(desc);
        }
        else if (choice == 2) {
            system("cls");
            tracker.viewBugs();
        }
        else if (choice == 3) {
            system("cls");
            int bugId;
            cout << "Enter bug ID to mark as fixed: ";
            cin >> bugId;
            tracker.markBugFixed(bugId);
        }
        else if (choice == 4) {
            break;
        }
        else {
            cout << "Invalid choice. Try again.\n";
        }

        cout << "\nPress Enter to return to menu...";
        cin.ignore();  // Wait for Enter key
    }


    return 0;
}
