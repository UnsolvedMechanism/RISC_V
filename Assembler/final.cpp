#include<bits/stdc++.h>
using namespace std;
#define ll long long

class RiscVCompiler{
public:
    vector<string> output;
    
    ll currLine = 0;
    vector<string> reg;
    map<string,string> reg_alias;
    map<string,ll> labels;
    map<string,char> hexChar;
    // map<int,string> intString;

    // Compiler
    RiscVCompiler(){
        
        //setting up hexChar
        for(ll i = 0 ; i < 10 ; i++){
            string temp = intToBinStr(i).substr(1);
            hexChar[temp] = i+'0';
        }
        for(ll i = 10 ; i < 16 ; i++){
            string temp = intToBinStr(i).substr(1);
            hexChar[temp] = i-10+'a';
        }
        //allocating registers representation
        reg.resize(32);
        for(ll i = 0 ; i < 32 ; i++){
            reg[i] = intToBinStr(i);
        }

        for(ll i = 0 ; i < 32 ; i++){
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
        for(ll ch = '2' ; ch <= '9' ; ch++){
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
        for(ll i = 0 ; i < text.size() ; i++){
            ll ind = text[i].find(":");
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
        for(currLine = 0 ; currLine < text.size() ; currLine++){
            trim(text[currLine]);
            // cout<<text[i]<<endl;
            line_break(text[currLine]);
            cout<<endl;
        }
    }

    void line_break(string line){
        ll ind = line.find(' ');
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
            cout<<"sFormat"<<endl;
            sFormat(line,com);
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

    // S-format
    void sFormat(string line, string com){
        // Commands included: sd, sw, sh, sb
        string func3, opCode = "0100011", rs1, rs2, immN;
        string RS1, RS2, imm, res;
        ll ind;
        // sd rs2, imm(rs1)
        ind = line.find(',');
        rs2 = line.substr(0,ind);
        line = line.substr(ind+1);
        
        ltrim(line);
        ind = line.find('(');
        immN = line.substr(0,ind);
        line = line.substr(ind+1);
        
        rs1 = line.substr(0,line.size()-1);

        if(com=="sb"){
            func3 = "000";
        }else if(com=="sh"){
            func3 = "001";
        }else if(com=="sw"){
            func3 = "010";
        }else if(com=="sd"){
            func3 = "011";
        }
        RS1 = reg_alias[rs1];
        RS2 = reg_alias[rs2];
        imm = signedIntToBin(stoi(immN),12);

        cout<<com<<" "<<rs2<<" "<<immN<<" "<<rs1<<endl;
        // cout<<imm<<endl;

        res = binToHex(imm.substr(0,7)+RS2+RS1+func3+imm.substr(7)+opCode);
        cout<<res<<endl;
    }

    // I-format
    void iFormat(string line, string com){
        string rd, func3, rs1, imm, immN, opCode, func6, hex;
        string RD, RS1;
        ll ind;

        ind = line.find(',');
        rd = line.substr(0,ind);
        line = line.substr(ind+1);
        ltrim(line);

        if(com[0]=='l'){
            opCode = "0000011";

            ind = line.find('(');
            RD = reg_alias[rd];
            if(ind==-1){
                RS1 = RD;
                immN = line;
            }else{
                immN = line.substr(0,ind);
                rs1 = line.substr(ind+1);
                rs1 = rs1.substr(0,rs1.size()-1);
                ltrim(rs1);
                RS1 = reg_alias[rs1];
            }

            if(com[com.size()-1]=='u'){
                imm = signedIntToBin(stoi(immN),13).substr(1);
                    if(com=="lbu"){
                        func3 = "100";
                    }else if(com=="lhu"){
                        func3 = "101";
                    }else if(com=="lwu"){
                        func3 = "110";
                    }
            }else{
                imm = signedIntToBin(stoi(immN),12);
                if(com=="lb"){
                    func3 = "000";
                }else if(com=="lh"){
                    func3 = "001";
                }else if(com=="lw"){
                    func3 = "010";
                }else if(com=="ld"){
                    func3 = "011";
                }

            }

            cout<<com<<" "<<rd<<" "<<rs1<<" "<<immN<<endl;
            cout<<RD<<" "<<RS1<<" "<<imm<<endl;

            hex = binToHex(imm+RS1+func3+RD+opCode);
            cout<<hex<<endl;
        }else{
            
            ind = line.find(',');
            rs1 = line.substr(0,ind);

            immN = line.substr(ind+1);
            trim(immN);

            cout<<com<<" "<<rd<<" "<<rs1<<" "<<immN<<endl;
            
            opCode = "0010011";
            RD = reg_alias[rd];
            RS1 = reg_alias[rs1];
            // immN = intToBinStr(stoi(immN));

            if(com[0]=='s'){
                if(com=="slli"){
                    func3 = "001";
                    func6 = "000000";
                }else if(com=="srli"){
                    func3 = "101";
                    func6 = "000000";
                }else if(com=="srai"){
                    func3 = "101";
                    func6 = "010000";
                }
                imm = signedIntToBin(stoi(immN),6);
                imm = func6+imm;
            }else{
                if(com=="addi"){
                    func3 = "000";
                }else if(com=="xori"){
                    func3 = "100";
                }else if(com=="ori"){
                    func3 = "110";
                }else if(com=="andi"){
                    func3 = "111";
                }
                imm = signedIntToBin(stoi(immN),12);
            }
            hex = binToHex(imm+RS1+func3+RD+opCode);
            cout<<hex<<endl;
        }
    }

    // R-format
    void rFormat(string line, string com){
        string opCode = "0110011";
        string func3, func7, RS1, RS2, RD;
        
        
        ll ind = line.find(',');
        string rd = line.substr(0,ind);
        
        if(line.size()<=ind) exitFunc(2);
        line = line.substr(ind+1);
        ltrim(line);
        ind = line.find(',');
        string rs1 = line.substr(0,ind);

        if(line.size()<=ind) exitFunc(2);
        line = line.substr(ind+1);
        trim(line);
        string rs2 = line;

        cout<<com<<" "<<rd<<" "<<rs1<<" "<<rs2<<endl;

        if(reg_alias.find(rs1)==reg_alias.end() || reg_alias.find(rs2)==reg_alias.end() || reg_alias.find(rd)==reg_alias.end()){
            exitFunc(2);
        }

        RS1 = reg_alias[rs1];
        RS2 = reg_alias[rs2];
        RD = reg_alias[rd];

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
        }else{
            exitFunc(1);
        }
        string hex = binToHex(func7+RS2+RS1+func3+RD+opCode);
        cout<<hex<<endl;
    }

    void exitFunc(int n){
        string err = ": ";
        
        switch (n){
        case 1:
            err += "command not found";
            break;
        case 2:
            err += "register missing";
            break;
        case 3:
            err += "immediate value exceeds limit";
            break;
        }
        cout<<"Error in line "<<(currLine+1)<<err;
        exit(0);
    }

    // For final step
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
    
    string signedIntToBin(ll n, ll b){
        string res = "";
        ll lim = (1<<(b-1));
        if(lim<=n || -lim>n){
            exitFunc(3);
        }
        bool bFlag = 0;
        ll temp = (1<<(b-1));

        if(n<0){
            n = (temp<<1)+n;    
        }

        while(temp){
            if(n>=temp){
                res = res+'1';
                n -= temp;
            }else{
                res = res+'0';
            }
            temp = (temp>>1);
        }
        return res;
    }

    string intToBinStr(int n){
        ll b = 32;
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
    
    // for(int i = -5 ; i <= 5 ; i++){
    //     cout<<obj.signedIntToBin(i,12)<<endl;
    // }

    // vector<string> t1 = {"add x3, x4, x7"};
    // obj.compile(t1);
    // beq x4, x7, L1";
    obj.compile(text);
    // // obj.printAlias();
    // cout<<obj.reg[0]<<endl;
    return 0;
}