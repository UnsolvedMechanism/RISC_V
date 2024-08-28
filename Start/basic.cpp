#include<bits/stdc++.h>
using namespace std;

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

class RiscVCompiler{
public:
    // string text;
    vector<string> output;
    
    vector<int*> reg;
    map<string,int*> reg_alias;
    map<string,int> labels;
    map<int,string> intString;

    RiscVCompiler(){
        //allocating memory for registers
        reg.resize(32);
        for(int i = 0 ; i < 32 ; i++){
            reg[i] = (int*)malloc(sizeof(int));
            *reg[i] = i;
        }

        for(int i = 0 ; i < 32 ; i++){
            string temp = "x";
            temp += to_string(i);
            cout<<temp<<endl;
            reg_alias[temp] = reg[i];
        }

        //assigning memory address for aliases
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
    
    // to reset values before compilation
    void reset(){
        for(int i = 0 ; i < 32 ; i++){
            *reg[i] = 0;
        }
        output.clear();
        labels.clear();
    }

    void compile(vector<string> text){
        reset();
        
        //checking for labels
        for(int i = 0 ; i < text.size() ; i++){
            int ind = text[i].find(":");
            cout<<text[i]<<endl;
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
            line_break(text[i]);
        }
    }

    void line_break(string line){
        int ind = line.find(' ');
        string com = line.substr(0,ind);
        // cout<<com<<endl;
        line = line.substr(ind+1);
        ltrim(line);
        
        if(*com.begin()=='l'||*com.end()=='i'){
            // I-format
        }else if(*com.begin()=='s' && com.length()==2){
            // S-format
        }else if(*com.begin()=='b'){
            // B-format
        }else if(*com.begin()=='j'){
            // J-format
        }else if(com=="lui"||com=="auipc"){
            // U-format
        }else{
            // R-format
            rFormat(line,com);
        }
    }
    
    // R-format
    void rFormat(string line, string com){
        string opCode = "0110011";
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

        int RS1 = *reg_alias[rs1];
        if(com=="add"){

        }

    }

    //functions for testing purposes
    void printAlias(){
        for(auto it:reg_alias){
            cout<<it.first<<endl;
        }
    }
    void check_LabelsText(vector<string> text){
        for(auto it:text){
            cout<<it<<endl;
        }
        for(auto it:labels){
            cout<<it.first<<" "<<it.second<<endl;
        }
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
    // string text = "add x3, x4, x7 
    // beq x4, x7, L1";
    RiscVCompiler obj;
    obj.compile(text);
    // obj.printAlias();
    // cout<<obj.reg[0]<<endl;
    return 0;
}