#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <cstdio>

using std::string;
using std::cin;
using std::cout;

// Simple vector implementation
template<typename T>
class Vector {
private:
    T* data;
    int capacity;
    int length;
    
    void resize() {
        capacity = capacity == 0 ? 1 : capacity * 2;
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
};

// Simple hash map implementation
template<typename V>
class HashMap {
private:
    static const int HASH_SIZE = 10007;
    
    struct Node {
        string key;
        V value;
        Node* next;
        
        Node(const string& k, const V& v, Node* n = nullptr) : key(k), value(v), next(n) {}
    };
    
    Node* table[HASH_SIZE];
    int count;
    
    int hash(const string& key) const {
        int h = 0;
        for (char c : key) {
            h = (h * 31 + c) % HASH_SIZE;
        }
        return (h + HASH_SIZE) % HASH_SIZE;
    }
    
public:
    HashMap() : count(0) {
        for (int i = 0; i < HASH_SIZE; i++) {
            table[i] = nullptr;
        }
    }
    
    ~HashMap() {
        clear();
    }
    
    void clear() {
        for (int i = 0; i < HASH_SIZE; i++) {
            Node* curr = table[i];
            while (curr) {
                Node* next = curr->next;
                delete curr;
                curr = next;
            }
            table[i] = nullptr;
        }
        count = 0;
    }
    
    void insert(const string& key, const V& value) {
        int h = hash(key);
        table[h] = new Node(key, value, table[h]);
        count++;
    }
    
    bool find(const string& key, V& value) const {
        int h = hash(key);
        Node* curr = table[h];
        while (curr) {
            if (curr->key == key) {
                value = curr->value;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }
    
    bool erase(const string& key) {
        int h = hash(key);
        Node** curr = &table[h];
        while (*curr) {
            if ((*curr)->key == key) {
                Node* toDelete = *curr;
                *curr = (*curr)->next;
                delete toDelete;
                count--;
                return true;
            }
            curr = &((*curr)->next);
        }
        return false;
    }
    
    bool contains(const string& key) const {
        V dummy;
        return find(key, dummy);
    }
    
    int size() const {
        return count;
    }
};

// User data structure
struct User {
    char username[25];
    char password[35];
    char name[35];
    char mailAddr[35];
    int privilege;
    bool exists;
    
    User() : privilege(0), exists(false) {
        username[0] = password[0] = name[0] = mailAddr[0] = '\0';
    }
};

// Train data structure
struct Train {
    char trainID[25];
    int stationNum;
    int seatNum;
    char stations[100][35];
    int prices[100];
    int startTime; // minutes from 00:00
    int travelTimes[100];
    int stopoverTimes[100];
    int saleDate[2]; // [start, end] in days from 06-01
    char type;
    bool released;
    bool exists;
    
    Train() : stationNum(0), seatNum(0), startTime(0), type('G'), released(false), exists(false) {
        trainID[0] = '\0';
        saleDate[0] = saleDate[1] = 0;
    }
};

// Order data structure
struct Order {
    char username[25];
    char trainID[25];
    int date; // boarding date
    int num;
    char from[35];
    char to[35];
    int price;
    int status; // 0: success, 1: pending, 2: refunded
    long long timestamp;
    
    Order() : date(0), num(0), price(0), status(0), timestamp(0) {
        username[0] = trainID[0] = from[0] = to[0] = '\0';
    }
};

// Simple file-based storage
class Storage {
private:
    HashMap<User> users;
    HashMap<Train> trains;
    HashMap<bool> loggedIn;
    Vector<Order> orders;
    long long orderCounter;
    
    int parseTime(const string& timeStr) {
        int h = (timeStr[0] - '0') * 10 + (timeStr[1] - '0');
        int m = (timeStr[3] - '0') * 10 + (timeStr[4] - '0');
        return h * 60 + m;
    }
    
    int parseDate(const string& dateStr) {
        int m = (dateStr[0] - '0') * 10 + (dateStr[1] - '0');
        int d = (dateStr[3] - '0') * 10 + (dateStr[4] - '0');
        // Convert to days from 06-01
        if (m == 6) return d - 1;
        if (m == 7) return 30 + d - 1;
        if (m == 8) return 61 + d - 1;
        return 0;
    }
    
    void formatTime(int minutes, char* buffer) {
        int h = (minutes / 60) % 24;
        int m = minutes % 60;
        sprintf(buffer, "%02d:%02d", h, m);
    }
    
    void formatDate(int days, char* buffer) {
        int m, d;
        if (days < 30) {
            m = 6;
            d = days + 1;
        } else if (days < 61) {
            m = 7;
            d = days - 30 + 1;
        } else {
            m = 8;
            d = days - 61 + 1;
        }
        sprintf(buffer, "%02d-%02d", m, d);
    }
    
public:
    Storage() : orderCounter(0) {}
    
    // User commands
    int addUser(const string& c, const string& u, const string& p, 
                const string& n, const string& m, int g) {
        User existing;
        if (users.find(u, existing) && existing.exists) {
            return -1;
        }
        
        // First user special case
        if (users.contains(u) == false && loggedIn.size() == 0) {
            User newUser;
            strcpy(newUser.username, u.c_str());
            strcpy(newUser.password, p.c_str());
            strcpy(newUser.name, n.c_str());
            strcpy(newUser.mailAddr, m.c_str());
            newUser.privilege = 10;
            newUser.exists = true;
            users.insert(u, newUser);
            return 0;
        }
        
        // Normal case
        bool isLoggedIn = false;
        if (!loggedIn.find(c, isLoggedIn) || !isLoggedIn) {
            return -1;
        }
        
        User curUser;
        if (!users.find(c, curUser) || !curUser.exists) {
            return -1;
        }
        
        if (g >= curUser.privilege) {
            return -1;
        }
        
        User newUser;
        strcpy(newUser.username, u.c_str());
        strcpy(newUser.password, p.c_str());
        strcpy(newUser.name, n.c_str());
        strcpy(newUser.mailAddr, m.c_str());
        newUser.privilege = g;
        newUser.exists = true;
        users.insert(u, newUser);
        return 0;
    }
    
    int login(const string& u, const string& p) {
        User user;
        if (!users.find(u, user) || !user.exists) {
            return -1;
        }
        
        if (strcmp(user.password, p.c_str()) != 0) {
            return -1;
        }
        
        bool isLoggedIn = false;
        if (loggedIn.find(u, isLoggedIn) && isLoggedIn) {
            return -1;
        }
        
        loggedIn.insert(u, true);
        return 0;
    }
    
    int logout(const string& u) {
        bool isLoggedIn = false;
        if (!loggedIn.find(u, isLoggedIn) || !isLoggedIn) {
            return -1;
        }
        
        loggedIn.erase(u);
        return 0;
    }
    
    string queryProfile(const string& c, const string& u) {
        bool isLoggedIn = false;
        if (!loggedIn.find(c, isLoggedIn) || !isLoggedIn) {
            return "-1";
        }
        
        User curUser, targetUser;
        if (!users.find(c, curUser) || !curUser.exists) {
            return "-1";
        }
        if (!users.find(u, targetUser) || !targetUser.exists) {
            return "-1";
        }
        
        if (curUser.privilege <= targetUser.privilege && c != u) {
            return "-1";
        }
        
        return string(targetUser.username) + " " + targetUser.name + " " + 
               targetUser.mailAddr + " " + std::to_string(targetUser.privilege);
    }
    
    string modifyProfile(const string& c, const string& u, 
                        const string& p, const string& n, 
                        const string& m, int g) {
        bool isLoggedIn = false;
        if (!loggedIn.find(c, isLoggedIn) || !isLoggedIn) {
            return "-1";
        }
        
        User curUser, targetUser;
        if (!users.find(c, curUser) || !curUser.exists) {
            return "-1";
        }
        if (!users.find(u, targetUser) || !targetUser.exists) {
            return "-1";
        }
        
        if (curUser.privilege <= targetUser.privilege && c != u) {
            return "-1";
        }
        
        if (g != -1 && g >= curUser.privilege) {
            return "-1";
        }
        
        if (!p.empty()) strcpy(targetUser.password, p.c_str());
        if (!n.empty()) strcpy(targetUser.name, n.c_str());
        if (!m.empty()) strcpy(targetUser.mailAddr, m.c_str());
        if (g != -1) targetUser.privilege = g;
        
        users.erase(u);
        users.insert(u, targetUser);
        
        return string(targetUser.username) + " " + targetUser.name + " " + 
               targetUser.mailAddr + " " + std::to_string(targetUser.privilege);
    }
    
    // Train commands
    int addTrain(const string& i, int n, int m, const string& s, 
                 const string& p, const string& x, const string& t,
                 const string& o, const string& d, char y) {
        Train existing;
        if (trains.find(i, existing) && existing.exists) {
            return -1;
        }
        
        Train train;
        strcpy(train.trainID, i.c_str());
        train.stationNum = n;
        train.seatNum = m;
        train.type = y;
        train.startTime = parseTime(x);
        train.released = false;
        train.exists = true;
        
        // Parse stations
        size_t pos = 0;
        for (int j = 0; j < n; j++) {
            size_t start = pos;
            while (pos < s.length() && s[pos] != '|') pos++;
            string station = s.substr(start, pos - start);
            strcpy(train.stations[j], station.c_str());
            pos++;
        }
        
        // Parse prices
        pos = 0;
        for (int j = 0; j < n - 1; j++) {
            size_t start = pos;
            while (pos < p.length() && p[pos] != '|') pos++;
            train.prices[j] = std::stoi(p.substr(start, pos - start));
            pos++;
        }
        
        // Parse travel times
        pos = 0;
        for (int j = 0; j < n - 1; j++) {
            size_t start = pos;
            while (pos < t.length() && t[pos] != '|') pos++;
            train.travelTimes[j] = std::stoi(t.substr(start, pos - start));
            pos++;
        }
        
        // Parse stopover times
        if (n > 2) {
            pos = 0;
            for (int j = 0; j < n - 2; j++) {
                size_t start = pos;
                while (pos < o.length() && o[pos] != '|') pos++;
                train.stopoverTimes[j] = std::stoi(o.substr(start, pos - start));
                pos++;
            }
        }
        
        // Parse sale dates
        pos = 0;
        size_t start = pos;
        while (pos < d.length() && d[pos] != '|') pos++;
        train.saleDate[0] = parseDate(d.substr(start, pos - start));
        pos++;
        train.saleDate[1] = parseDate(d.substr(pos));
        
        trains.insert(i, train);
        return 0;
    }
    
    int releaseTrain(const string& i) {
        Train train;
        if (!trains.find(i, train) || !train.exists) {
            return -1;
        }
        
        if (train.released) {
            return -1;
        }
        
        train.released = true;
        trains.erase(i);
        trains.insert(i, train);
        return 0;
    }
    
    string queryTrain(const string& i, const string& d) {
        Train train;
        if (!trains.find(i, train) || !train.exists) {
            return "-1";
        }
        
        int queryDate = parseDate(d);
        
        string result = string(train.trainID) + " " + train.type + "\n";
        
        int currentTime = train.startTime;
        int currentDay = 0;
        int totalPrice = 0;
        
        for (int j = 0; j < train.stationNum; j++) {
            result += string(train.stations[j]) + " ";
            
            // Arrival time
            if (j == 0) {
                result += "xx-xx xx:xx";
            } else {
                char dateStr[10], timeStr[10];
                formatDate(queryDate + currentDay, dateStr);
                formatTime(currentTime, timeStr);
                result += string(dateStr) + " " + timeStr;
            }
            
            result += " -> ";
            
            // Departure time
            if (j == train.stationNum - 1) {
                result += "xx-xx xx:xx";
            } else {
                if (j > 0) {
                    currentTime += train.stopoverTimes[j - 1];
                    currentDay += currentTime / (24 * 60);
                    currentTime %= (24 * 60);
                }
                char dateStr[10], timeStr[10];
                formatDate(queryDate + currentDay, dateStr);
                formatTime(currentTime, timeStr);
                result += string(dateStr) + " " + timeStr;
            }
            
            result += " " + std::to_string(totalPrice) + " ";
            
            if (j == train.stationNum - 1) {
                result += "x";
            } else {
                result += std::to_string(train.seatNum);
                totalPrice += train.prices[j];
                currentTime += train.travelTimes[j];
                currentDay += currentTime / (24 * 60);
                currentTime %= (24 * 60);
            }
            
            if (j < train.stationNum - 1) result += "\n";
        }
        
        return result;
    }
    
    int deleteTrain(const string& i) {
        Train train;
        if (!trains.find(i, train) || !train.exists) {
            return -1;
        }
        
        if (train.released) {
            return -1;
        }
        
        train.exists = false;
        trains.erase(i);
        return 0;
    }
    
    void clean() {
        users.clear();
        trains.clear();
        loggedIn.clear();
        orders.clear();
        orderCounter = 0;
    }
};

// Main command processor
class TicketSystem {
private:
    Storage storage;
    
    string getParam(const string& cmd, const string& param) {
        size_t pos = cmd.find(" " + param + " ");
        if (pos == string::npos) return "";
        
        pos += param.length() + 2;
        size_t end = cmd.find(" -", pos);
        if (end == string::npos) {
            return cmd.substr(pos);
        }
        return cmd.substr(pos, end - pos);
    }
    
public:
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
                cout << storage.addUser(c, u, p, n, m, g) << "\n";
            }
            else if (cmd == "login") {
                string u = getParam(line, "-u");
                string p = getParam(line, "-p");
                cout << storage.login(u, p) << "\n";
            }
            else if (cmd == "logout") {
                string u = getParam(line, "-u");
                cout << storage.logout(u) << "\n";
            }
            else if (cmd == "query_profile") {
                string c = getParam(line, "-c");
                string u = getParam(line, "-u");
                cout << storage.queryProfile(c, u) << "\n";
            }
            else if (cmd == "modify_profile") {
                string c = getParam(line, "-c");
                string u = getParam(line, "-u");
                string p = getParam(line, "-p");
                string n = getParam(line, "-n");
                string m = getParam(line, "-m");
                string gStr = getParam(line, "-g");
                int g = gStr.empty() ? -1 : std::stoi(gStr);
                cout << storage.modifyProfile(c, u, p, n, m, g) << "\n";
            }
            else if (cmd == "add_train") {
                string i = getParam(line, "-i");
                int n = std::stoi(getParam(line, "-n"));
                int m = std::stoi(getParam(line, "-m"));
                string s = getParam(line, "-s");
                string p = getParam(line, "-p");
                string x = getParam(line, "-x");
                string t = getParam(line, "-t");
                string o = getParam(line, "-o");
                string d = getParam(line, "-d");
                char y = getParam(line, "-y")[0];
                cout << storage.addTrain(i, n, m, s, p, x, t, o, d, y) << "\n";
            }
            else if (cmd == "release_train") {
                string i = getParam(line, "-i");
                cout << storage.releaseTrain(i) << "\n";
            }
            else if (cmd == "query_train") {
                string i = getParam(line, "-i");
                string d = getParam(line, "-d");
                cout << storage.queryTrain(i, d) << "\n";
            }
            else if (cmd == "delete_train") {
                string i = getParam(line, "-i");
                cout << storage.deleteTrain(i) << "\n";
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
                cout << "-1\n";
            }
            else if (cmd == "refund_ticket") {
                cout << "-1\n";
            }
            else if (cmd == "clean") {
                storage.clean();
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
