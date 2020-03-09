#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

using namespace std;

typedef unsigned int T;
typedef unsigned char uchar;

struct commands {
    const string command_name;
    const string command_type;
    const T command_code;
};

void print_bin (T ans) {
    vector<uchar> bin;
    while (ans>0){
        bin.push_back(ans%2);
        ans/=2;
    }
    bin.resize(32);
    for (int i = 31; i>=0; --i) {
        cout<<(int)bin[i];
    }
    cout<<endl;
}

int put(vector<T> &Program, int index, uchar byte0, uchar byte1, uchar byte2, uchar byte3) {
    Program[index] = (byte3 << 24u) + (byte2 << 16u) + (byte1 << 8u) + byte0;
    return 0;
}

uchar readbyte(const vector<T> &Program, int index, T byte_index) {
    return (Program[index] << (byte_index * 8u)) & 255u;

}

vector<string> split(const string &s, char delim) {
    vector<string> tokens;
    string token = s;
    istringstream strstream(s);
    while (getline(strstream, token, delim)) {
        if (token.empty()) {
            continue;
        }
        tokens.push_back(token);
    }
    return tokens;
}

//удаляет пробелы по краям
void str_normalize(string &s) {
    unsigned long long left = s.find_first_not_of(' ');
    if (left == string::npos) {
        s.clear();
        return;
    }
    s.erase(0, left);
    unsigned long long right = s.find_last_not_of(' ') + 1;
    if (right == s.length()) {
        return;
    }
    s.erase(right);
}

int record_RM (vector<T>& Program, int address, const vector<string>& splitcommands, uchar code, const map<string, int>& labels) {
    T ans=code;
    auto registr = (uchar)stoi(splitcommands[1].substr(1, 2));
    T num;
    try {
        num =(T)stoi(splitcommands[2]);
    } catch(...) {
        num = labels.find(splitcommands[2])->second;
    }
    ans = (((ans<<4u) | registr)<<20u) | num;
    Program[address] = ans;
    //print_bin(ans);
    return 0;
}
int record_RR (vector<T>& Program, int address, const vector<string>& splitcommands, uchar code, const map<string, int>& labels) {
    T ans=code;
    auto registr1 = (uchar)stoi(splitcommands[1].substr(1, 2));
    auto registr2 = (uchar)stoi(splitcommands[2].substr(1, 2));
    T num;
    try {
        num =(T)stoi(splitcommands[3]);
    } catch(...) {
        num = labels.find(splitcommands[3])->second;
    }
    ans = (((((ans<<4u) | registr1)<<4u) | registr2)<<16u) | num;
    Program[address] = ans;
    //print_bin(ans);
    return 0;
}
int record_RI (vector<T>& Program, int address, const vector<string>& splitcommands, uchar code, const map<string, int>& labels) {
    T ans=code;
    auto registr = (uchar)stoi(splitcommands[1].substr(1, 8));
    T num;
    try {
        num =(T)stoi(splitcommands[2]);
    } catch(...) {
        num = labels.find(splitcommands[2])->second;
    }
    ans = (((ans<<4u) | registr)<<20u) | num;
    Program[address] = ans;
   // print_bin(ans);
    return 0;
}
int record_J (vector<T>& Program, int address, const vector<string>& splitcommands, uchar code, const map<string, int>& labels) {
    T ans=code;
    T num;
    try {
        num =(T)stoi(splitcommands[1]);
    } catch(...) {
        num = labels.find(splitcommands[1])->second;
    }
    ans = (ans<<24u) + num;
    Program[address] = ans;
    //print_bin(ans);
    return 0;
}

int main() {
    const struct commands Commands[100] = {{"halt",    "RI", 0},
                                           {"syscall", "RI", 1},
                                           {"add",     "RR", 2},
                                           {"addi",    "RI", 3},
                                           {"sub",     "RR", 4},
                                           {"subi",    "RI", 5},
                                           {"mul",     "RR", 6},
                                           {"muli",    "RI", 7},
                                           {"div",     "RR", 8},
                                           {"divi",    "RI", 9},
                                           {"lc",      "RI", 12},
                                           {"shl",     "RR", 13},
                                           {"shli",    "RI", 14},
                                           {"shr",     "RR", 15},
                                           {"shri",    "RI", 16},
                                           {"and",     "RR", 17},
                                           {"andi",    "RI", 18},
                                           {"or",      "RR", 19},
                                           {"ori",     "RI", 20},
                                           {"xor",     "RR", 21},
                                           {"xori",    "RI", 22},
                                           {"not",     "RI", 23},
                                           {"mov",     "RR", 24},
                                           {"addd",    "RR", 32},
                                           {"subd",    "RR", 33},
                                           {"muld",    "RR", 34},
                                           {"divd",    "RR", 35},
                                           {"itod",    "RR", 36},
                                           {"dtoi",    "RR", 37},
                                           {"push",    "RI", 38},
                                           {"pop",     "RI", 39},
                                           {"call",    "RR", 40},
                                           {"calli",   "J",  41},
                                           {"ret",     "RI", 42},
                                           {"cmp",     "RR", 43},
                                           {"cmpi",    "RI", 44},
                                           {"cmpd",    "RR", 45},
                                           {"jmp",     "J",  46},
                                           {"jne",     "J",  47},
                                           {"jeq",     "J",  48},
                                           {"jle",     "J",  49},
                                           {"jl",      "J",  50},
                                           {"jge",     "J",  51},
                                           {"jg",      "J",  52},
                                           {"load",    "RM", 64},
                                           {"store",   "RM", 65},
                                           {"load2",   "RM", 66},
                                           {"store2",  "RM", 67},
                                           {"loadr",   "RR", 68},
                                           {"loadr2",  "RR", 69},
                                           {"storer",  "RR", 70},
                                           {"storer2", "RR", 71}};

    ifstream input_asm("input.fasm");
    vector<T> Program;
    Program.resize((1u) << 20u);
    map<string, int> labels;
    map<string, T> codes;
    map<T, string> types;
    int r[16] = {};

    //Заполняем мапы для быстрого нахождения кода или типа команды
    for (auto &i: Commands) {
        codes[i.command_name] = i.command_code;
        types[i.command_code] = i.command_type;
    }
    //первый проход
    for (int cur_ptr = 0; !input_asm.eof();) {
        string s;
        getline(input_asm, s);

        s = s.substr(0, max(s.find_first_of(';') + 1, s.length())); //удаляем комментарии

        vector<string> splitcommands = split(s, ' ');
        bool flag = false;
        for (auto &i:splitcommands) {
            if (i[i.length() - 1] == ':') {
                i.erase(i.length() - 1);
                labels[i] = cur_ptr;
            } else {
                flag = true;
            }
        }
        cur_ptr += flag;
    }

    //второй проход
    input_asm.seekg(0, ios_base::beg);
    for (int cur_ptr = 0; !input_asm.eof();) {
        string s;
        getline(input_asm, s);

        s = s.substr(0, s.find_first_of(';')); //удаляем комментарии

        vector<string> splitcommands = split(s, ' ');
        for (int i = 0; i < splitcommands.size(); ++i) {
            if (splitcommands[i][splitcommands[i].size()-1] == ':') {
                splitcommands.erase(splitcommands.begin()+i);
                i--;
            }
        }
        if (splitcommands.empty()) {
            continue;
        }
        
        int cmd_code = codes[splitcommands[0]];
        string cmd_type = types[cmd_code];
        if (cmd_type == "RM") {
            record_RM(Program, cur_ptr, splitcommands,  cmd_code, labels);
        } else if (cmd_type == "RR") {
            record_RR(Program, cur_ptr, splitcommands,  cmd_code, labels);
        } else if (cmd_type == "RI") {
            record_RI(Program, cur_ptr, splitcommands,  cmd_code, labels);
        } else if (cmd_type == "J") {
            record_J(Program, cur_ptr, splitcommands,  cmd_code, labels);
        } else if (splitcommands[0] == "end"){
            Program[cur_ptr] = 0u;
        } else {
            cout<<"Compilation Failed"<<endl;
        }



        cout<<cur_ptr<<": ";
        for (auto& i: splitcommands) {
            cout<<i<<" ";
        }
        cout<<endl;

        cur_ptr += 1;
    }
    input_asm.close();

    for (int i = 0; i<Program.size(); i++) {
        if (Program[i] == 0) {
            continue;
        }
        cout.width(3);
        cout<<i<<": ";
        print_bin(Program[i]);
    }
    Program.clear();
    return 0;
}
