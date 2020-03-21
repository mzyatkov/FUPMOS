#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstring>
#include <algorithm>

#define TWO_20 1048576u
#define TWO_16 65536u

using namespace std;

typedef unsigned int T;
typedef unsigned char uchar;

/*
void print_bin(T ans) {
    vector<uchar> bin;
    while (ans > 0) {
        bin.push_back(ans % 2);
        ans /= 2;
    }
    bin.resize(32);
    for (int i = 31; i >= 0; --i) {
        cout << (int) bin[i];
    }
    cout << endl;
}*/

vector<string> split(const string &s, char delim) {
    vector<string> tokens;
    string token = s;
    istringstream strstream(s);
    while (getline(strstream, token, delim)) {
        token.erase(remove(token.begin(), token.end(), '\t'), token.end());
        if (token.empty()) {
            continue;
        }
        tokens.push_back(token);
    }
    return tokens;
}

struct commands {
    const string command_name;
    const string command_type;
    const T command_code;
};

class Fupm {
public:
    Fupm() {
        Program.resize(TWO_20);
        input_asm.open("input.fasm");
        flags = 0;
        r[14] = TWO_20 - 1u;
    }

    ~Fupm() {
        Program.clear();
        input_asm.close();
    }

    ifstream input_asm;

    int intialization();

    int execution();

private:
    vector<T> Program;
    map<string, int> labels;
    T r[16]{};
    int flags;
    enum SYSCALLS {
        EXIT = 0, SCANINT = 100, SCANDOUBLE = 101, PRINTINT = 102, PRINTDOUBLE = 103, GETCHAR = 104, PUTCHAR = 105
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
                                           {"storer",  "RR", 69},
                                           {"loadr2",  "RR", 70},
                                           {"storer2", "RR", 71}};
    const map<string, T> codes = [&] {
        map<string, T> result;
        for (auto &i: Commands) {
            result[i.command_name] = i.command_code;
        }
        return result;
    }();
    const map<T, string> types = [&] {
        map<T, string> result;
        for (auto &i: Commands) {
            result[i.command_code] = i.command_type;
        }
        return result;
    }();

    //Преобразует вектор в машинное слово и записывает в программу
    int record_line(const vector<string> &splitcommands, int address, int error_address);

    /*ассемблеррные команды*/

    void halt(int rgstr, int operand) { while (true) { continue; }}

    int syscall(int rgstr, int operand) {
        double out, inp;
        unsigned long n, ans;
        switch (operand) {
            case EXIT:
                return 1;
                break;
            case SCANINT:
                cin >> r[rgstr];
                break;
            case SCANDOUBLE:
                cin >> inp;
                memcpy(&ans, &inp, 8);
                r[rgstr + 1] = ans >> 32ul;
                r[rgstr] = ans & ((1ul << 32ul) - 1ul);
                break;
            case PRINTINT:
                cout << (int) r[rgstr];
                break;
            case PRINTDOUBLE:
                n = ((unsigned long) r[rgstr + 1] << 32u) + r[rgstr];
                memcpy(&out, &n, 8);
                cout << out;
                break;
            case GETCHAR:
                char c;
                cin >> c;
                r[rgstr] = (uchar) c;
                break;
            case PUTCHAR:
                cout << (char) r[rgstr];
                break;
            default:
                cout << "ERror" << endl;
                break;
        }
        return 0;
    }

    void add(int rgstr1, int rgstr2, int modifier) {
        r[rgstr1] = r[rgstr1] + (r[rgstr2] + modifier);
    }

    void addi(int rgstr, int operand) {
        r[rgstr] = (r[rgstr] + operand);
    }

    void sub(int rgstr1, int rgstr2, int modifier) {
        r[rgstr1] = (int) r[rgstr1] - (int) (r[rgstr2] + modifier);
    }

    void subi(int rgstr, int operand) {
        r[rgstr] = ((int) r[rgstr] - operand);
    }

    void muli(int rgstr, int operand) {
        unsigned long ans = (unsigned long) r[rgstr] * operand;
        r[rgstr + 1] = (ans >> 32ul) & (-1u);
        r[rgstr] = ans & (-1u);
    }

    void mul(int rgstr1, int rgstr2, int modifier) {
        muli(rgstr1, (int) r[rgstr2] + modifier);
    }

    int divi(int rgstr, int operand) {
        unsigned long left = ((unsigned long) r[rgstr + 1] << 32ul) + (T) r[rgstr];
        if (operand == 0) { return 1; }
        unsigned long ans = left / operand;
        r[rgstr + 1] = left % operand;
        r[rgstr] = ans;
        if ((ans >> 32u) != 0) { return 1; }
        return 0;
    }

    int div(int rgstr1, int rgstr2, int modifier) {
        return divi(rgstr1, (int) r[rgstr2] + modifier);
    }

    void lc(int rgstr, int operand) {
        r[rgstr] = operand;
    }

    void shli(int rgstr, int operand) {
        if ((T) operand >= 32u) {
            r[rgstr] = 0;
            return;
        }
        r[rgstr] = r[rgstr] << (T) operand;
    }

    void shl(int rgstr1, int rgstr2, int modifier) {
        shli(rgstr1, (int) r[rgstr2] + modifier);
    }

    void shri(int rgstr, int operand) {
        if ((T) operand >= 32u) {
            r[rgstr] = 0;
            return;
        }
        r[rgstr] = r[rgstr] >> (T) operand;
    }

    void shr(int rgstr1, int rgstr2, int modifier) {
        shri(rgstr1, (int) r[rgstr2] + modifier);
    }

    void andi(int rgstr, int operand) {
        r[rgstr] = (T) r[rgstr] & (T) operand;
    }

    void aand(int rgstr1, int rgstr2, int modifier) {
        andi(rgstr1, (int) r[rgstr2] + modifier);
    }

    void ori(int rgstr, int operand) {
        r[rgstr] = r[rgstr] | (T) operand;
    }

    void oor(int rgstr1, int rgstr2, int modifier) {
        ori(rgstr1, (int) r[rgstr2] + modifier);
    }

    void xori(int rgstr, int operand) {
        r[rgstr] = r[rgstr] ^ (T) operand;
    }

    void xxor(int rgstr1, int rgstr2, int modifier) {
        xori(rgstr1, (int) r[rgstr2] + modifier);
    }

    void nnot(int rgstr, int operand) {
        r[rgstr] = ~r[rgstr];
    }

    void mov(int rgstr1, int rgstr2, int modifier) {
        r[rgstr1] = (int) r[rgstr2] + modifier;
    }

    void addd(int rgstr1, int rgstr2, int modifier) {
        uint64_t double1 = ((uint64_t) r[rgstr1 + 1] << 32ul) + r[rgstr1];
        uint64_t double2 = ((uint64_t) r[rgstr2 + 1] << 32ul) + (T) (r[rgstr2] + modifier);
        double double3;
        double double4;
        memcpy(&double3, &double1, 8);
        memcpy(&double4, &double2, 8);
        double sum = double3 + double4;
        uint64_t ans;
        memcpy(&ans, &sum, 8);
        r[rgstr1 + 1] = (unsigned long) ans >> 32u;
        r[rgstr1] = (unsigned long) ans & ((1ul << 32ul) - 1ul);
    }

    void subd(int rgstr1, int rgstr2, int modifier) {
        uint64_t double1 = ((uint64_t) r[rgstr1 + 1] << 32ul) + r[rgstr1];
        uint64_t double2 = ((uint64_t) r[rgstr2 + 1] << 32ul) + (T) (r[rgstr2] + modifier);
        double double3;
        double double4;
        memcpy(&double3, &double1, 8);
        memcpy(&double4, &double2, 8);
        double sum = double3 - double4;
        uint64_t ans;
        memcpy(&ans, &sum, 8);
        r[rgstr1 + 1] = (unsigned long) ans >> 32u;
        r[rgstr1] = (unsigned long) ans & ((1ul << 32ul) - 1ul);
    }

    void muld(int rgstr1, int rgstr2, int modifier) {
        uint64_t double1 = ((uint64_t) r[rgstr1 + 1] << 32ul) + r[rgstr1];
        uint64_t double2 = ((uint64_t) r[rgstr2 + 1] << 32ul) + (T) (r[rgstr2] + modifier);
        double double3;
        double double4;
        memcpy(&double3, &double1, 8);
        memcpy(&double4, &double2, 8);
        double sum = double3 * double4;
        uint64_t ans;
        memcpy(&ans, &sum, 8);
        r[rgstr1 + 1] = (unsigned long) ans >> 32u;
        r[rgstr1] = (unsigned long) ans & ((1ul << 32ul) - 1ul);
    }

    int divd(int rgstr1, int rgstr2, int modifier) {
        uint64_t double1 = ((uint64_t) r[rgstr1 + 1] << 32ul) + r[rgstr1];
        uint64_t double2 = ((uint64_t) r[rgstr2 + 1] << 32ul) + (T) (r[rgstr2] + modifier);
        double double3;
        double double4;
        memcpy(&double3, &double1, 8);
        memcpy(&double4, &double2, 8);
        if (double4 == 0.) { return 1; }
        double sum = double3 / double4;
        uint64_t ans;
        memcpy(&ans, &sum, 8);
        r[rgstr1 + 1] = (unsigned long) ans >> 32u;
        r[rgstr1] = (unsigned long) ans & ((1ul << 32ul) - 1ul);
        return 0;
    }

    void itod(int rgstr1, int rgstr2, int modifier) {
        auto n = (double) (r[rgstr2] + modifier);
        uint64_t ans;
        memcpy(&ans, &n, 8);
        r[rgstr1 + 1] = (unsigned long) ans >> 32u;
        r[rgstr1] = (unsigned long) ans & ((1ul << 32ul) - 1ul);
    }

    int dtoi(int rgstr1, int rgstr2, int modifier) {
        auto n = ((unsigned long) r[rgstr2 + 1] << 32u) + (T) (r[rgstr2] + modifier);
        double ans;
        memcpy(&ans, &n, 8);
        if (((unsigned long) ans >> 32ul) != 0) { return 1; }
        r[rgstr1] = (unsigned long) ans & ((1ul << 32ul) - 1ul);
        return 0;
    }

    void push(int rgstr, int operand) {
        Program[--r[14]] = r[rgstr] + operand;
    }

    void pop(int rgstr, int operand) {
        r[rgstr] = Program[r[14]++] + operand;
    }

    void calli(T address) {
        push(15, 1);
        r[15] = address - 1u;
    }

    void call(int rgstr1, int rgstr2, int modifier) {
        calli(r[rgstr2] + modifier);
        r[rgstr1] = r[rgstr2] + modifier;
    }

    void ret(int operand) {
        r[15] = Program[r[14]] - 1u;
        r[14] += operand + 1u;
    }

    void cmpi(int rgstr, int operand) {
        flags = ((int) r[rgstr] - operand);
    }

    void cmp(int rgstr1, int rgstr2, int modifier) {
        cmpi(rgstr1, r[rgstr2 + modifier]);
    }

    void cmpd(int rgstr1, int rgstr2, int modifier) {
        uint64_t double1 = ((uint64_t) r[rgstr1 + 1] << 32ul) + r[rgstr1];
        uint64_t double2 = ((uint64_t) r[rgstr2 + 1] << 32ul) + (T) (r[rgstr2] + modifier);
        double double3;
        double double4;
        memcpy(&double3, &double1, 8);
        memcpy(&double4, &double2, 8);
        double sum = double3 - double4;
        uint64_t ans;
        flags = (int) sum;
    }

    void jmp(T address) {
        r[15] = address - 1u;
    }

    void jne(T address) {
        if (flags != 0) {
            r[15] = address - 1u;
        }
    }

    void jeq(T address) {
        if (flags == 0) {
            r[15] = address - 1u;
        }
    }

    void jle(T address) {
        if (flags <= 0) {
            r[15] = address - 1u;
        }
    }

    void jl(T address) {
        if (flags < 0) {
            r[15] = address - 1u;
        }
    }

    void jge(T address) {
        if (flags >= 0) {
            r[15] = address - 1u;
        }
    }

    void jg(T address) {
        if (flags > 0) {
            r[15] = address - 1u;
        }
    }

    void load(int rgstr, T address) {
        r[rgstr] = Program[address];
    }

    void store(int rgstr, T address) {
        Program[address] = r[rgstr];
    }

    void load2(int rgstr, T address) {
        r[rgstr] = Program[address];
        r[rgstr + 1] = Program[address + 1];
    }

    void store2(int rgstr, T address) {
        Program[address] = r[rgstr];
        Program[address + 1] = r[rgstr + 1];
    }

    void loadr(int rgstr1, int rgstr2, int modifier) {
        load(rgstr1, r[rgstr2] + modifier);
    }

    void storer(int rgstr1, int rgstr2, int modifier) {
        store(rgstr1, r[rgstr2] + modifier);
    }

    void loadr2(int rgstr1, int rgstr2, int modifier) {
        load2(rgstr1, r[rgstr2] + modifier);
    }

    void storer2(int rgstr1, int rgstr2, int modifier) {
        store2(rgstr1, r[rgstr2] + modifier);
    }
};


int Fupm::intialization() {
    //первый проход
    for (int cur_ptr = 0; !input_asm.eof();) {
        string s;
        getline(input_asm, s);

        s = s.substr(0, s.find_first_of(';')); //удаляем комментарии

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
    input_asm.clear();
    input_asm.seekg(0, ios_base::beg);
    for (int cur_ptr = 0, error_address = 1; !input_asm.eof(); error_address++) {
        string s;
        getline(input_asm, s);

        s = s.substr(0, s.find_first_of(';')); //удаляем комментарии

        vector<string> splitcommands = split(s, ' ');
        for (int i = 0; i < splitcommands.size(); ++i) {
            if (splitcommands[i][splitcommands[i].size() - 1] == ':') {
                splitcommands.erase(splitcommands.begin() + i);
                i--;
            }
        }
        if (splitcommands.empty()) {
            continue;
        }

        //записываем слово(32 бита). Возвращает 1, если ошибка компиляции
        if (record_line(splitcommands, cur_ptr, error_address) == 1) {
            return 1;
        }

        /*cout << cur_ptr << ": ";
        for (auto &i: splitcommands) {
            cout << i << " ";
        }
        cout << endl;*/

        cur_ptr += 1;

    }
    return 0;
}

int Fupm::record_line(const vector<string> &splitcommands, int address, int error_address) {
    try {
        T num, ans;
        if (splitcommands[0] == "end") {
            r[15] = (T) labels.at(splitcommands[1]);
            return 0;
        }
        if (splitcommands[0] == "word") {
            return 0;
        }

        ans = codes.at(splitcommands[0]);
        string type = string(types.at(ans));

        if (splitcommands[0] == "ret") {
            num = (T) stoi(splitcommands[1]);
            Program[address] = (ans << 24u) | num;
            return 0;
        }
        if (type == "RM") {
            auto registr = (uchar) stoi(splitcommands[1].substr(1, 2));
            try {
                num = (T) stoi(splitcommands[2]);
            } catch (invalid_argument &) {
                if (labels.find(splitcommands[2]) != labels.end()) {
                    num = labels.find(splitcommands[2])->second;
                } else {
                    throw out_of_range("COMPILATION ERROR");
                }
            }
            ans = (((ans << 4u) | registr) << 20u) | num;
        } else if (type == "RR") {
            auto registr1 = (uchar) stoi(splitcommands[1].substr(1, 2));
            auto registr2 = (uchar) stoi(splitcommands[2].substr(1, 2));
            try {
                num = (T) stoi(splitcommands[3]);
            } catch (invalid_argument &) {
                if (labels.find(splitcommands[3]) != labels.end()) {
                    num = labels.find(splitcommands[3])->second;
                } else {
                    throw out_of_range("COMPILATION ERROR");
                }
            }
            ans = (((((ans << 4u) | registr1) << 4u) | registr2) << 16u) | num;
        } else if (type == "RI") {
            auto registr = (uchar) stoi(splitcommands[1].substr(1, 8));
            try {
                num = (T) stoi(splitcommands[2]);
            } catch (invalid_argument &) {
                if (labels.find(splitcommands[2]) != labels.end()) {
                    num = labels.find(splitcommands[2])->second;
                } else {
                    throw out_of_range("COMPILATION ERROR");
                }
            }
            ans = (((ans << 4u) | registr) << 20u) | num;
        } else if (type == "J") {
            try {
                num = (T) stoi(splitcommands[1]);
            } catch (invalid_argument &) {
                if (labels.find(splitcommands[1]) != labels.end()) {
                    num = labels.find(splitcommands[1])->second;
                } else {
                    throw out_of_range("COMPILATION ERROR");
                }
            }
            ans = (ans << 24u) + num;
        } else {
            throw out_of_range("COMPILATION ERROR");
        }
        //print_bin(ans);
        Program[address] = ans;

    } catch (out_of_range &) {
        cout << "error in line " << error_address << endl;
        return 1;
    }
    return 0;
}

int Fupm::execution() {
    for (;; r[15]++) {
        T line = Program[r[15]];
        //для удобства вычисляем все возможные разбиения машинного слова
        uchar code = line >> 24u;
        T rgstr = (line >> 20u) & 15u;
        T rgstr1 = rgstr;
        T rgstr2 = (line >> 16u) & 15u;
        T modifier = line & (TWO_16 - 1u);
        T address = line & (TWO_20 - 1u);
        int operand = address;
        switch (code) {
            case 0:
                halt(rgstr, operand);
                break;
            case 1:
                if (syscall(rgstr, operand) == 1) { return 0; }
                break;
            case 2:
                add(rgstr1, rgstr2, modifier);
                break;
            case 3:
                addi(rgstr, operand);
                break;
            case 4:
                sub(rgstr1, rgstr2, modifier);
                break;
            case 5:
                subi(rgstr, operand);
                break;
            case 6:
                mul(rgstr1, rgstr2, modifier);
                break;
            case 7:
                muli(rgstr, operand);
                break;
            case 8:
                if (div(rgstr1, rgstr2, modifier) == 1) { return 0; };
                break;
            case 9:
                if (divi(rgstr, operand) == 1) { return 0; };
                break;
            case 12:
                lc(rgstr, operand);
                break;
            case 13:
                shl(rgstr1, rgstr2, modifier);
                break;
            case 14:
                shli(rgstr, operand);
                break;
            case 15:
                shr(rgstr1, rgstr2, modifier);
                break;
            case 16:
                shri(rgstr, operand);
                break;
            case 17    :
                aand(rgstr1, rgstr2, modifier);
                break;
            case 18    :
                andi(rgstr, operand);
                break;
            case 19    :
                oor(rgstr1, rgstr2, modifier);
                break;
            case 20    :
                ori(rgstr, operand);
                break;
            case 21    :
                xxor(rgstr1, rgstr2, modifier);
                break;
            case 22    :
                xori(rgstr, operand);
                break;
            case 23    :
                nnot(rgstr, operand);
                break;
            case 24    :
                mov(rgstr1, rgstr2, modifier);
                break;
            case 32    :
                addd(rgstr1, rgstr2, modifier);
                break;
            case 33    :
                subd(rgstr1, rgstr2, modifier);
                break;
            case 34    :
                muld(rgstr1, rgstr2, modifier);
                break;
            case 35    :
                if (divd(rgstr1, rgstr2, modifier) == 1) { return 0; };
                break;
            case 36    :
                itod(rgstr1, rgstr2, modifier);
                break;
            case 37    :
                if (dtoi(rgstr1, rgstr2, modifier) == 1) { return 0; };
                break;
            case 38    :
                push(rgstr, operand);
                break;
            case 39    :
                pop(rgstr, operand);
                break;
            case 40    :
                call(rgstr1, rgstr2, modifier);
                break;
            case 41    :
                calli(address);
                break;
            case 42    :
                ret(operand);
                break;
            case 43    :
                cmp(rgstr1, rgstr2, modifier);
                break;
            case 44    :
                cmpi(rgstr, operand);
                break;
            case 45    :
                cmpd(rgstr1, rgstr2, modifier);
                break;
            case 46    :
                jmp(address);
                break;
            case 47    :
                jne(address);
                break;
            case 48    :
                jeq(address);
                break;
            case 49    :
                jle(address);
                break;
            case 50    :
                jl(address);
                break;
            case 51    :
                jge(address);
                break;
            case 52    :
                jg(address);
                break;
            case 64    :
                load(rgstr, address);
                break;
            case 65    :
                store(rgstr, address);
                break;
            case 66    :
                load2(rgstr, address);
                break;
            case 67    :
                store2(rgstr, address);
                break;
            case 68    :
                loadr(rgstr1, rgstr2, modifier);
                break;
            case 69    :
                storer(rgstr1, rgstr2, modifier);
                break;
            case 70    :
                loadr2(rgstr1, rgstr2, modifier);
            case 71    :
                storer2(rgstr1, rgstr2, modifier);
                break;

            default:
                break;
        }
    }
}

int main() {
    Fupm CPU;
    if (CPU.intialization() == 0) {
        CPU.execution();
    }
    return 0;
}
