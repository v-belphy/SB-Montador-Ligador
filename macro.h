#include <bits/stdc++.h>
using namespace std;

/*
TABLES:
- MNT
- MDT

FORMAT IN INPUT:
[NAME]: MACRO
        [...]
        ENDMACRO
*/

map<string, array<int, 2>> MNT; // MNT = [name, number of arg, line in MDT]
vector<vector<string>> MDT;
map<string, vector<string>> macro_args;

void process_macro(vector<vector<string>> &macro_lines, string &macro_name){
    int begin_idx = MDT.size();
    for(int i=0; i<(int)(macro_lines.size()); i++) MDT.push_back(macro_lines[i]);
    MDT.push_back({"ENDMACRO"});
    MNT[macro_name] = {(int)(macro_args[macro_name].size()), begin_idx};
}

vector<vector<string>> expand_macro(vector<vector<string>> &new_file, vector<bool> &v_skip){
    vector<vector<string>> result;

    for(int i=0; i<(int)(new_file.size()); i++){
        if(v_skip[i]) continue;
        vector<string> vaux;
        for(int j=0; j<(int)(new_file[i].size()); j++){
            string word = new_file[i][j];
            if(MNT.count(word)){
                if(vaux.size()) result.push_back(vaux), vaux.clear();
                int arg = MNT[word][0];
                map<string, string> param;
                if((int)(new_file[i].size() - j - 1) != arg){
                    cerr << "Número de operandos incorreto para a macro: " << word << endl;
                    result.clear();
                    return result;
                }
                for(int k=0; k<arg; k++){
                    param[macro_args[word][k]] = new_file[i][j+k+1];
                }
                for(int k=MNT[word][1]; k<MDT.size(); k++){
                    if(MDT[k][0] == "ENDMACRO") break;
                    vector<string> vs;
                    for(auto it: MDT[k]) vs.push_back((param.count(it) ? param[it] : it));
                    result.push_back(vs);
                }
                break;
            } else {
                vaux.push_back(new_file[i][j]); 
            }
        }
        if(vaux.size()) result.push_back(vaux);
    }

}

vector<vector<string>> solve_macro(vector<vector<string>> &new_file){
    bool solving_macro = false;
    vector<vector<string>> macro_lines;
    string macro_name = "";

    // do MDT and MNT
    vector<bool> v_skip((int)(new_file.size()), false);
    for(int i=0; i<(int)(new_file.size()); i++){
        if(1 < (int)(new_file[i].size()) && new_file[i][1]== "MACRO"){
            v_skip[i] = true;
            solving_macro = true;
            macro_name = new_file[i][0].substr(0, (int)(new_file[i][0].size()) - 1);
            for(int j=2; j<(int)(new_file[i].size()); j++){
                macro_args[macro_name].push_back(new_file[i][j]);
            }
        } else if(new_file[i][0] == "ENDMACRO"){
            v_skip[i] = true;
            solving_macro = false;
            process_macro(macro_lines, macro_name);
            macro_lines.clear();
        } else {
            if(solving_macro) v_skip[i] = true, macro_lines.push_back(new_file[i]);
        }
    }

    // expand macro
    return expand_macro(new_file, v_skip);   
    
}