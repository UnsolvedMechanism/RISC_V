#include<bits/stdc++.h>
using namespace std;

class RiscVCompiler{
public:
    vector<string> output;
    
    vector<string> reg;
    map<string,string> reg_alias;
    map<string,int> labels;
    map<string,char> hexChar;
    // map<int,string> intString;

    // Compiler
    RiscVCompiler(){
        
        //setting up hexChar
        for(int i = 0 ; i < 10 ; i++){
            string temp = intToBinStr(i).substr(1);
            hexChar[temp] = i+'0';
        }
        for(int i = 10 ; i < 16 ; i++){
            string temp = intToBinStr(i).substr(1);
            hexChar[temp] = i-10+'a';
        }
        //allocating registers representation
        reg.resize(32);
        for(int i = 0 ; i < 32 ; i++){
            reg[i] = intToBinStr(i);
        }

        for(int i = 0 ; i < 32 ; i++){
            string temp = "x";
            temp += to_string(i);
            // cout<<temp<<endl;
            reg_alias[temp] = reg[i];
        }

        //assigning value for aliases
        reg_alias["zero"] = reg[0]; //zero constant
        reg_alias["ra"] = reg[1];   //return address
        reg_alias["sp"] = reg[2];   //stack pointer
        reg_alias["gp"] = reg[3];   //global pointer
        reg_alias["tp"] = reg[4];   //thread pointer

        //temporaries
        reg_alias["t0"] = reg[5];   
        reg_alias["t1"] = reg[6];
        reg_alias["t2"] = reg[7];
        for(char ch = '3' ; ch < '7' ; ch++){
            string temp = "t";
            temp += ch;
            reg_alias[temp] = reg[ch-'3'+28];
        }

        //saved registers
        reg_alias["s0"] = reg[8];   //saved pointer
        reg_alias["fp"] = reg[8];   //frame pointer
        reg_alias["s1"] = reg[9];   
        for(int ch = '2' ; ch <= '9' ; ch++){
            string temp = "s"; temp+=ch;
            reg_alias[temp] = reg[ch-'2'+18];
        }
        reg_alias["s10"] = reg[26];
        reg_alias["s11"] = reg[27];

        //fn args/return values
        for(char ch = '0' ; ch < '8' ; ch++){
            string temp = "a"; temp+=ch;
            reg_alias[temp] = reg[ch-'0'+10];
        }
    }

    void compile(vector<string> text){
        reset();
        
        //checking for labels
        for(int i = 0 ; i < text.size() ; i++){
            int ind = text[i].find(":");
            // cout<<text[i]<<endl;
            //removing label from the line for simplicity
            if(ind != -1){
                labels[text[i].substr(0,ind)] = i;
                text[i] = text[i].substr(ind+1);
                ltrim(text[i]);
            }
        }
        // check_LabelsText(text);

        //compiling each line
        for(int i = 0 ; i < text.size() ; i++){
            trim(text[i]);
            // cout<<text[i]<<endl;
            line_break(text[i]);
        }
    }


private:

    void line_break(string line){
        int ind = line.find(' ');
        string com = line.substr(0,ind);
        line = line.substr(ind+1);
        ltrim(line);
        // cout<<com<<endl;
        // cout<<line<<endl;
        
        if(com[0]=='l'||com[com.size()-1]=='i'){
            // I-format
            cout<<"iFormat"<<endl;
            iFormat(line,com);
        }else if(com[0]=='s' && com.length()==2){
            // S-format
        }else if(com[0]=='b'){
            // B-format
        }else if(com[0]=='j'){
            // J-format
        }else if(com=="lui"||com=="auipc"){
            // U-format
        }else{
            // R-format
            cout<<"rFormat"<<endl;
            rFormat(line,com);
        }
    }

    void iFormat(string line, string com){
        string rd, func3, rs1, imm, immN, opCode, func6;
        string RD, RS1;
        int ind;
        if(com[0]=='l'){
            //load functions
        }else{
            ind = line.find(',');
            rd = line.substr(0,ind);
            
            line = line.substr(ind+1);
            ltrim(line);
            ind = line.find(',');
            rs1 = line.substr(0,ind);

            immN = line.substr(ind+1);
            trim(immN);

            cout<<com<<" "<<rd<<" "<<rs1<<" "<<immN<<endl;
            
            opCode = "0010011";
            RD = reg_alias[rd];
            RS1 = reg_alias[rd];
            immN = intToBinStr(stoi(immN));

            if(com=="addi"){
                func3 = "000";
                func6 = "000000";
            }else if(com=="xori"){
                func3 = "100";
                func6 = "000000";
            }else if(com=="ori"){
                func3 = "110";
                func6 = "000000";
            }else if(com=="andi"){
                func3 = "111";
                func6 = "000000";
            }else if(com=="slli"){
                func3 = "001";
                func6 = "000000";
            }else if(com=="srli"){
                func3 = "101";
                func6 = "000000";
            }else if(com=="srai"){
                func3 = "101";
                func6 = "010000";
            }
            
        }
    }

    // R-format
    void rFormat(string line, string com){
        string opCode = "0110011";
        string func3, func7;
        // cout<<line<<endl;
        int ind = line.find(',');
        string rd = line.substr(0,ind);
        
        line = line.substr(ind+1);
        ltrim(line);
        ind = line.find(',');
        string rs1 = line.substr(0,ind);

        line = line.substr(ind+1);
        trim(line);
        string rs2 = line;

        cout<<com<<" "<<rd<<" "<<rs1<<" "<<rs2<<endl;

        string RS1 = reg_alias[rs1];
        string RS2 = reg_alias[rs2];
        string RD = reg_alias[rd];

        cout<<RD<<" "<<RS1<<" "<<RS2<<endl;
        if(com=="add"){
            func3 = "000";
            func7 = "0000000";
        }else if(com=="sub"){
            func3 = "000";
            func7 = "0100000";
        }else if(com=="or"){
            func3 = "110";
            func7 = "0000000";
        }else if(com=="and"){
            func3 = "111";
            func7 = "0000000";
        }else if(com=="xor"){
            func3 = "100";
            func7 = "0000000";
        }else if(com=="sll"){
            func3 = "001";
            func7 = "0000000";
        }else if(com=="srl"){
            func3 = "101";
            func7 = "0000000";
        }else if(com=="sra"){
            func3 = "101";
            func7 = "0100000";
        }
        string hex = binToHex(func7+RS2+RS1+func3+RD+opCode);
        cout<<hex<<endl;
    }


    string binToHex(string str){
        // cout<<str<<endl;
        // return str;
        string temp, res = "";
        for(int i = 0 ; i < 8 ; i++){
            temp = str.substr(4*i,4);
            res = res + hexChar[temp];
            cout<<temp<<" ";
        }
        cout<<endl;
        return res;
    }
    
    string intToBinStr(int n){
        int b = 32;
        string res = "";
        while(b>1){
            b /= 2;
            if(n>=b){
                res = res + '1';
                n -= b;
            }else{
                res = res + '0';
            }
        }
        return res;
    }

    void reset(){
        output.clear();
        labels.clear();
    }

    inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    inline void trim(std::string &s) {
        rtrim(s);
        ltrim(s);
    }

};

int main(){

    ifstream inputFile("input.txt");
    vector<string> text;
    string line;
    while(getline(inputFile,line)){
        text.push_back(line);
    }
    inputFile.close();

    RiscVCompiler obj;

    // vector<string> t1 = {"add x3, x4, x7"};
    // obj.compile(t1);
    // beq x4, x7, L1";
    obj.compile(text);
    // // obj.printAlias();
    // cout<<obj.reg[0]<<endl;
    return 0;
}