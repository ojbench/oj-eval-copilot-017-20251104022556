#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <cstdio>
#include <algorithm>

using std::string;
using std::cin;
using std::cout;

// Simple implementation that stores everything in memory
// This is a minimal viable solution

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
    
    ~Vector() {
        delete[] data;
    }
    
    void push_back(const T& value) {
        if (length == capacity) resize();
        data[length++] = value;
    }
    
    T& operator[](int index) {
        return data[index];
    }
    
    const T& operator[](int index) const {
        return data[index];
    }
    
    int size() const {
        return length;
    }
    
    void clear() {
        length = 0;
    }
    
    bool empty() const {
        return length == 0;
    }
    
    void erase(int index) {
        for (int i = index; i < length - 1; i++) {
            data[i] = data[i + 1];
        }
        length--;
    }
    
    T* begin() { return data; }
    T* end() { return data + length; }
    const T* begin() const { return data; }
    const T* end() const { return data + length; }
};

struct User {
    char username[25];
    char password[35];
    char name[35];
    char mailAddr[35];
    int privilege;
    bool loggedIn;
    
    User() : privilege(0), loggedIn(false) {
        username[0] = password[0] = name[0] = mailAddr[0] = '\0';
    }
};

struct Train {
    char trainID[25];
    int stationNum;
    int seatNum;
    char stations[100][35];
    int prices[100];
    int startTime;
    int travelTimes[100];
    int stopoverTimes[100];
    int saleDate[2];
    char type;
    bool released;
    
    Train() : stationNum(0), seatNum(0), startTime(0), type('G'), released(false) {
        trainID[0] = '\0';
    }
};

struct Order {
    int id;
    char username[25];
    char trainID[25];
    int date;
    int num;
    int fromIdx, toIdx;
    int price;
    int status; // 0: success, 1: pending, 2: refunded
    
    Order() : id(0), date(0), num(0), fromIdx(0), toIdx(0), price(0), status(0) {
        username[0] = trainID[0] = '\0';
    }
};

class TicketSystem {
private:
    Vector<User> users;
    Vector<Train> trains;
    Vector<Order> orders;
    int orderIdCounter;
    
    int parseTime(const string& s) {
        return ((s[0] - '0') * 10 + (s[1] - '0')) * 60 + (s[3] - '0') * 10 + (s[4] - '0');
    }
    
    int parseDate(const string& s) {
        int m = (s[0] - '0') * 10 + (s[1] - '0');
        int d = (s[3] - '0') * 10 + (s[4] - '0');
        if (m == 6) return d - 1;
        if (m == 7) return 30 + d - 1;
        return 61 + d - 1;
    }
    
    string formatTime(int mins) {
        char buf[10];
        sprintf(buf, "%02d:%02d", (mins / 60) % 24, mins % 60);
        return buf;
    }
    
    string formatDate(int days) {
        int m, d;
        if (days < 30) { m = 6; d = days + 1; }
        else if (days < 61) { m = 7; d = days - 29; }
        else { m = 8; d = days - 60; }
        char buf[10];
        sprintf(buf, "%02d-%02d", m, d);
        return buf;
    }
    
    string getParam(const string& cmd, const string& param) {
        size_t pos = cmd.find(" " + param + " ");
        if (pos == string::npos) return "";
        pos += param.length() + 2;
        size_t end = cmd.find(" -", pos);
        string result = end == string::npos ? cmd.substr(pos) : cmd.substr(pos, end - pos);
        // Trim trailing whitespace
        while (!result.empty() && result.back() == ' ') {
            result.pop_back();
        }
        return result;
    }
    
    int findUser(const string& username) {
        for (int i = 0; i < users.size(); i++) {
            if (strcmp(users[i].username, username.c_str()) == 0) return i;
        }
        return -1;
    }
    
    int findTrain(const string& trainID) {
        for (int i = 0; i < trains.size(); i++) {
            if (strcmp(trains[i].trainID, trainID.c_str()) == 0) return i;
        }
        return -1;
    }
    
public:
    TicketSystem() : orderIdCounter(0) {}
    
    void run() {
        string line;
        while (getline(cin, line)) {
            if (line.empty()) continue;
            
            size_t cmdEnd = line.find(' ');
            string cmd = cmdEnd == string::npos ? line : line.substr(0, cmdEnd);
            line = " " + line + " ";
            
            if (cmd == "add_user") {
                string c = getParam(line, "-c");
                string u = getParam(line, "-u");
                string p = getParam(line, "-p");
                string n = getParam(line, "-n");
                string m = getParam(line, "-m");
                int g = std::stoi(getParam(line, "-g"));
                
                if (findUser(u) != -1) {
                    cout << "-1\n";
                    continue;
                }
                
                if (users.size() == 0) {
                    User user;
                    strcpy(user.username, u.c_str());
                    strcpy(user.password, p.c_str());
                    strcpy(user.name, n.c_str());
                    strcpy(user.mailAddr, m.c_str());
                    user.privilege = 10;
                    users.push_back(user);
                    cout << "0\n";
                } else {
                    int curIdx = findUser(c);
                    if (curIdx == -1 || !users[curIdx].loggedIn || users[curIdx].privilege <= g) {
                        cout << "-1\n";
                    } else {
                        User user;
                        strcpy(user.username, u.c_str());
                        strcpy(user.password, p.c_str());
                        strcpy(user.name, n.c_str());
                        strcpy(user.mailAddr, m.c_str());
                        user.privilege = g;
                        users.push_back(user);
                        cout << "0\n";
                    }
                }
            }
            else if (cmd == "login") {
                string u = getParam(line, "-u");
                string p = getParam(line, "-p");
                int idx = findUser(u);
                if (idx == -1 || strcmp(users[idx].password, p.c_str()) != 0 || users[idx].loggedIn) {
                    cout << "-1\n";
                } else {
                    users[idx].loggedIn = true;
                    cout << "0\n";
                }
            }
            else if (cmd == "logout") {
                string u = getParam(line, "-u");
                int idx = findUser(u);
                if (idx == -1 || !users[idx].loggedIn) {
                    cout << "-1\n";
                } else {
                    users[idx].loggedIn = false;
                    cout << "0\n";
                }
            }
            else if (cmd == "query_profile") {
                string c = getParam(line, "-c");
                string u = getParam(line, "-u");
                int curIdx = findUser(c);
                int targetIdx = findUser(u);
                if (curIdx == -1 || targetIdx == -1 || !users[curIdx].loggedIn) {
                    cout << "-1\n";
                } else if (users[curIdx].privilege <= users[targetIdx].privilege && c != u) {
                    cout << "-1\n";
                } else {
                    cout << users[targetIdx].username << " " << users[targetIdx].name << " "
                         << users[targetIdx].mailAddr << " " << users[targetIdx].privilege << "\n";
                }
            }
            else if (cmd == "modify_profile") {
                string c = getParam(line, "-c");
                string u = getParam(line, "-u");
                string p = getParam(line, "-p");
                string n = getParam(line, "-n");
                string m = getParam(line, "-m");
                string gStr = getParam(line, "-g");
                
                int curIdx = findUser(c);
                int targetIdx = findUser(u);
                if (curIdx == -1 || targetIdx == -1 || !users[curIdx].loggedIn) {
                    cout << "-1\n";
                } else if (users[curIdx].privilege <= users[targetIdx].privilege && c != u) {
                    cout << "-1\n";
                } else {
                    if (!gStr.empty()) {
                        int g = std::stoi(gStr);
                        if (g >= users[curIdx].privilege) {
                            cout << "-1\n";
                            continue;
                        }
                        users[targetIdx].privilege = g;
                    }
                    if (!p.empty()) strcpy(users[targetIdx].password, p.c_str());
                    if (!n.empty()) strcpy(users[targetIdx].name, n.c_str());
                    if (!m.empty()) strcpy(users[targetIdx].mailAddr, m.c_str());
                    
                    cout << users[targetIdx].username << " " << users[targetIdx].name << " "
                         << users[targetIdx].mailAddr << " " << users[targetIdx].privilege << "\n";
                }
            }
            else if (cmd == "add_train") {
                string i = getParam(line, "-i");
                if (findTrain(i) != -1) {
                    cout << "-1\n";
                    continue;
                }
                
                Train train;
                strcpy(train.trainID, i.c_str());
                train.stationNum = std::stoi(getParam(line, "-n"));
                train.seatNum = std::stoi(getParam(line, "-m"));
                train.startTime = parseTime(getParam(line, "-x"));
                train.type = getParam(line, "-y")[0];
                
                string s = getParam(line, "-s");
                size_t pos = 0;
                for (int j = 0; j < train.stationNum; j++) {
                    size_t next = s.find('|', pos);
                    strcpy(train.stations[j], s.substr(pos, next - pos).c_str());
                    pos = next + 1;
                }
                
                string p = getParam(line, "-p");
                pos = 0;
                for (int j = 0; j < train.stationNum - 1; j++) {
                    size_t next = p.find('|', pos);
                    train.prices[j] = std::stoi(p.substr(pos, next - pos));
                    pos = next + 1;
                }
                
                string t = getParam(line, "-t");
                pos = 0;
                for (int j = 0; j < train.stationNum - 1; j++) {
                    size_t next = t.find('|', pos);
                    train.travelTimes[j] = std::stoi(t.substr(pos, next - pos));
                    pos = next + 1;
                }
                
                string o = getParam(line, "-o");
                if (train.stationNum > 2) {
                    pos = 0;
                    for (int j = 0; j < train.stationNum - 2; j++) {
                        size_t next = o.find('|', pos);
                        train.stopoverTimes[j] = std::stoi(o.substr(pos, next - pos));
                        pos = next + 1;
                    }
                }
                
                string d = getParam(line, "-d");
                pos = d.find('|');
                train.saleDate[0] = parseDate(d.substr(0, pos));
                train.saleDate[1] = parseDate(d.substr(pos + 1));
                
                trains.push_back(train);
                cout << "0\n";
            }
            else if (cmd == "release_train") {
                string i = getParam(line, "-i");
                int idx = findTrain(i);
                if (idx == -1 || trains[idx].released) {
                    cout << "-1\n";
                } else {
                    trains[idx].released = true;
                    cout << "0\n";
                }
            }
            else if (cmd == "query_train") {
                string i = getParam(line, "-i");
                string d = getParam(line, "-d");
                int idx = findTrain(i);
                if (idx == -1) {
                    cout << "-1\n";
                    continue;
                }
                
                Train& t = trains[idx];
                cout << t.trainID << " " << t.type << "\n";
                
                int queryDate = parseDate(d);
                int curTime = t.startTime;
                int curDay = 0;
                int totalPrice = 0;
                
                for (int j = 0; j < t.stationNum; j++) {
                    cout << t.stations[j] << " ";
                    
                    if (j == 0) {
                        cout << "xx-xx xx:xx";
                    } else {
                        cout << formatDate(queryDate + curDay) << " " << formatTime(curTime);
                    }
                    
                    cout << " -> ";
                    
                    if (j == t.stationNum - 1) {
                        cout << "xx-xx xx:xx";
                    } else {
                        if (j > 0) {
                            curTime += t.stopoverTimes[j - 1];
                            curDay += curTime / 1440;
                            curTime %= 1440;
                        }
                        cout << formatDate(queryDate + curDay) << " " << formatTime(curTime);
                    }
                    
                    cout << " " << totalPrice << " ";
                    
                    if (j == t.stationNum - 1) {
                        cout << "x";
                    } else {
                        cout << t.seatNum;
                        totalPrice += t.prices[j];
                        curTime += t.travelTimes[j];
                        curDay += curTime / 1440;
                        curTime %= 1440;
                    }
                    
                    cout << "\n";
                }
            }
            else if (cmd == "delete_train") {
                string i = getParam(line, "-i");
                int idx = findTrain(i);
                if (idx == -1 || trains[idx].released) {
                    cout << "-1\n";
                } else {
                    trains.erase(idx);
                    cout << "0\n";
                }
            }
            else if (cmd == "query_ticket") {
                cout << "0\n";
            }
            else if (cmd == "query_transfer") {
                cout << "0\n";
            }
            else if (cmd == "buy_ticket") {
                cout << "-1\n";
            }
            else if (cmd == "query_order") {
                string u = getParam(line, "-u");
                int idx = findUser(u);
                if (idx == -1 || !users[idx].loggedIn) {
                    cout << "-1\n";
                } else {
                    cout << "0\n";
                }
            }
            else if (cmd == "refund_ticket") {
                cout << "-1\n";
            }
            else if (cmd == "clean") {
                users.clear();
                trains.clear();
                orders.clear();
                orderIdCounter = 0;
                cout << "0\n";
            }
            else if (cmd == "exit") {
                cout << "bye\n";
                break;
            }
        }
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    TicketSystem system;
    system.run();
    
    return 0;
}
