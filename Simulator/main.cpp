#include<bits/stdc++.h>
using namespace std;
#define ll long long

class RiscVAssembler{
public:
    vector<string> output;

    ll currLine;
    vector<string> reg;
    map<string,string> reg_alias;
    map<string,ll> labels;
    map<string,char> hexChar;
    // map<int,string> intString;

    // constructor
    RiscVAssembler(){
        currLine = -1;
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
        writeInFIle();
    }

private:
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
                imm = signedIntToBin(stoi(immN),12);
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
                imm = signedIntToBin(stoi(immN),7).substr(1);
                if(immN[0]=='-') exitFunc(3);
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
        writeInFIle();
        
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

    void writeInFIle(){
        ofstream myFile("output.hex");
        if(myFile.is_open()){
            for(auto it:output){
                myFile<<it<<endl;
            }
            cout<<"Hex codes stored in output.hex file"<<endl;
            myFile.close();
        }else{
            cout<<"Error in creating an output file"<<endl;
        }
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

class Simulator{
    
public:
    RiscVAssembler obj;
    vector<int> instructions;
    vector<long long> regs;
    vector<string> text; //stores the risc v code line by line
    vector<bitset<8>> memBlock;
    ll PC;

    Simulator(){
        PC = 0;
        regs.resize(32);
        memBlock.resize(0x40000);
        for(int i = 0 ; i < 32 ; i++){
            regs[i] = 0;
        }
    }

    void simulate(){
        string com;
        while(1){
            cin>>com;
            cout<<com<<endl;
            if(com=="load"){
                loadFile();
            }else if(com == "run"){
                runFile();
            }else if(com=="exit"){
                cout<<"Exiting the simulator"<<endl;
                exit(0);
            }else if(com=="regs"){
                printRegs();
            }else if(com=="step"){
                runSingleLine();
            }else if(com=="mem"){
                printMem();
            }
            cout<<endl;
        }
    }

    void printMem(){
        ll add, count;
        cin>>hex>>add;
        cin>>count;
        for(int i = 0 ; i < count ; i++){
            cout<<"Memory["<<hex<<add+i<<"] = 0x"<<hex<<bitsetToNum(memBlock[add+i-0x10000])<<endl;
        }
    }

    void printRegs(){
        cout<<"Registers:"<<endl;
        for(int i = 0 ; i < 32 ; i++){
            cout<<"x"<<to_string(i)<<" = 0x"<<hex<<regs[i]<<endl;
        }
    }

    void runSingleLine(){
        ll opCode = (instructions[PC/4]&0x7f);
        cout<<"Executed: "<<text[PC/4]<<"; PC = 0x"<<setfill('0')<<setw(8)<<hex<<PC<<endl;
        // cout<<"Executed: "<<text[PC]<<"; PC = 0x"<<setfill('0')<<setw(8)<<hex<<4*PC<<endl;
        if(opCode==0b0110011){
            // cout<<"rInst"<<endl;
            rInst(instructions[PC/4]>>7);
        }else if(opCode==0b0010011){
            // cout<<"iInst"<<endl;
            iInst(instructions[PC/4]>>7);
        }else if(opCode==0b0000011){
            // cout<<"iInst_load"<<endl;
            loadInst(instructions[PC/4]>>7);
        }else if(opCode==0b0100011){
            cout<<"iInst_store"<<endl;
            storeInst(instructions[PC/4]>>7);
        }else if(opCode==0b1100011){
            // cout<<"bInst"<<endl;
            bInst(instructions[PC/4]>>7);
        }else if(opCode==0b1101111){
            // cout<<"jal"<<endl;
            jInst(instructions[PC/4]>>7);
        }else if(opCode==0b1100111){
            // cout<<"jalr"<<endl;
            jalrInst(instructions[PC/4]>>7);
        }else if(opCode==0b0110111){
            // cout<<"lui"<<endl;
            luiInst(instructions[PC/4]>>7);
        }
        PC+=4;
    }

    void runFile(){
        ll opCode;
        while(PC/4<instructions.size()){
            runSingleLine();
        }
    }

    void storeInst(ll inst){
        ll imm = (inst&0b11111);
        inst = inst>>5;
        ll func3 = (inst&0b111);
        inst = inst>>3;
        ll rs1 = (inst&0b11111);
        inst = inst>>5;
        ll rs2 = (inst&0b11111);
        inst = inst>>5;
        imm += (inst<<5);

        cout<<rs1<<endl;
        cout<<rs2<<endl;
        cout<<imm<<endl;

        switch (func3){
            case 0x0:
                storeMem(regs[rs1]+imm,regs[rs2],1);
                break;
            case 0x1:
                storeMem(regs[rs1]+imm,regs[rs2],2);
                break;
            case 0x2:
                storeMem(regs[rs1]+imm,regs[rs2],4);
                break;
            case 0x3:
                storeMem(regs[rs1]+imm,regs[rs2],8);
                break;
        }
    }

    void storeMem(ll add, ll num, ll n){
        cout<<add<<" "<<num<<" "<<n<<endl;
        for(int i = 0 ; i < n ; i++){
            memBlock[add-0x10000+i] = (num&0xff);
            cout<<"Stored at address:"<<add+i<<endl;
            num = num>>16;
        }
    }

    void loadInst(ll inst){
        int rd = (inst&0b11111);
        inst = inst>>5;
        int func3 = (inst&0b111);
        inst = inst>>3;
        int rs1 = (inst&0b11111);
        inst = inst>>5;
        int imm = inst;

        if(rd==0) return;

        switch(func3){
            case 0x0:
                // cout<<getNum(regs[rs1]+imm,1)<<endl;
                regs[rd] = getNum(regs[rs1]+imm,1);
                break;
            case 0x1:
                // cout<<getNum(regs[rs1]+imm,2)<<endl;
                regs[rd] = getNum(regs[rs1]+imm,2);
                break;
            case 0x2:
                // cout<<getNum(regs[rs1]+imm,4)<<endl;
                regs[rd] = getNum(regs[rs1]+imm,4);
                break;
            case 0x3:
                // cout<<getNum(regs[rs1]+imm,8)<<endl;
                regs[rd] = getNum(regs[rs1]+imm,8);
                break;
            case 0x4:
                // cout<<getNum(regs[rs1]+imm,1)<<endl;
                regs[rd] = (getNum(regs[rs1]+imm,1)&0xf);
                break;
            case 0x5:
                // cout<<getNum(regs[rs1]+imm,2)<<endl;
                regs[rd] = (getNum(regs[rs1]+imm,2)&0xff);
                break;
            case 0x6:
                // cout<<getNum(regs[rs1]+imm,4)<<endl;
                regs[rd] = (getNum(regs[rs1]+imm,4)&0xffff);
                break;
        }
    }

    void luiInst(ll inst){
        int rd = (inst&0b11111);
        inst = inst>>5;
        
        ll imm = inst<<12;

        if(rd!=0) regs[rd] = imm;
    }

    void jInst(ll inst){
        int rd = (inst&0b11111);
        inst = inst >> 5;

        ll imm = (inst&0xff);
        inst = inst >> 8;
        imm = imm << 11;
        
        ll imm11 = (inst&0b1);
        inst = inst >> 1;
        imm11 = imm11 << 10;
        imm = imm | imm11;

        ll imm10_1 = (inst&0x3ff);
        inst = inst >> 10;
        imm = imm | imm10_1;

        ll imm20 = (inst&0b1);
        imm = imm << 1;

        if (imm20 == 0b1){
            imm = (imm^0xfffff);
            imm += 0b1;
            imm = 0 - imm;
        }
        // storing the value of PC+4 in rd and then I'll increase PC
        if (rd != 0b00000){
            regs[rd] = PC + 4;
        }
        PC += imm;
        PC -= 4;

        cout<<"jal format imm:"<<imm<<endl;        
    }

    void jalrInst(ll inst){
        int rd = (inst&0b11111);
        inst = inst>>5;
        int func3 = (inst&0b111);
        inst = inst>>3;
        int rs1 = (inst&0b11111);
        inst = inst>>5;
        int imm = inst;

        if(rd!=0){
            regs[rd] = PC+4;
        }
        PC = regs[rs1]+imm;
    }

    void bInst(ll inst){
        ll imm = (inst&0b1);
        imm = imm << 10;
        inst = inst >> 1;
        imm = imm|(inst&0b1111);
        inst = inst >> 4;

        int func3 = (inst&0b111);
        inst = inst >> 3;

        ll rs1 = (inst&0b11111);
        inst = inst >> 5;

        ll rs2 = (inst&0b11111);
        inst = inst >> 5;

        ll imm10_5 = (inst&0b111111);
        inst = inst >> 6;

        imm10_5 = imm10_5 << 4;

        imm = imm|imm10_5;

        imm = imm << 1;

        int imm12 = (inst&0b1);
        
        if (imm12 == 0b1){
            imm = (imm^0xfff);
            imm += 0b1;
            imm = 0 - imm;
        }

        switch(func3){
            case 0x0:
                if (regs[rs1] == regs[rs2]){
                    PC +=imm-4;
                }
                break;
            case 0x1:
                if (regs[rs1] != regs[rs2]){
                    PC +=imm-4;
                }
                break;
            case 0x4:
                if (regs[rs1] < regs[rs2]){
                    PC +=imm-4;
                }
                break;
            case 0x5:
                if (regs[rs1] >= regs[rs2]){
                    PC +=imm-4;
                }
                break;
            case 0x6:
                if (abs(regs[rs1]) < abs(regs[rs2])){
                    PC +=imm-4;
                }
                break;
            case 0x7:
                if(abs(regs[rs1]) >= abs(regs[rs2])){
                    PC +=imm-4;
                }
                break;      
        }
        cout<<"Branch format imm:"<<imm<<endl;
    }

    void iInst(ll inst){
        int rd = (inst&0b11111);
        inst = inst>>5;
        int func3 = (inst&0b111);
        inst = inst>>3;
        int rs1 = (inst&0b11111);
        inst = inst>>5;
        int imm = inst;

        if(rd==0) return;

        switch(func3){
            case 0x0:
                regs[rd] = (regs[rs1]+inst);
                break;
            case 0x4:
                regs[rd] = (regs[rs1]^inst);
                break;
            case 0x6:
                regs[rd] = (regs[rs1]|inst);
                break;
            case 0x7:
                regs[rd] = (regs[rs1]&inst);
                break;
            case 0x1:
                regs[rd] = (regs[rs1]<<inst);
                break;
            case 0x5:
                if((inst>>6)>0){
                    regs[rd] = (regs[rs1]>>(inst&0b111111));
                }else{
                    regs[rd] = (regs[rs1]>>(inst&0b111111));
                }
                break;
        }
    }

    void rInst(ll inst){
        int rd = (inst&0b11111);
        inst = inst>>5;
        int func3 = (inst&0b111);
        inst = inst>>3;
        int rs1 = (inst&0b11111);
        inst = inst>>5;
        int rs2 = (inst&0b11111);
        inst = inst>>5;

        if(rd==0) return;

        switch(inst){
            case 0x00:
                switch(func3){
                    case 0x0:
                        regs[rd] = (regs[rs1]+regs[rs2]);
                        break;
                    case 0x4:
                        regs[rd] = (regs[rs1]^regs[rs2]);
                        break;
                    case 0x6:
                        regs[rd] = (regs[rs1]|regs[rs2]);
                        break;
                    case 0x7:
                        regs[rd] = (regs[rs1]&regs[rs2]);
                        break;
                    case 0x1:
                        regs[rd] = (regs[rs1]<<regs[rs2]);
                        break;
                    case 0x5:
                        regs[rd] = (regs[rs1]>>regs[rs2]);
                        break;
                }
                break;
            case 0x20:
                switch(func3){
                    case 0x0:
                        regs[rd] = (regs[rs1]-regs[rs2]);
                        break;
                    case 0x5:
                        regs[rd] = (regs[rs1]>>regs[rs2]);
                        break;
                }
                break;
        }
    }

    void loadFile(){
        string fileName;
        cin>>fileName;

        reset();

        ifstream inputFile(fileName);
        string line;
        while(getline(inputFile,line)){
            text.push_back(line);
        }
        inputFile.close();

    
        obj.compile(text);

        ifstream file("output.hex");
        
        if (!file.is_open()) {
            std::cerr << "Unable to open file!" << std::endl;
            exit(0);
        }

        while (std::getline(file, line)) {
            std::stringstream ss;
            ll value;

            ss << std::hex << line;
            ss >> value;

            instructions.push_back(value);
        }
        
        file.close();

        std::cout << "Hex values converted to integers:" << std::endl;
        for (const auto& val : instructions) {
            cout <<std::hex<< val << endl;
        }
    }

    ll getNum(ll add, ll n){
        ll res = 0, temp;
        cout<<add<<" "<<n<<endl;
        for(int i = 0 ; i < n ; i++){
            temp = bitsetToNum(memBlock[add+i]);
            res += (res<<(8*i))+temp;
            cout<<res<<endl;
        }
        return res;
    }

    ll bitsetToNum(bitset<8> b){
        ll num = 0;
        for(int i = 0 ; i < 8 ; i++){
            num += (1<<(i))*b[i];
        }
        return num;
    }

    void reset(){
        PC = 0;
        for(int i = 0 ; i < regs.size() ; i++){
            regs[i] = 0;
        }
        text.clear();
        instructions.clear();
    }
};

int main(){
    Simulator obj;
    obj.simulate();

    // ifstream inputFile("input.s");
    // vector<string> text;
    // string line;
    // while(getline(inputFile,line)){
    //     text.push_back(line);
    // }
    // inputFile.close();

    // RiscVAssembler obj;
    
    // obj.compile(text);
    return 0;
}