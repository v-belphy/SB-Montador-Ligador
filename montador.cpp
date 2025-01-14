#include <bits/stdc++.h>
#include "preprocessor.h"

using namespace std;

map<string, int> TS;
vector<pair<string, int>> Tuso;
set<string> externs;
map<string, int> Tdef;
map<string, array<int, 2>> instructions;
vector<vector<string>> codigo_gerado;
map<int, int> endereco_linha;
vector<int> relativos;
bool flag_ligar = false;

int hexa_neg_pos(string s) {
    bool msb;
    string val = s.substr(2, (int) s.size() - 2);
    /*int num = stoi(val, nullptr, 16);
    int len = val.size();
    int mask = 1 << ((len * 4) - 1);
    msb = mask & num;
    if (msb) {
        while (val.size() < 4) {
            string aux = "F";
            aux += val;
            val = aux;
        }
    }*/
    int norm = stoi(val, nullptr, 16);
    if (norm < (1 << 15)) {
        return norm;
    } else {
        return norm - (1 << 16);
    }
}

int transform_to_number(string &s){
    bool flag = true;
    for(int i=0; i<(int)(s.size()); i++){
        if(i == 0 && s[i] == '-'){continue;}
        if(s[i] >= '0' && s[i] <= '9'){continue;}
        flag = false;
    }
    if(flag) return stoi(s);
    if(s.size() > 6){cerr << "ERRO: [Overflow] Tamanho maximo e de 16 bits" << endl; return 0;}
    if((int)(s.size()) > 1){
        if(s[0] == '0' && s[1] == 'X'){
            flag = true;
            for(int i=2; i<(int)(s.size()); i++){
                if((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'A' && s[i] <= 'F')){continue;}
                flag = false;
            }
        }
    }
    if(flag){
        return hexa_neg_pos(s);
    }
    cerr << "ERRO: Operando nao e um numero" << endl;
    return 0;
}

bool is_a_number(string &s){
    for(int i=0; i<(int)(s.size()); i++){
        if(!(s[i] >= '0' && s[i] <= '9')) return false; 
    }
    return true;
}

pair<string, int> solve_vector(string &s){
    for(int i=0; i<(int)(s.size()); i++){
        if(s[i] == '+'){return {s.substr(0, i), stoi(s.substr(i+1))};} 
    }
    return {s.substr(0, (int)(s.size())), 0};
}

void set_instructions(){
    // instructions:
    // name = [op_code, size]
    instructions["ADD"] = {1, 2};
    instructions["SUB"] = {2, 2};
    instructions["MUL"] = {3, 2};
    instructions["MULT"] = {3, 2};
    instructions["DIV"] = {4, 2};
    instructions["JMP"] = {5, 2};
    instructions["JMPN"] = {6, 2};
    instructions["JMPP"] = {7, 2};
    instructions["JMPZ"] = {8, 2};
    instructions["COPY"] = {9, 3};
    instructions["LOAD"] = {10, 2};
    instructions["STORE"] = {11, 2};
    instructions["INPUT"] = {12, 2};
    instructions["OUTPUT"] = {13, 2};
    instructions["STOP"] = {14, 1};
}

bool check_name_of_label(string &s){
    if(s[0] >= '0' && s[0] <= '9') return false;
    for(int i=0; i<(int)(s.size()); i++){
        if(!((s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= '0' && s[i] <= '9') || s[i] == '_')) return false; 
    }
    return true;
}

void set_def(){
    if(Tdef.empty()) return;
    for(auto it: Tdef){
        if(TS.count(it.first)){
            Tdef[it.first] = TS[it.first];
        }
    }
    return;
}

vector<vector<string>> uso_def(vector<vector<string>> &file){
    vector<vector<string>> result;
    for(int i=0; i<(int)(file.size()); i++){
        vector<string> v;
        for(int j=0; j<(int)(file[i].size()); j++){
            if(file[i][j] == "BEGIN" || file[i][j] == "END"){flag_ligar = true; continue;}
            else if(file[i][j] == "EXTERN"){
                if(j-1<(int)(file[i].size())){
                    externs.insert(file[i][j-1].substr(0, (int)(file[i][j-1].size())-1));
                }
            } else if(file[i][j] == "PUBLIC"){
                if(j+1<(int)(file[i].size())){
                    Tdef[file[i][j+1]] = -1;
                    break;
                }
            } else {
                v.push_back(file[i][j]);
            }
        }
        if(!v.empty()) result.push_back(v);
    }
    return result;
}

int first_pass(vector<vector<string>> &file){
    int pos = 0;
    int prev_label = -1;
    int skip = 0;
    for(int i=0; i<(int)(file.size()); i++){
        if(file[i].size() == 2 && file[i][0] == "SECTION" && (file[i][1] == "DATA" || file[i][1] == "TEXT")) continue;
        for(int j=0; j<(int)(file[i].size()); j++){
            if(skip){
                if(externs.find(file[i][j]) != externs.end()){
                    Tuso.push_back({file[i][j], pos - skip});
                }
                skip--; continue;
            }
            string word = file[i][j];
            int n = word.size();
            if(word[n - 1] == ':'){
                string label = word.substr(0, n-1);
                if(!check_name_of_label(label)){
                    cerr << "ERRO: Rótulo com erro léxico (Apresenta um rótulo que começa com números ou apresenta um caracter especial diferente de '_' )" << endl;
                    return -1;
                } if(prev_label == i){
                    cerr << "ERRO: Mais de um rótulo na mesma linha" << endl;
                    return -1;
                } else if(TS.count(label)){
                    cerr << "ERRO: Rotulo " << label << " redefinido" << endl;
                    return -1;
                } else {
                    TS[label] = pos;
                    prev_label = i;
                }
            } else if(instructions.count(word)){
                pos += instructions[word][1];
                skip = instructions[word][1] - 1;
            } else if(word == "SPACE"){
                if(j + 1 < (int)(file[i].size())){
                    skip = 1, pos += stoi(file[i][j+1]);
                }
                else skip = 0, pos += 1;
            } else if(word == "CONST"){
                pos ++;
                skip = 1;
            } else {
                cerr << "Instrução ou diretiva: '" << word << "' inválida" << endl;
                return -1;
            }
        }
    }
    return 0;
}

int second_pass(vector<vector<string>> &file){
    int pos = 0, j;
    for(int i=0; i<(int)(file.size()); i++){
        if(file[i].size() == 2 && file[i][0] == "SECTION" && (file[i][1] == "DATA" || file[i][1] == "TEXT")) continue;
        for(j=0; j<(int)(file[i].size()); j++){
            string word = file[i][j];
            int n = word.size();
            if(word[n - 1] == ':') continue;
            vector<string> v;
            if(instructions.count(word)){
                int args = instructions[word][1];
                if(j + args > (int)(file[i].size())){
                    cerr << "Falta operandos para a instrução: " << word << endl;
                    return -1;
                }
                if(j + args < (int)(file[i].size())){
                    cerr << "Possui mais operandos do que o permitido na instrução: " << word << endl;
                    return -1;
                }
                while(j<(int)(file[i].size())){
                    int idx = 0;
                    tie(word, idx) = solve_vector(file[i][j]);
                    if(instructions.count(word)) v.push_back(to_string(instructions[word][0])), relativos.push_back(0);
                    else if(TS.count(word)) v.push_back(to_string(TS[word] + idx)), relativos.push_back(1);
                    else{
                        cerr << "Rotulo ausente: " << word << endl;
                        return -1;
                    }
                    j++;
                }
                codigo_gerado.push_back(v);
                endereco_linha[(int)(codigo_gerado.size()-1)] = pos;
                pos += args;
            } else if(word == "SPACE"){
                int arg = 1;
                if(j + 1 < (int)(file[i].size())){
                    if(!is_a_number(file[i][j+1])){
                        cerr << "Operando na diretiva '" << word << "' não é um número" << endl;
                        return -1;
                    }
                    arg = stoi(file[i][j+1]);
                    j++;
                }
                else if(j + 2 < (int)(file[i].size())){
                    cerr << "Possui mais operandos do que o permitido na diretiva: " << word << endl;
                    return -1;
                }
                while(arg--){
                    relativos.push_back(0);
                    v.push_back("00");
                    codigo_gerado.push_back(v);
                    v.clear();
                    endereco_linha[(int)(codigo_gerado.size()-1)] = pos++;
                }
            } else if(word == "CONST"){
                if(j + 1 >= (int)(file[i].size())){
                    cerr << "Falta operandos para a diretiva: " << word << endl;
                    return -1;
                }
                if(j + 2 < (int)(file[i].size())){
                    cerr << "Possui mais operandos do que o permitido na diretiva: " << word << endl;
                    return -1;
                }
                relativos.push_back(0);
                v.push_back(to_string(transform_to_number(file[i][j+1])));
                codigo_gerado.push_back(v);
                endereco_linha[(int)(codigo_gerado.size()-1)] = pos++;
                j++;
            } else {
                cerr << "Instrução ou diretiva: '" << word << "' inválida" << endl;
                return -1;
            }
        }
    }
    return 0;
}

int main(int argc, char** argv){
    if(argc == 1){cerr << "Favor passar o arquivo para o programa" << endl;  return 0;}
    string filename(argv[1]);
    string new_filename = filename.substr(0, (int)(filename.size())-4);
    string extension = filename.substr((int)(filename.size())-3);
    if(extension == "asm"){
        vector<vector<string>> file = preprocess_file(filename);
        ofstream arquivoSaida(new_filename + ".pre");
        if (!arquivoSaida.is_open()) {
            std::cerr << "Erro ao abrir o arquivo para escrita!" << std::endl;
            return 1;
        }

        for(int i=0; i<(int)(file.size()); i++){
            for(int j=0; j<(int)(file[i].size()); j++){
                if(j == (int)(file[i].size()) - 1) arquivoSaida << file[i][j] << endl;
                else arquivoSaida << file[i][j] << ' ';
            }
        }

        arquivoSaida.close();
    } else if(extension == "pre"){
        vector<vector<string>> file = preprocess_file(filename);
        file = uso_def(file);

        set_instructions();
        if(first_pass(file) == -1) return 0;
        if(second_pass(file) == -1) return 0;

        set_def();

        ofstream arquivoSaida(new_filename + ".obj");
        if (!arquivoSaida.is_open()) {
            std::cerr << "Erro ao abrir o arquivo para escrita!" << std::endl;
            return 1;
        }

        if(flag_ligar){

            for(auto it: Tdef){
                if(TS.count(it.first) == 0){cerr << "Label public nao foi declarada" << endl; return 0;}
            }

            for(auto it: Tdef){
                arquivoSaida << "D, " << it.first << ' ' << it.second << endl;
            }
            for(auto it: Tuso){
                arquivoSaida << "U, " << it.first << ' ' << it.second << endl;
            }
            arquivoSaida << "R, "; for(auto it: relativos){arquivoSaida << it << ' ';}
            arquivoSaida << endl;
        }
        for(int i=0; i<codigo_gerado.size(); i++){
            for(auto it: codigo_gerado[i]) arquivoSaida << it << ' ';
        }
        arquivoSaida << endl;
    
    } else {
        cout << "extensao invalida! somente é aceito extensoes de arquivos .asm e .pre" << endl;
    }

    return 0;
}