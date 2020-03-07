#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

using namespace std;

struct commands {
    const string command_name;
    const string command_type;
    const char command_code;
};

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

vector<string> split(const string &s, char delim) {
    vector<string> tokens;
    string token;
    istringstream Stream(s);
    while (getline(Stream, token, delim)
    {
        tokens.push_back(token);
    }
    return tokens;
}


int main() {
    ifstream input_asm("input.fasm");
    vector <unsigned int> Program;
    Program.resize(1u<<20);
    string s;
    map<string, int> labels;
    map<string, char> codes;
    map<char, string> types;
    int r[16]={};
    //Заполняем мапы для быстрого нахождения кода или типа команды
    for (auto i: Commands) {
        codes[i.command_name] = i.command_code;
        types[i.command_code] = i.command_type;
    }

    for (int i = 0; input_asm.eof(); i++) {
        getline(input_asm, s);
        int doubledot = 0;
        find
        if (s[s.size()-1] == ':') {
            s.erase(s.size()-1);
            labels[s] = i;
        } else {

        }
    }
    

    input_asm.close();
    Program.clear();
    return 0;
}
