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
    const char command_code;
};

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
    map<string, char> codes;
    map<char, string> types;
    int r[16] = {};

    //Заполняем мапы для быстрого нахождения кода или типа команды
    for (auto &i: Commands) {
        codes[i.command_name] = i.command_code;
        types[i.command_code] = i.command_type;
    }
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
    input_asm.seekg(0, ios_base::beg);

    for (int cur_ptr = 0; !input_asm.eof();) {
        string s;
        getline(input_asm, s);

        s = s.substr(0, max(s.find_first_of(';') + 1, s.length())); //удаляем комментарии

        vector<string> splitcommands = split(s, ' ');
        bool flag = false;
        for (auto &i:splitcommands) {
            if (i[i.length() - 1] == ':') {
                i.
            }
        }
        cur_ptr += flag;
    }
    input_asm.close();
    Program.clear();
    return 0;
}
