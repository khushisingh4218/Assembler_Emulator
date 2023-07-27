/*****************************************************************************
TITLE: Claims																																
AUTHOR: 2101CS37 KHUSHI SINGH
Declaration of Authorship
This emu.cpp is part of the miniproject of CS209/CS210 at the 
department of Computer Science and Engg, IIT Patna . 
*****************************************************************************/
#include <bits/stdc++.h>
#include <fstream>
using namespace std;
typedef struct Mnemonic
{
    string mnem;
    string opcode;
    int operand;
    Mnemonic(string mnem, string opcode, int operand)
    {
        this->mnem = mnem;
        this->opcode = opcode;
        this->operand = operand;
    }
    //  ______________________________________________
    // |_____operand_________|_____specification______|
    // |    0                |  No operand is needed  |
    // |    1                |  Value is required     |
    // |    2                |  Data is required      |
    // |    3                |  Set is required       |
    // |    4                |  Offset is required    |
    // |_____________________|________________________|

} Mnemonic;

vector<Mnemonic> mnemonics = {
    Mnemonic("data", "", 2),           // Stop the emulator
    Mnemonic("ldc", "00000000", 1),    // Reserve a memory location, initialized to the value specified
    Mnemonic("adc", "00000001", 1),    // Load accumulator with the value specified
    Mnemonic("ldl", "00000010", 4),    // Add the value specified to the accumulator
    Mnemonic("stl", "00000011", 4),    // Load local
    Mnemonic("ldnl", "00000100", 4),   // Store local
    Mnemonic("stnl", "00000101", 4),   // Load non-local
    Mnemonic("add", "00000110", 0),    // Store non-local
    Mnemonic("sub", "00000111", 0),    // Addition
    Mnemonic("shl", "00001000", 0),    // Subtraction
    Mnemonic("shr", "00001001", 0),    // Shift left
    Mnemonic("adj", "00001010", 1),    // Shift right
    Mnemonic("a2sp", "00001011", 0),   // Adjust SP
    Mnemonic("sp2a", "00001100", 0),   // Transfer A to SP
    Mnemonic("call", "00001101", 4),   // Transfer SP to A
    Mnemonic("return", "00001110", 0), // Call procedure
    Mnemonic("brz", "00001111", 4),    // Return from procedure
    Mnemonic("brlz", "00010000", 4),   // If accumulator is zero, branch to specified offset
    Mnemonic("br", "00010001", 4),     // If accumulator is less than zero,branch to specified offset
    Mnemonic("HALT", "00010010", 0),   // Branch to specified offset
    Mnemonic("SET", "", 3)             // Set the label on this line to the specified value
};
string deci_2scomp(int num, int n);
void execution(int, int, int, int, int, string);
int findMnemonic(string str);
int operations = 0;
int binary_to_deci(string s);
map<string, char> hexadeci;
map<string, int> options;
set<pair<int, pair<string, string>>> isa;
vector<string> machine_code(999999);
int machine_itr[999999];
int main(int argc, char **argv)
{

    hexadeci["0000"] = '0';
    hexadeci["0001"] = '1';
    hexadeci["0010"] = '2';
    hexadeci["0011"] = '3';
    hexadeci["0100"] = '4';
    hexadeci["0101"] = '5';
    hexadeci["0110"] = '6';
    hexadeci["0111"] = '7';
    hexadeci["1000"] = '8';
    hexadeci["1001"] = '9';
    hexadeci["1010"] = 'A';
    hexadeci["1011"] = 'B';
    hexadeci["1100"] = 'C';
    hexadeci["1101"] = 'D';
    hexadeci["1110"] = 'E';
    hexadeci["1111"] = 'F';

    ifstream assembly;
    if (argc != 3)
    {
        cout << "Invalid format!" << endl;
        /*
        ./emu [function] file.o
        -trace to show instruction trace
        -read to show read memory
        -write to show memory writes
        -before to show memory dump before execution
        -after to show memory dump after execution
        -wipe to erase written flags before execution
        -isa to show Instruction set architecture
        */

        return 0;
    }
    options["-trace"] = 1;
    options["-read"] = 2;
    options["-write"] = 3;
    options["-before"] = 4;
    options["-after"] = 5;
    options["-wipe"] = 6;
    options["-isa"] = 7;
    // filename
    string file_name = argv[2];
    int pos = file_name.find(".");
    file_name = file_name.substr(0, pos);
    string object_name = file_name + ".obj";
    // function
    string opt = "";
    for (int i = 0; i < strlen(argv[1]); i++)
    {
        opt = opt + argv[1][i];
    }
    // cout<<log_name<<endl<<listing_name<<endl<<object_name;
    int a;  // register A
    int b;  // register B
    int pc; // program counter
    int sp; // stack pointer
    FILE *object = NULL;
    object = fopen(object_name.c_str(), "rb");
    size_t itr;
    if (object != NULL)
    {
        itr = fread(machine_itr, sizeof(int), 999999, object);
        for (int i = 0; i < itr; i++)
        {
            machine_code[i] = deci_2scomp(machine_itr[i], 32);
        }

        if (options[opt] == 6)
        {
            // wipe
            cout << "Wiping written flags before execution." << endl;
            a = 0, b = 0, sp = 0, pc = 0;
            cout << "SP,PC,A,B are in reset state." << endl;
        }
        if (options[opt] == 7)
        {
            // isa
            cout << "Displaying Instruction Set Architecture:" << endl;
            for (auto i : isa)
            { // mnemonic             //operand
                cout << i.first << ":  " << i.second.first << "   " << i.second.second;
                cout << endl;
            }
            cout << "   "
                 << "SET   value" << endl
                 << endl;
        }
        if (options[opt] == 4)
        {
            
            // before
            cout << "Displaying memory dump before execution:" << endl;
            for (int i = 0; i < itr - 4; i += 4)
            {
                cout << deci_2scomp(i, 32);
                for (int j = 0; j < 4; j++)
                {
                    cout << " " << machine_code[i + j];
                }
                cout << endl
                     << endl;
            }

            execution(itr, pc, a, b, sp, opt);
        }
        if (options[opt] == 5)
        {
            // after
            cout << "Displaying memory dump after execution has been done:" << endl;
            for (int i = 0; i < itr - 4; i += 4)
            {
                cout << deci_2scomp(i, 32);
                for (int j = 0; j < 4; j++)
                {
                    cout << " " << machine_code[i + j];
                }
                cout << endl
                     << endl;
            }
        }
        cout << "Execution successful!!" << endl;
        cout << "No. of instructions executed: " << operations;

        fclose(object);
    }
}

string deci_2scomp(int num, int n)
{
    char bin[n + 1];
    for (int i = 0; i < n; i++)
    {
        unsigned int mask = 1u << (n - i - 1);
        bin[i] = (num & mask) ? '1' : '0';
    }
    bin[n] = '\0';
    string s = "";
    for (int i = 0; i < n; i += 4)
    {
        string temp = "";
        for (int j = 0; j < 4; j++)
        {
            temp += bin[i + j];
        }
        s += hexadeci[temp];
    }
    return s;
}

int binary_to_deci(string s)
{
    int i, N, decimal_number = 0, p = 0;
    N = s.size();
    for (i = N - 1; i >= 0; i--)
    {
        if (s[i] == '1')
        {
            decimal_number += pow(2, p);
        }
        p++;
    }
    return decimal_number;
}

string DecimalToBinaryString(int a)
{
    string binary = "";
    int mask = 1;
    for (int i = 0; i < 31; i++)
    {
        if ((mask & a) >= 1)
            binary = "1" + binary;
        else
            binary = "0" + binary;
        mask <<= 1;
    }
    cout << binary << endl;
    return binary;
}

int string_to_deci(string s, int n)
{
    int val = 0;
    unsigned int i;
    string s1 = s;
    if (n == 24)
    {

        if (s[0] == '8' or s[0] == '9' or s[0] == 'A' or s[0] == 'B' or s[0] == 'C' or s[0] == 'D' or s[0] == 'E' or s[0] == 'F')
        {
            while (s1.length() < 8)
            {
                s1.insert(0, "F");
            }
        }
        else
        {
            while (s1.length() < 8)
            {
                s1.insert(0, "0");
            }
        }
    }
    else
    {

        while (s1.length() < n / 4)
        {
            s1.insert(0, "0");
        }
    }
    stringstream ss;
    ss << std::hex << s1;
    ss >> i;
    val = static_cast<int>(i);

    return val;
}

int FindOpcode(string str)
{
    for (int i = 0; i < mnemonics.size(); i++)
    {
        if (mnemonics[i].opcode == str)
            return i;
    }
    return -1;
}

void execution(int itr, int pc, int a, int b, int sp, string opt)
{
    for (pc = 0; pc < itr; pc++)
    {   operations++;
        int opcode = string_to_deci(machine_code[pc].substr(6, 2), 8);
        int val = string_to_deci(machine_code[pc].substr(0, 6), 24);
        string str = DecimalToBinaryString(opcode);
        int index = FindOpcode(str);

        if (mnemonics[index].operand == 0)
        {
            if (options[opt] == 1)
            {
                cout << "PC= " << deci_2scomp(pc, 32).c_str() << " SP= " << deci_2scomp(sp, 32).c_str() << " A= " << deci_2scomp(a, 32).c_str() << " B= " << deci_2scomp(b, 32).c_str() << " " << mnemonics[index].mnem.c_str() << endl;
            }

            if (str.substr(24,8) == "00000110")
                a += b;
            else if (str.substr(24, 8) == "00000111")
                a = b - a;
            else if (str.substr(24, 8) == "00001000")
                a = b << a;
            else if (str.substr(24, 8) == "00001001")
                a = b >> a;
            else if (str.substr(24, 8) == "00001011")
            {
                sp = a;
                a = b;
            }
            else if (str.substr(24, 8) == "00001100")
            {
                b = a;
                a = sp;
            }
            else if (str.substr(24, 8) == "00001110")
            {
                pc = a;
                a = b;
            }
            else if (str.substr(24, 8) == "00010010")
            {
                if (opt == "-traces")
                    cout << "Execution finished!!" << endl;
                return;
            }
            else
            {
                cout << "Invalid instruction!! Execution halted." << endl;
                return;
            }
        }
        else if (mnemonics[index].operand == 1)
        {
            if (options[opt] == 1)
            {
                cout << "PC= " << deci_2scomp(pc, 32).c_str() << " SP= " << deci_2scomp(sp, 32).c_str() << " A= " << deci_2scomp(a, 32).c_str() << " B= " << deci_2scomp(b, 32).c_str() << " " << mnemonics[index].mnem.c_str() << endl
                     << mnemonics[index].mnem << " " << deci_2scomp(val, 32) << endl;
            }

            if (mnemonics[index].operand == 4)
            {
                if (str.substr(24, 8) == "00001101")
                {
                    b = a;
                    a = pc;
                    pc += val;
                }
                else if (str.substr(24, 8) == "00001111")
                {
                    if (!a)
                        pc += val;
                }
                else if (str.substr(24, 8) == "00010000")
                {
                    if (a < 0)
                        pc += val;
                }
                else if (str.substr(24, 8) == "00010001")
                {
                    pc += val;
                }
                else
                {
                    cout << "Invalid instruction! Execution halted." << endl;
                    return;
                }
            }
            else
            {
                if (str.substr(24, 8) == "00000000")
                {
                    b = a;
                    a = val;
                }
                else if (str.substr(24, 8) == "00000001")
                {
                    a += val;
                }
                else if (str.substr(24, 8) == "00000010")
                {
                    b = a;
                    if (!(sp + val >= 0 && sp + val <= 9999999))
                    {
                        if (options[opt] == 1)
                            cout << "Invalid memory access" << endl;
                        return;
                    }
                    a = machine_itr[sp + val];
                    if (opt == "-read")
                    {
                        cout << "Reading memory: [" << deci_2scomp(sp + val, 32).c_str() << "] contains " << deci_2scomp(a, 32).c_str() << endl;
                    }
                }

                else if (str.substr(24, 8) == "00000011")
                {
                    if (!(sp + val >= 0 && sp + val <= 9999999))
                    {
                        if (options[opt] == 1)
                            cout << "Invalid memory access" << endl;
                        return;
                    }
                    if (opt == "-write")
                    {
                        cout << "Writing memory: " << deci_2scomp(sp + val, 32).c_str() << endl;
                        cout << "Initial value contained in reg A: " << deci_2scomp(machine_itr[sp + val], 32).c_str() << endl;
                    }
                    machine_itr[sp + val] = a;
                    if (opt == "-write")
                    {
                        cout << "Final value contained in reg A: " << deci_2scomp(a, 32).c_str() << endl;
                        a = b;
                    }
                }

                else if (str.substr(24, 8) == "00000100")
                {
                    if (!(sp + val >= 0 && sp + val <= 9999999))
                    {
                        if (options[opt] == 1)
                            cout << "Invalid memory access" << endl;
                        return;
                    }
                    if (opt == "-read")
                    {
                        cout << "Reading memory: [" << deci_2scomp(a + val, 32).c_str() << "] contains " << deci_2scomp(machine_itr[a + val], 32).c_str() << endl;
                    }
                    a = machine_itr[a + val];
                }

                else if (str.substr() == "00000101")
                {
                    if (!(sp + val >= 0 && sp + val <= 9999999))
                    {
                        if (options[opt] == 1)
                            cout << "Invalid memory access" << endl;
                        return;
                    }
                    if (opt == "-write")
                    {
                        cout << "Writing memory: " << deci_2scomp(a + val, 32).c_str() << endl;
                        cout << "Initial value contained in reg A: " << deci_2scomp(machine_itr[a + val], 32).c_str() << endl;
                    }
                    machine_itr[a + val] = b;
                    if (opt == "-write")
                    {
                        cout << "Final value contained in reg A: " << deci_2scomp(b, 32).c_str() << endl;
                    }
                }
                else if (str.substr(24, 8) == "00001010")
                {
                    sp += val;
                }

                else
                {
                    cout << "Invalid execution! Execution halted." << endl;
                    return;
                }
            }
        }

        if (pc >= itr - 1)
        {
            if (opt == "-trace")
                cout << "Invalid memory access!" << endl;
            return;
        }
    }
}