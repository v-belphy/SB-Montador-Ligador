#include <bits/stdc++.h>
#include "macro.h"

using namespace std;

string remove_comments(string &s){
    string result = "";
    for(int i=0; i<(int)(s.size()); i++){
        if(s[i] == ';') break;
        result += s[i];
    }
    return result;    
}

string to_upper(string &s){
    string result = "";
    for(int i=0; i<(int)(s.size()); i++){
        result += toupper(s[i]);
    }
    return result;
}

string trim_spaces(string &s){
    // remove extra spaces (or ,) between words
    string aux = "";
    char prev = '#';
    for(int i=0; i<(int)(s.size()); i++){
        if((prev == ',' || prev == ' ' || prev == '\t') && (s[i] == ',' || s[i] == ' ' || s[i] == '\t')) continue;
        aux += s[i];
        prev = s[i];
    }
    s = aux;

    //if first char or last char is a space, remove it
    string result= "";
    bool fc_space = (s[0] == ' ' || s[0] == ',' || s[0] == '\t');
    bool lc_space = (s[(int)(s.size()) - 1] == ' ' || s[(int)(s.size()) - 1] == ',' || s[(int)(s.size()) - 1] == '\t');
    for(int i=(int)(fc_space); i<(int)(s.size()) - (int)(lc_space); i++){
        result += s[i];
    }
    return result;
}

vector<string> formatted_line(string &s){
    vector<string> vs;
    string word = "";
    for(int i=0; i<(int)(s.size()); i++){
        if(s[i] == ' ' || s[i] == ',') vs.push_back(word), word = "";
        else word += s[i];
    }
    vs.push_back(word);
    return vs;
}

vector<vector<string>> section_text_data(vector<vector<string>> &file){
    vector<vector<string>> result;

    int idx_data = -1, idx_text = -1;
    bool flag = false;
    for(int i=0; i<(int)(file.size()); i++){
        for(int j=0; j<(int)(file[i].size()); j++){
            if(j + 1 < (int)(file[i].size())){
                if(file[i][j] == "SECTION" && file[i][j+1] == "DATA") idx_data = i;
                if(file[i][j] == "SECTION" && file[i][j+1] == "TEXT") idx_text = i;
            }
        }
    }
    if(idx_data > idx_text || idx_data == -1 || idx_text == -1){
        for(int i=0; i<(int)(file.size()); i++){
            //if(i == idx_data || i == idx_text) continue;
            result.push_back(file[i]);
        }
    } else {
        bool flagend = false;
        for(int i=0; i<idx_data; i++) result.push_back(file[i]);
        for(int i=idx_text; i<file.size(); i++){
            if(file[i][0] == "END"){flagend = true; continue;}
            result.push_back(file[i]);
        }
        for(int i=idx_data; i<idx_text; i++) result.push_back(file[i]);
        if(flagend) result.push_back({"END"});
    }

    return result;
}

vector<vector<string>> begin_exception(vector<vector<string>> &file){
    vector<vector<string>> result;
    bool flag = false;
    for(int i=0; i<file.size(); i++){
        vector<string> v;
        if(file[i].size() == 2 && file[i][0] == "BEGIN:"){
            string label = file[i][1]; label += ":";
            v.push_back(label);
            v.push_back(file[i][0].substr(0, (int)(file[i][0].size()) - 1));
            result.push_back(v);
        } else if(file[i].size() == 1 && file[i][0] == "BEGIN:"){
            v.push_back(file[i][0].substr(0, (int)(file[i][0].size()) - 1));
            result.push_back(v);
        } else {
            result.push_back(file[i]);
        }
    }
    return result;
}

vector<vector<string>> preprocess_file (string &filename){
    ifstream inputFile(filename);
    vector<vector<string>> new_file; 

    if(!inputFile.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << filename << endl;
        return new_file;
    }

    string line;
    while (getline(inputFile, line)) {
        
        line = remove_comments(line);
        if(line.empty()) continue;
        line = to_upper(line);
        line = trim_spaces(line);
        if(line.empty()) continue;

        vector<string> processed_line = formatted_line(line);

        new_file.push_back(processed_line);
    }

    new_file = begin_exception(new_file);      
    new_file = section_text_data(new_file);


    inputFile.close();

    new_file = solve_macro(new_file);
    return new_file;
}