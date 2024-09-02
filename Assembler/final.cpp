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

    // constructor
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
        for(currLine = 0 ; currLine < text.size() ; currLine++){
            ll ind = text[currLine].find(":");
            
            //removing label from the line for simplicity
            if(ind != -1){
                string lab = text[currLine].substr(0,ind);
                if(labels.find(lab)!=labels.end()) exitFunc(6);
                labels[lab] = currLine;
                text[currLine] = text[currLine].substr(ind+1);
                ltrim(text[currLine]);
            }
        }

        //compiling each line
        for(currLine = 0 ; currLine < text.size() ; currLine++){
            trim(text[currLine]);
            line_break(text[currLine]);
        }
    }

    void line_break(string line){
        // Finding the command
        ll ind = line.find(' ');
        string com = line.substr(0,ind);
        line = line.substr(ind+1);
        ltrim(line);
        
        char l = com[com.size()-1];
        if((l>='0'&&l<='9')||(l==',')) exitFunc(1);

        if(com=="lui"||com=="auipc"){
            // U-format
            uFormat(line,com);
        }else if(com[0]=='l'||l=='i'||com=="jalr"){
            // I-format
            iFormat(line,com);
        }else if(com[0]=='s' && com.length()==2){
            // S-format
            sFormat(line,com);
        }else if(com[0]=='b'){
            // B-format
            bFormat(line,com);
        }else if(com[0]=='j'){
            // J-format
            jFormat(line,com);
        }else{
            // R-format
            rFormat(line,com);
        }
    }

    // B-format
    void bFormat(string line, string com){
        string rs1, rs2, imm, func3, opCode = "1100011", RS1, RS2, immN;

        ll ind = line.find(','), num;
        rs1 = line.substr(0,ind);
        trim(rs1);

        if(line.size()<=ind) exitFunc(2);
        line = line.substr(ind+1);

        ind = line.find(',');
        rs2 = line.substr(0,ind);
        trim(rs2);

        if(line.size()<=ind) exitFunc(4);
        imm = line.substr(ind+1);
        trim(imm);

        if(reg_alias.find(rs1)==reg_alias.end()||reg_alias.find(rs2)==reg_alias.end()) exitFunc(2);
        RS1 = reg_alias[rs1];
        RS2 = reg_alias[rs2];
        
        if(labels.find(imm)!=labels.end()){
            ll j = labels[imm];
            num = 4*(j-currLine);
        }else{
            if(!checkImm(imm)) exitFunc(5);
            num = stoi(imm);
        }
        immN = signedIntToBin(num,13);
        if(com=="beq") func3 = "000";
        else if(com=="bne") func3 = "001";
        else if(com=="blt") func3 = "100";
        else if(com=="bge") func3 = "101";
        else if(com=="bltu") func3 = "110";
        else if(com=="bgeu") func3 = "111";
        else exitFunc(1);

        string hex = binToHex(immN[0]+immN.substr(2,6)+RS2+RS1+func3+immN.substr(8,4)+immN[1]+opCode,8);
        output.push_back(hex);
    }

    // U-format
    void uFormat(string line, string com){
        if(com!="lui") exitFunc(1);
        string opCode = "0110111", rd, RD, immN, imm, hex;
        
        ll ind = line.find(',');
        rd = line.substr(0,ind);
        trim(rd);
        if(line.size()<=ind) exitFunc(4);
        immN = line.substr(ind+1);
        trim(immN);

        if(reg_alias.find(rd)==reg_alias.end()) exitFunc(2);
        RD = reg_alias[rd];

        if(immN[0]=='-') exitFunc(3);
        if(immN.size()>2 && immN.substr(0,2)=="0x"){
            if(immN.size()>10||immN.size()<3) exitFunc(3); 
            imm = immN.substr(2);
            for(int i = 0 ; i < imm.size() ; i++){
                if((imm[i]<'0'||imm[i]>'9')&&(imm[i]<'a'||imm[i]>'f')) exitFunc(5);
            }
            while(imm.size()>5) imm = imm.substr(1);
            while(imm.size()<5) imm = "0"+imm;
            hex = binToHex(RD+opCode,3);
            hex = imm+hex;
        }else{

            if(!checkImm(immN)) exitFunc(5);

            ll t1 = 2*(INT_MAX+1ll)-1ll;
            ll num = 0;
            for(int i = 0 ; i < immN.size() ; i++){
                num*=10;
                num += (immN[i]-'0');
            }

            if(num>t1) exitFunc(3);
            if(num>INT_MAX){
                num = num-INT_MAX-1;
            }

            imm = signedIntToBin(num,32);
            hex = binToHex(imm.substr(12,20)+RD+opCode,8);
        }
        output.push_back(hex);
    }

    // J-format
    void jFormat(string line, string com){
        if(com!="jal")  exitFunc(1);
        string func3, rd, RD, immN, imm, hex, opCode = "1101111";

        ll ind = line.find(',');
        rd = line.substr(0,ind);
        trim(rd);
        if(line.size()<=ind) exitFunc(4);
        immN = line.substr(ind+1);
        trim(immN);
        if(immN=="") exitFunc(4);
        ll num;
        if(labels.find(immN)!=labels.end()){
            num = 4*(labels[immN]-currLine);
        }else{
            if(!checkImm(immN)) exitFunc(5);
            num = stoi(immN);
        }

        imm = signedIntToBin(num,21);
        if(reg_alias.find(rd)==reg_alias.end()) exitFunc(2);
        RD = reg_alias[rd];

        hex = binToHex(imm[0]+imm.substr(10,10)+imm[9]+imm.substr(1,8)+RD+opCode,8);
        output.push_back(hex);
    }

    // S-format
    void sFormat(string line, string com){
        // Commands included: sd, sw, sh, sb
        string func3, opCode = "0100011", rs1, rs2, immN;
        string RS1, RS2, imm, hex;
        ll ind;
        
        ind = line.find(',');
        rs2 = line.substr(0,ind);
        if(line.size()<=ind) exitFunc(2);
        line = line.substr(ind+1);
        
        ltrim(line);
        ind = line.find('(');
        immN = line.substr(0,ind);
        if(immN=="") exitFunc(4);
        if(line.size()<=ind+1 || line.find(')')==-1) exitFunc(2);
        line = line.substr(ind+1);
        
        rs1 = line.substr(0,line.size()-1);

        if(!checkImm(immN)) exitFunc(5);

        if(com=="sb") func3 = "000";
        else if(com=="sh") func3 = "001";
        else if(com=="sw") func3 = "010";
        else if(com=="sd") func3 = "011";

        if(reg_alias.find(rs1)==reg_alias.end() || reg_alias.find(rs2)==reg_alias.end()) exitFunc(2);
        RS1 = reg_alias[rs1];
        RS2 = reg_alias[rs2];
        imm = signedIntToBin(stoi(immN),12);

        
        hex = binToHex(imm.substr(0,7)+RS2+RS1+func3+imm.substr(7)+opCode,8);
        output.push_back(hex);
    }

    // I-format
    void iFormat(string line, string com){
        string rd, func3, rs1, imm, immN, opCode, func6, hex;
        string RD, RS1;
        ll ind;
        
        ind = line.find(',');
        rd = line.substr(0,ind);
        trim(rd);
        if(line.size()<=ind) exitFunc(2);
        line = line.substr(ind+1);
        ltrim(line);

        if(com=="jalr"){
            func3 = "000";
            opCode = "1100111";

            ind = line.find(',');
            rs1 = line.substr(0,ind);
            rtrim(rs1);
            if(line.size()<=ind) exitFunc(2);
            immN = line.substr(ind+1);
            trim(immN);
            
            imm = signedIntToBin(stoi(immN),12);

            if(reg_alias.find(rd)==reg_alias.end() || reg_alias.find(rs1)==reg_alias.end()) exitFunc(2);
            if(!checkImm(immN)) exitFunc(5);
            RD = reg_alias[rd];
            RS1 = reg_alias[rs1];

            hex = binToHex(imm+RS1+func3+RD+opCode,8);
        }else if(com[0]=='l'){
            opCode = "0000011";

            ind = line.find('(');
            if(reg_alias.find(rd)==reg_alias.end()) exitFunc(2);
            RD = reg_alias[rd];
            if(ind==-1){
                RS1 = RD;
                immN = line;
            }else{
                immN = line.substr(0,ind);
                trim(immN);
                if(immN=="") exitFunc(4);
                if(line.size()<=ind+1 || line.find(')')==-1) exitFunc(2); 
                rs1 = line.substr(ind+1);
                rs1 = rs1.substr(0,rs1.size()-1);
                ltrim(rs1);
                if(reg_alias.find(rs1)==reg_alias.end()) exitFunc(2);
                RS1 = reg_alias[rs1];
            }

            if(!checkImm(immN)) exitFunc(5);

            if(com[com.size()-1]=='u'){
                imm = signedIntToBin(stoi(immN),13).substr(1);
                    if(com=="lbu") func3 = "100";
                    else if(com=="lhu") func3 = "101";
                    else if(com=="lwu") func3 = "110";
                    else exitFunc(1);
            }else{
                imm = signedIntToBin(stoi(immN),12);
                if(com=="lb") func3 = "000";
                else if(com=="lh") func3 = "001";
                else if(com=="lw") func3 = "010";
                else if(com=="ld") func3 = "011";
                else exitFunc(1);
            }

            hex = binToHex(imm+RS1+func3+RD+opCode,8);
        }else{
            
            opCode = "0010011";
            ind = line.find(',');
            rs1 = line.substr(0,ind);
            
            if(line.size()<=ind) exitFunc(2);
            immN = line.substr(ind+1);
            trim(immN);

            if(reg_alias.find(rd)==reg_alias.end() || reg_alias.find(rs1)==reg_alias.end()) exitFunc(2);
            if(!checkImm(immN)) exitFunc(5);
            RD = reg_alias[rd];
            RS1 = reg_alias[rs1];

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
                }else{
                    exitFunc(1);
                }
                imm = signedIntToBin(stoi(immN),6);
                imm = func6+imm;
            }else{
                if(com=="addi") func3 = "000";
                else if(com=="xori") func3 = "100";
                else if(com=="ori") func3 = "110";
                else if(com=="andi") func3 = "111";
                else exitFunc(1);
                imm = signedIntToBin(stoi(immN),12);
            }
            hex = binToHex(imm+RS1+func3+RD+opCode,8);
        }
        output.push_back(hex);
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

        if(reg_alias.find(rs1)==reg_alias.end() || reg_alias.find(rs2)==reg_alias.end() || reg_alias.find(rd)==reg_alias.end()){
            exitFunc(2);
        }

        RS1 = reg_alias[rs1];
        RS2 = reg_alias[rs2];
        RD = reg_alias[rd];

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
        string hex = binToHex(func7+RS2+RS1+func3+RD+opCode,8);
        output.push_back(hex);
    }

    // exit compilation in case invalid command is called
    void exitFunc(int n){
        string err = ": ";
        
        switch (n){
        case 1:
            err += "command not found";
            break;
        case 2:
            err += "register not found";
            break;
        case 3:
            err += "immediate value exceeds limit";
            break;
        case 4:
            err += "immediate value not found";
            break;
        case 5:
            err += "incorrect immediate value";
            break;
        case 6:
            err += "multiple labels defined";
        }
        cout<<"Error in line "<<(currLine+1)<<err;
        exit(0);
    }

    // For final step
    string binToHex(string str, ll n){
        string temp, res = "";
        for(int i = 0 ; i < n ; i++){
            temp = str.substr(4*i,4);
            res = res + hexChar[temp];
        }
        return res;
    }
    
    string signedIntToBin(ll n, ll b){
        string res = ""; 
        
        ll lim = ((ll)1<<(b-1));
        
        if(lim<=n || -lim>n){
            exitFunc(3);
        }
        bool bFlag = 0;
        ll temp = ((ll)1<<(b-1));

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

    bool checkImm(string imm){
        for(int i = imm[0]=='-'?1:0 ; i < imm.length() ; i++) 
            if(imm[i]<'0' || imm[i]>'9') return 0;
        return 1;
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
    ifstream inputFile("input.s");
    vector<string> text;
    string line;
    while(getline(inputFile,line)){
        text.push_back(line);
    }
    inputFile.close();

    RiscVCompiler obj;
    
    obj.compile(text);
    for(auto it:obj.output){
        cout<<it<<endl;
    }

    return 0;
}