#include <iostream>
#include <cstring>
#include <string>

using namespace std;

template<typename T>
class Vector {
private:
    T* data;
    int capacity;
    int length;
    void resize() {
        capacity = capacity == 0 ? 4 : capacity * 2;
        T* newData = new T[capacity];
        for (int i = 0; i < length; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }
public:
    Vector() : data(nullptr), capacity(0), length(0) {}
    ~Vector() { delete[] data; }
    void push_back(const T& value) {
        if (length == capacity) resize();
        data[length++] = value;
    }
    T& operator[](int index) { return data[index]; }
    int size() const { return length; }
};

struct User {
    char username[25];
    char password[35];
    bool loggedIn;
    User() : loggedIn(false) {
        username[0] = password[0] = '\0';
    }
};

int main() {
    Vector<User> users;
    
    User u;
    strcpy(u.username, "alice");
    strcpy(u.password, "pass123456");
    users.push_back(u);
    
    cout << "Stored user: [" << users[0].username << "]" << endl;
    cout << "Stored pass: [" << users[0].password << "]" << endl;
    
    string search = "alice";
    int found = -1;
    for (int i = 0; i < users.size(); i++) {
        if (strcmp(users[i].username, search.c_str()) == 0) {
            found = i;
            break;
        }
    }
    
    cout << "Found at index: " << found << endl;
    
    if (found >= 0) {
        string p = "pass123456";
        cout << "Password match: " << (strcmp(users[found].password, p.c_str()) == 0) << endl;
    }
    
    return 0;
}
