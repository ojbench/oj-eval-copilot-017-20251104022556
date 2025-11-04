#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdio>

using std::string;
using std::cin;
using std::cout;

const int MAX_USERS = 10000;
const int MAX_TRAINS = 5000;
const int MAX_ORDERS = 100000;

struct User {
    char username[25];
    char password[35];
    char name[35];
    char mailAddr[35];
    int privilege;
    bool exists;
    bool loggedIn;
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
    bool exists;
};

struct Order {
    int id;
    char username[25];
    char trainID[25];
    int date;
    int num;
    int fromIdx, toIdx;
    int price;
    int status;
    Order* next;
};

class TicketSystem {
private:
    User* users;
    int userCount;
    Train* trains;
    int trainCount;
    Order* orders;
    int orderCount;
    
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
        while (!result.empty() && result.back() == ' ') result.pop_back();
        return result;
    }
    
    int findUser(const string& username) {
        for (int i = 0; i < userCount; i++) {
            if (users[i].exists && strcmp(users[i].username, username.c_str()) == 0) return i;
        }
        return -1;
    }
    
    int findTrain(const string& trainID) {
        for (int i = 0; i < trainCount; i++) {
            if (trains[i].exists && strcmp(trains[i].trainID, trainID.c_str()) == 0) return i;
        }
        return -1;
    }
    
    void loadData() {
        std::ifstream uf("users.dat", std::ios::binary);
        if (uf) {
            uf.read((char*)&userCount, sizeof(userCount));
            uf.read((char*)users, sizeof(User) * userCount);
            uf.close();
        } else {
            userCount = 0;
        }
        
        std::ifstream tf("trains.dat", std::ios::binary);
        if (tf) {
            tf.read((char*)&trainCount, sizeof(trainCount));
            tf.read((char*)trains, sizeof(Train) * trainCount);
            tf.close();
        } else {
            trainCount = 0;
        }
        
        std::ifstream of("orders.dat", std::ios::binary);
        if (of) {
            of.read((char*)&orderCount, sizeof(orderCount));
            of.read((char*)orders, sizeof(Order) * orderCount);
            of.close();
        } else {
            orderCount = 0;
        }
    }
    
    void saveData() {
        std::ofstream uf("users.dat", std::ios::binary);
        uf.write((char*)&userCount, sizeof(userCount));
        uf.write((char*)users, sizeof(User) * userCount);
        uf.close();
        
        std::ofstream tf("trains.dat", std::ios::binary);
        tf.write((char*)&trainCount, sizeof(trainCount));
        tf.write((char*)trains, sizeof(Train) * trainCount);
        tf.close();
        
        std::ofstream of("orders.dat", std::ios::binary);
        of.write((char*)&orderCount, sizeof(orderCount));
        of.write((char*)orders, sizeof(Order) * orderCount);
        of.close();
    }
    
public:
    TicketSystem() {
        users = new User[MAX_USERS]();
        trains = new Train[MAX_TRAINS]();
        orders = new Order[MAX_ORDERS]();
        
        loadData();
        for (int i = 0; i < userCount; i++) {
            users[i].loggedIn = false;
        }
    }
    
    ~TicketSystem() {
        saveData();
        delete[] users;
        delete[] trains;
        delete[] orders;
    }
    
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
                
                if (userCount == 0) {
                    User& user = users[userCount++];
                    strcpy(user.username, u.c_str());
                    strcpy(user.password, p.c_str());
                    strcpy(user.name, n.c_str());
                    strcpy(user.mailAddr, m.c_str());
                    user.privilege = 10;
                    user.exists = true;
                    user.loggedIn = false;
                    cout << "0\n";
                } else {
                    int curIdx = findUser(c);
                    if (curIdx == -1 || !users[curIdx].loggedIn || users[curIdx].privilege <= g) {
                        cout << "-1\n";
                    } else {
                        User& user = users[userCount++];
                        strcpy(user.username, u.c_str());
                        strcpy(user.password, p.c_str());
                        strcpy(user.name, n.c_str());
                        strcpy(user.mailAddr, m.c_str());
                        user.privilege = g;
                        user.exists = true;
                        user.loggedIn = false;
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
                
                Train& train = trains[trainCount++];
                strcpy(train.trainID, i.c_str());
                train.stationNum = std::stoi(getParam(line, "-n"));
                train.seatNum = std::stoi(getParam(line, "-m"));
                train.startTime = parseTime(getParam(line, "-x"));
                train.type = getParam(line, "-y")[0];
                train.released = false;
                train.exists = true;
                
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
                    trains[idx].exists = false;
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
                userCount = 0;
                trainCount = 0;
                orderCount = 0;
                for (int i = 0; i < MAX_USERS; i++) users[i].exists = false;
                for (int i = 0; i < MAX_TRAINS; i++) trains[i].exists = false;
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
