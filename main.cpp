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

class Fupm {
public:
    Fupm():codes(),types() {
        for (auto &i: Commands) {
            codes[i.command_name] = i.command_code;
            types[i.command_code] = i.command_type;
        }
        Program.resize((1u)<<20u);
    }
    ~Fupm()  {
        Program.clear();
    }

    void intialization(ifstream& input_asm);
    void record_line(const vector<string>& splitcommands, int address);

private:
    vector<T> Program;
    map<string, int> labels;
    int r[16]{};
    map<string, T> codes;
    map<T, string> types;
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

};

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

void Fupm::intialization(ifstream &input_asm) {
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

        record_line(splitcommands, cur_ptr);
        
        cout<<cur_ptr<<": ";
        for (auto& i: splitcommands) {
            cout<<i<<" ";
        }
        cout<<endl;

        cur_ptr += 1;
    }
    input_asm.close();
}

void Fupm::record_line(const vector<string>& splitcommands, int address) {
    if (splitcommands[0] == "end") { Program[address] = 0u; return;}
    try {
        T ans = *new T(codes.at(splitcommands[0]));
        string type = *new string(types.at(ans));
        T num;
        if (type=="RM") {
            auto registr = (uchar)stoi(splitcommands[1].substr(1, 2));
            try {
                num =(T)stoi(splitcommands[2]);
            } catch(invalid_argument&) {
                if (labels.find(splitcommands[2]) != labels.end()) {
                    num = labels.find(splitcommands[2])->second;
                } else {
                    throw out_of_range("COMPILATION ERROR");
                }
            }
            ans = (((ans<<4u) | registr)<<20u) | num;
        } else if (type=="RR") {
            auto registr1 = (uchar)stoi(splitcommands[1].substr(1, 2));
            auto registr2 = (uchar)stoi(splitcommands[2].substr(1, 2));
            try {
                num =(T)stoi(splitcommands[3]);
            } catch(invalid_argument&) {
                if (labels.find(splitcommands[3]) != labels.end()) {
                    num = labels.find(splitcommands[3])->second;
                } else {
                    throw out_of_range("COMPILATION ERROR");
                }
            }
            ans = (((((ans<<4u) | registr1)<<4u) | registr2)<<16u) | num;
        } else if (type=="RI") {
            auto registr = (uchar)stoi(splitcommands[1].substr(1, 8));
            try {
                num =(T)stoi(splitcommands[2]);
            } catch(invalid_argument&) {
                if (labels.find(splitcommands[2]) != labels.end()) {
                    num = labels.find(splitcommands[3])->second;
                } else {
                    throw out_of_range("COMPILATION ERROR");
                }
            }
            ans = (((ans<<4u) | registr)<<20u) | num;
        } else if (type=="J") {
            try {
                num =(T)stoi(splitcommands[1]);
            } catch(invalid_argument&) {
                if (labels.find(splitcommands[1]) != labels.end()) {
                    num = labels.find(splitcommands[1])->second;
                } else {
                    throw out_of_range("COMPILATION ERROR");
                }
            }
            ans = (ans<<24u) + num;
        } else {
            throw out_of_range("COMPILATION ERROR");
        }
        Program[address] = ans;

    } catch(out_of_range&) {
        cout<<"COMPILERROR"<<endl;
    }
}

/*void print_bin (T ans) {
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
}*/

int main() {
    Fupm CPU;
    ifstream input_asm("input.fasm");
    CPU.intialization(input_asm);


    return 0;
}
