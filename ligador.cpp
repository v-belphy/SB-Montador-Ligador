#include <bits/stdc++.h>

using namespace std;

map<string, int> def[5];
map<string, int> uso[5];
vector<int> relativos;
vector<int> codigo[5];
vector<int> codigo_geral;
int tam[5];

void parsear(string line, int idx){
    if(line.size() == 0){return;}
    if(line[0] == 'D'){
        string name = "", num = "";
        bool flag = false;
        for(int i=3; i<(int)(line.size()); i++){
            if(line[i] == ' '){flag = true; continue;}
            if(!flag) name += line[i];
            else num += line[i];
        }
        def[idx][name] = stoi(num);
    } else if(line[0] == 'U'){
        string name = "", num = "";
        bool flag = false;
        for(int i=3; i<(int)(line.size()); i++){
            if(line[i] == ' '){flag = true; continue;}
            if(!flag) name += line[i];
            else num += line[i];
        }
        uso[idx][name] = stoi(num);
    } else if(line[0] == 'R'){
        string num = "";
        for(int i=3; i<(int)(line.size()); i++){
            num += line[i];
            if(line[i] == ' '){
                relativos.push_back(stoi(num));
                num = "";
            }
        }
        if(num.size()) relativos.push_back(stoi(num));
    } else {
        string num = "";
        for(int i=0; i<(int)(line.size()); i++){
            num += line[i];
            if(line[i] == ' '){
                codigo[idx].push_back(stoi(num));
                num = "";
            }
        }
        if(num.size()) codigo[idx].push_back(stoi(num));
        
        for(auto it: codigo[idx]) codigo_geral.push_back(it);
        for(int i=idx+1; i<5; i++){
            tam[i] += codigo[idx].size();;
        }
    }
    return;
}

int main(int argc, char **argv){
    if(argc > 6){cerr << "Ligador permite no maximo 5 arquivos" << endl; return 0;}
    if(argc <= 2){cerr << "Ligador requer no minimo 2 arquivos" << endl; return 0;}

    string outfile(argv[1]);
    string outfilename = outfile.substr(0, (int)(outfile.size())-4);

    for(int i=1; i<argc; i++){
        string filename(argv[i]);
        ifstream inputFile(filename);
        vector<vector<string>> new_file; 

        if(!inputFile.is_open()) {
            cerr << "Erro ao abrir o arquivo: " << filename << endl;
            return 0;
        }

        string line;
        while (getline(inputFile, line)) {
            parsear(line, i-1);
        }
    }

    for(int i=0; i<argc - 1; i++){
        for(auto it: uso[i]){
            int val = -1;
            string name = it.first;
            int num = it.second;
            for(int j=0; j<argc - 1; j++){
                for(auto et: def[j]){
                    if(name == et.first){
                        val = tam[j] + et.second;
                    }
                    if(val != -1){break;}
                }
                if(val != -1){break;}
            }
            if(val == -1){cerr << name << " nao foi declarado em nenhum arquivo" << endl; return 0;}
            codigo_geral[num + tam[i]] = val;
        }
    }

    ofstream arquivoSaida(outfilename + ".e");
    if (!arquivoSaida.is_open()) {
        cerr << "Erro ao abrir o arquivo para escrita!" << endl;
        return 1;
    }

    for(int i=0; i<argc-1; i++){
        for(auto it: def[i]) arquivoSaida << "D, " << it.first << ' ' << it.second + tam[i] << endl;
    }
    for(int i=0; i<argc-1; i++){
        for(auto it: uso[i]) arquivoSaida << "U, " << it.first << ' ' << it.second + tam[i] << endl;
    }
    arquivoSaida << "R,"; for(auto it: relativos) arquivoSaida << ' ' << it; arquivoSaida << endl;
    for(int i=0; i<(int)(codigo_geral.size()); i++){
        if(i == (int)(codigo_geral.size())) arquivoSaida << codigo_geral[i] << endl;
        else arquivoSaida << codigo_geral[i] << ' ';
    }
}