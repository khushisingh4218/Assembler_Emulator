/*****************************************************************************
TITLE: Assembler(asm.cpp)																																
AUTHOR: 
Name: KHUSHI SINGH
Roll No. : 2101CS37
Declaration of Authorship
This asm.cpp is part of the miniproject of CS209/CS210 at the 
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
vector<string> errors;
unordered_map<string, int> label;
unordered_map<string, int> used_label;
vector<string> source[999999];
map<string, char> hexadeci;
string machine_code[999999] = {""};
int machine_itr[999999];
string toHex(int num);
int stringToNum(string str);
bool ValidLabel(string label_check);
int binary_to_deci(string s);
void modify_labels_using_SET(fstream &log);
string deci_2scomp(int data, int n);
int string_to_deci(string s, int n, fstream &log);
int findMnemonic(string mnem);
void first_pass(fstream &log, string str, int line_no, int *pc);
void generate_machine_code(fstream &log, string mnem, string label, string operand, int iterator);
void second_pass(ifstream &assembly, fstream &object, fstream &listing, fstream &log);
string numToStr(int num);

int main(int argc, char **argv)
{
    hexadeci.insert({"0000", '0'});
    hexadeci.insert({"0001", '1'});
    hexadeci.insert({"0010", '2'});
    hexadeci.insert({"0011", '3'});
    hexadeci.insert({"0100", '4'});
    hexadeci.insert({"0101", '5'});
    hexadeci.insert({"0110", '6'});
    hexadeci.insert({"0111", '7'});
    hexadeci.insert({"1000", '8'});
    hexadeci.insert({"1001", '9'});
    hexadeci.insert({"1010", 'A'});
    hexadeci.insert({"1011", 'B'});
    hexadeci.insert({"1100", 'C'});
    hexadeci.insert({"1101", 'D'});
    hexadeci.insert({"1110", 'E'});
    hexadeci.insert({"1111", 'F'});
    ifstream assembly;
    if (argc <= 1)
    {
        cout << "Input file is not specified" << endl;
        return 0;
    }
    string file_name = argv[1];
    int pos = file_name.find(".");
    file_name = file_name.substr(0, pos);

    string object_name = file_name + ".obj";
    string listing_name = file_name + ".list";
    string log_name = file_name + ".log";
    // cout<<log_name<<endl<<listing_name<<endl<<object_name;

    fstream object, listing, log;
    assembly.open(argv[1]);
    if (!assembly)
    {
        cout << "File does not exist!!" << endl;
        return 0;
    }

    log.open(log_name, ios::out);
     //object.open(object_name, ios::out );
    listing.open(listing_name, ios::out);

    if (assembly.is_open())
    {
        int line_no = 1, pc = 0;
        string current_line;
        log << "ERRORS:" << endl;
        while (getline(assembly, current_line))
        {
            string str;
            for (int i = 0; i < current_line.length(); i++)
            {
                if (current_line[i] == ';')
                    break;
                str = str + current_line[i];
            }

            // calling first pass
            first_pass(log, str, line_no, &pc);
            line_no++;
        }
        modify_labels_using_SET(log);
    }

    cout << "First pass completed!" << endl;
    //   for(int i=0;i<20;i++){
    //     for(int j=0;j<source[i].size();j++){
    //         cout<<source[i][j]<<" ";
    //     }
    //     cout<<endl;
    //   }

    // calling second pass
    second_pass(assembly, object, listing, log);
    cout << "Second pass completed!" << endl;

    // writing into listing file
    if (listing.is_open())
    {
        for (int i = 0; i < 999999; i++)
        {
            if (source[i].empty())
                break;

            listing << deci_2scomp(i, 32) << " ";
            listing << machine_code[i] << " ";

            for (int j = 0; j < source[i].size(); j++)
            {
                listing << source[i][j] << " ";
            }
            listing << endl;
        }
    }

    // writing into log file
    log << endl
        << "Labels in the asm file:" << endl;
    for (auto i : label)
    {
        log << i.first << " used at program counter " << i.second << endl;
    }
    log << endl
        << "Warning:" << endl
        << "Unused labels:" << endl;
    for (auto i : label)
    {
        if (used_label.find(i.first) == used_label.end())
        {
            log << i.first << endl;
        }
    }

    // writing into object file
    int num = 0;
    FILE *obj_pointer;
    obj_pointer = fopen(object_name.c_str(), "wb");
    for (int i = 0; i < 999999; i++)
    {
        if (source[i].empty())
            break;
        num++;
    }
    cout<<"size"<<num<<endl;
    fwrite(machine_itr, sizeof(int), num, obj_pointer);
    fclose(obj_pointer);
    listing.close();
    log.close();
    assembly.close();
cout<<"here"<<endl;
    for(int i=0;i<50;i++){
        cout<<machine_itr[i]<<endl;
    }
    cout<<"here"<<endl;
}

// function for first pass which lists out the errors
void first_pass(fstream &log, string str, int line_no, int *pc)
{
    int str_len = str.length();

    if (str_len != 0)
    {
        string s;
        // since an instruction can be splitted into maximum 5 parts
        string text[5] = {""};
        int text_len[5] = {0};
        for (int j = 0; j < str_len; j++)
        {
            s = s + str[j];
            if (s[j] == ':')
                s += " ";
        }
        stringstream ss(s);

        int i = 0;
        while (ss >> text[i])
        {

            text_len[i] = text[i].length();
            i++;
        }
        i = 0;
        while (i < 5)
        {
            if (text_len[i] != 0)
                source[*(pc)].push_back(text[i]);
            i++;
        }

        // checking if label is present
        if (text_len[0] != 0 && text[0][text_len[0] - 1] == ':')
        {
            string label_check = text[0].substr(0, text_len[0] - 1);

            if (!ValidLabel(label_check))
            {
                log << "ERROR at Line " << line_no << ":"
                    << "Bogus label name! Label should be alphanumeric only && should not begin with a digit." << endl;
            }
            else
            {//label-label

                if (label.find(label_check) == label.end())
                {   //if encountered a new label, store that label 
                    label[label_check] = *(pc);
                    if (text_len[1] != 0 || text_len[2] != 0)
              
                        (*pc)++;
                }
                else
                {
                    log << "ERROR at line " << line_no << ":"
                        << " Duplicate label!" << endl;
                }
            }
            if (text[1] != "")
            {//label-mnemonic

                // checking if mnemonic is followed by label
                if (text[1][text_len[1] - 1] != ':')
                {
                    string mnemonic = text[1];
                    int index = findMnemonic(mnemonic);
                    //index refers to the index of mnemonic 
                    if (index == -1)
                    {//invalid mnemonic
                        log << "ERROR at line " << line_no << ":"
                            << " Bogus mnemonic" << endl;
                    }
                    else
                    {

                        if (mnemonics[index].operand == 0)
                        { //no operand is needed
                            if (!(text[2] == "" && text[3] == ""))
                                log << "ERROR at line " << line_no << ": Unexpected operand! " << mnemonic << " requires no operand." << endl;
                        }
                        else
                        {
                            if (!(text[2] != "" && text[3] == ""))
                                log << "ERROR at line " << line_no << ":" << mnemonic << " requires one && only one operand." << endl;
                        }
                    }
                }
            }
        }
        else
        {
            // when there is no label

            string mnemonic = text[0];

            int index = findMnemonic(mnemonic);
            if (index == -1)
            {
                log << "ERROR at line " << line_no << ":"
                    << " Bogus mnemonic" << endl;
            }
            else
            {
                if (mnemonics[index].operand == 0)
                {
                    if (!(text[1] == "" && text[2] == ""))
                        log << "ERROR at line " << line_no << ":" << mnemonic << " requires no operand." << endl;
                }
                else
                {
                    if (!(text[1] != "" && text[2] == ""))
                        log << "ERROR at line " << line_no << ":" << mnemonic << " requires one && only one operand." << endl;
                }
                (*pc)++;
            }
        }
    }
}

// function for second pass which generates machine code and looks for more errors and warnings(if any)
void second_pass(ifstream &assembly, fstream &object, fstream &listing, fstream &log)
{
   
    for (int i = 0; i < 999999; i++)
    {

        if (source[i].empty())
            break;

        // instruction contains only label
        if (source[i].size()==1 && source[i][0][source[0].size() - 1] == ':')
             continue;
        //data
        int idx=findMnemonic(source[i][0]);
        if(idx!=-1 && mnemonics[idx].operand==2){
             generate_machine_code(log,source[i][0], "", source[i][1], i);
        }
        // instruction contains mnemonic with no operand
        if (source[i].size() == 1)
        {
            string mnem = source[i][0];
            generate_machine_code(log, mnem, "", "", i);
            continue;
        }

        //instruction contains mnemonic with operands
        string mnem = source[i][0];
        int index = findMnemonic(mnem);

        if (index != -1)
        {
            
            // instruction contains mnemonic with operand value
            if (mnemonics[index].operand == 1||mnemonics[index].operand==4)
            {

                generate_machine_code(log, mnem, "", source[i][1], i);
            }
            // instruction contains mnemonic with offset
            else if (mnemonics[index].operand == 4)
            {
                if (label.find(source[i][1]) != label.end())
                {
                    used_label[source[i][1]]++;
                }
                generate_machine_code(log, mnem, "", source[i][1], i);
            }
        }
        else
        {
            // if no mnemonic is there in the beginning of instruction
            if (source[i][0][source[i][0].length() - 1] != ':')
                log << "ERROR: " << source[i][0] << " is not defined!" << endl;
            else
            {
                // instruction starts with label
             
                string str1 = source[i][1];
                
                int idx = findMnemonic(str1);
               
                if (idx == -1)
                    log << "ERROR: " << str1 << " is undefined!" << endl;
                else
                {   
                    //data
                    if(mnemonics[idx].operand==2){
                        generate_machine_code(log,str1,"",source[i][2],i);
                    }
                    
                    //SET
                    if (mnemonics[idx].operand == 3)
                    {
                        generate_machine_code(log, str1, "", source[i][2], i);
                    }
                    
                    // label is followed by mnemonic with no operand
                    if (mnemonics[idx].operand == 0 )
                    {
                        generate_machine_code(log, str1, "", "", i);
                    }

                    else if (mnemonics[idx].operand == 0 && source[i][2] != "")
                    {
                         log << "ERROR: " << str1 << " requires no operand!" << endl;
                    }
                    // label is followed by mnemonic which requires value
                    else if (mnemonics[idx].operand == 1 && source[i][2] != "")
                    {
                        string lbl=source[i][0].substr(0,source[i][0].length()-1);
                        string opr = source[i][2];
                        if (ValidLabel(lbl) && label.find(lbl) != label.end())
                        {
                            used_label[opr]++;
                            generate_machine_code(log, str1, source[i][0].substr(0, source[i][0].length() - 1), opr, i);
                        }
                    }
                     else if (mnemonics[idx].operand == 4 && source[i][2] != "")
                    {
                        string lbl=source[i][0].substr(0,source[i][0].length()-1);
                        str1=source[i][1];
                        string opr = source[i][2];
                        
                        if (ValidLabel(lbl) && label.find(lbl) != label.end())
                        {
                            used_label[opr]++;
                            generate_machine_code(log, source[i][1], "", source[i][2], i);
                        }
                    }
                    // label is followed by another label && then an instruction
                    else if (str1[str1.length() - 1] == ':')
                    {
                        string mnem = source[i][2];
                        int idx = findMnemonic(mnem);
                        if (mnemonics[idx].operand == 0)
                        {
                            generate_machine_code(log, mnem, "", "", i);
                        }
                        else
                        {   
                            string opr;
                            if (source[i].size() > 3)
                                opr = source[i][3];

                            if (label.find(opr) != label.end())
                                used_label[opr]++;

                            generate_machine_code(log, mnem, source[i][1].substr(0, source[i][1].length() - 1), opr, i);
                        }
                    }
                }
            }
        }
    }
}

// function to convert given code to string
string numToStr(int num)
{
    stringstream stream;

    stream << num;

    string str;

    stream >> str;
    return str;
}

// function to generate machine code from the given instruction in assembly language
void generate_machine_code(fstream &log, string mnem, string label, string opr, int i)
{  
    int index = findMnemonic(mnem);
    if (index == -1)
        return;
    if (mnemonics[index].operand == 0)
    {
        machine_itr[i] = binary_to_deci(mnemonics[index].opcode);
        string code32 = "000000000000000000000000" + mnemonics[index].opcode;
        machine_code[i] = deci_2scomp(binary_to_deci(mnemonics[index].opcode), 32);
    }
    else if (mnemonics[index].operand == 1)
    {

        int val1 = string_to_deci(opr, 24, log);
        int val2 = binary_to_deci(mnemonics[index].opcode);
        machine_itr[i] = val1 << 8 | val2;

        machine_code[i] = deci_2scomp(val1, 24) + deci_2scomp(val2, 8);
    }
    else if (mnemonics[index].operand == 2)
    {
        
        // mnemonic is data
        used_label[label]++;
        int val = string_to_deci(opr, 32, log);
        machine_itr[i] = val;
        machine_code[i] = deci_2scomp(val, 32);
    }
    else if (mnemonics[index].operand == 3)
    {
        
        // mnemonic is SET
        used_label[label]++;
        int val = string_to_deci(opr, 32, log);
        machine_itr[i] = val;
        machine_code[i] = deci_2scomp(val, 32);
    }
    else if (mnemonics[index].operand == 4)
    {
     
        // offset with mnemonic
        
        int val1 = string_to_deci(opr, 24, log);
        int val2 = binary_to_deci(mnemonics[index].opcode);
        if (!ValidLabel(label))
           {val1 = val1 - 1 - i;}
        machine_itr[i] = val1 << 8 | val2;
        machine_code[i] = deci_2scomp(val1, 24) + deci_2scomp(val2, 8);
        
    }
    cout<<machine_itr[i]<<endl;
}

// function which checks out the validity of a label
bool ValidLabel(string s)
{
    if (s[0] == '1' || s[0] == '2' || s[0] == '3' || s[0] == '4' || s[0] == '5' || s[0] == '6' || s[0] == '7' || s[0] == '8' || s[0] == '9' || s[0] == '0')
        return false;
    for (int i = 0; i < s.length(); i++)
    {
        if (!((s[i] == '_') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= '0' && s[i] <= '9')))
            return false;
    }
    return true;
}

// function to check if an operand is valid or not
int ValidOperand(string s)
{

    if (ValidLabel(s) && (label.find(s) != label.end()))return 5;
  if (ValidLabel(s) && (label.find(s) == label.end()))return 1;
    int key = 3;

    for (int i = 0; i < s.length(); i++)
    {
        if ((s[i] < '0' || s[i] > '7'))
            key = 2;
    }

    if (key == 3)
        return key;

    for (int i = 0; i < s.length(); i++)
    {
        if ((s[i] < '0' || s[i] > '9'))
            key = 4;
    }

    if (key == 2)
        return key;
    for (int i = 0; i < s.length(); i++)
    {
        if (!((s[i] >= 'a' && s[i] <= 'f') || (s[i] >= 'A' && s[i] <= 'F') || (s[i] >= '0' && s[i] <= '9')))
            key = 0;
    }
    if (key == 4)
        return key;
    return 0;
}

// function to convert string(hex, oct, decimal) to decimal
int string_to_deci(string s, int n, fstream &log)
{
    if (n == 32 || n == 24)
    {
        int value = 0;
        int len = s.length();
        if (s == "0")
            return 0;
        // valid label
        if (ValidOperand(s) == 5)
        {
            return value = label[s];
        }
        else if (ValidOperand(s) == 1)
        { // invalid label
        
            return 0;
        }
        // string starts with 'positive' sign
        if (s[0] == '+')
        {
            // string is octal
            if (s[1] == '0')
            {
                if (ValidOperand(s.substr(2, len - 2)) == 3)
                {
                    int product = 1;
                    for (int i = len - 1; i >= 2; i--)
                    {
                        value += ((s[i] - '0') * product);
                        product *= 8;
                    }
                }
                else
                    log << "ERROR:Invalid format of number. Use prefix '0x' for hex and '0' for octal." << endl;
            }

            {
                // string is decimal
                
                if (ValidOperand(s.substr(1, len - 1)) == 3 || ValidOperand(s.substr(1, len - 1)) == 2)
                    value = stoi(s.substr(1, len - 1));
                else
                    log << "ERROR:Invalid format of number. Use prefix '0x' for hex and '0' for octal." << endl;
            }
            return value;
        }

        // string starts with 'negative' sign
        if (s[0] == '-')
        {
            // string is octal
            if (s[1] == '0')
            {
                if (ValidOperand(s.substr(2, len - 2)) == 3)
                {
                    int product = 1;
                    for (int i = len - 1; i >= 2; i--)
                    {
                        value += ((s[i] - '0') * product);
                        product *= 8;
                    }
                    value *= -1;
                }
                else
                    log << "ERROR:Invalid format of number. Use prefix '0x' for hex and '0' for octal." << endl;
            }
            else
            {
                // string is decimal
                if (ValidOperand(s.substr(1, len - 1)) == 3 || ValidOperand(s.substr(1, len - 1)) == 2)
                {
                    value = stoi(s.substr(1, len - 1));
                    value *= -1;
                }
                else
                    log << "ERROR:Invalid format of number. Use prefix '0x' for hex and '0' for octal." << endl;
            }
            return value;
        }

        // string is hexadecimal
        if (s[0] == '0' && s[1] == 'x')
        {
            if (ValidOperand(s.substr(2, len - 2)) == 3 || ValidOperand(s.substr(2, len - 2)) == 2 || ValidOperand(s.substr(2, len - 2)) == 4)
            {
                unsigned int x;
                string s1 = s.substr(2, len - 2);
                while (s1.length() < (n) / 4)
                {
                    s1.insert(0, "0");
                }
                stringstream ss;
                ss << std::hex << s1;
                ss >> x;
                value = static_cast<int>(x);
            }
            else
                log << "ERROR:Invalid format of number. Use prefix '0x' for hex and '0' for octal." << endl;
        }
        else if (s[0] == '0')
        {
            // string is octal
            if (ValidOperand(s.substr(1, len - 1)) == 3)
            {
                value = 0;
                int product = 1;
                for (int i = len - 1; i >= 1; i--)
                {
                    value += ((s[i] - '0') * product);
                    product *= 8;
                }
            }
            else
                log << "ERROR:Invalid format of number. Use prefix '0x' for hex and '0' for octal." << endl;
        }
        else
        {
            // string is decimal
            if (ValidOperand(s) == 3 || ValidOperand(s) == 2)
                value = stoi(s);
            else
                log << "ERROR:Invalid format of number. Use prefix '0x' for hex and '0' for octal." << endl;
        }
        return value;
    }

    return 0;
}

// function to calculate 2s complement with required no. of bita for a given decimal string
string deci_2scomp(int data, int n)
{
    const size_t BITS = n;
    char bin_str[BITS + 1];
    for (unsigned int i = 0; i < BITS; i++)
    {
        unsigned int mask = 1u << (BITS - 1 - i);
        bin_str[i] = (data & mask) ? '1' : '0';
    }
    bin_str[BITS] = '\0';

    string ret = "";
    for (int i = 0; i < BITS; i += 4)
    {
        string tmp = "";
        tmp += bin_str[i];
        tmp += bin_str[i + 1];
        tmp += bin_str[i + 2];
        tmp += bin_str[i + 3];
        ret += hexadeci[tmp];
    }
    return ret;
}

// function to implement SET instruction
void modify_labels_using_SET(fstream &log)
{
    for (int i = 0; i < 999999; i++)
    {
        if (source[i].empty())
            break;

        if (source[i].size() == 3 && source[i][1] == "SET" && label.find(source[i][0].substr(0, source[i][0].length() - 1)) != label.end())
        { //label followed by SET
            label[source[i][0].substr(0, source[i][0].length() - 1)] = string_to_deci(source[i][2], 32, log);
        }
        else if (source[i].size() == 4 && source[i][2] == "SET" && label.find(source[i][1].substr(0, source[i][1].length() - 1)) != label.end())
        { 
            label[source[i][1].substr(0, source[i][1].length() - 1)] = string_to_deci(source[i][3], 32, log);
        }
    }
}

// function to convert binary string to decimal number
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

// function to check and find the index of mnemonic present in mnemonics vector
int findMnemonic(string str)
{
    for (int i = 0; i < mnemonics.size(); i++)
    {
        if (mnemonics[i].mnem == str)
            return i;
    }
    return -1;
}

// function to convert a number into hex format
string toHex(int num)
{
    const string table = "0123456789abcdef"; // lookup hexadecimal
    unsigned int x = num;                    // make negative complement
    string r = "";
    while (x > 0)
    {
        int y = x % 16;   // get remainder
        r = table[y] + r; // concatenate in the reverse order
        x /= 16;
    }
    return r == "" ? "0" : r;
}

// function to convert string into int
int stringToNum(string str)
{
    stringstream ss(str);
    int x = 0;
    ss >> x;
    return x;
}
