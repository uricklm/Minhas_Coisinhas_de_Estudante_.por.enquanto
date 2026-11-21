#pragma once
// ============================================================
// entrada.h - funcoes de apoio usadas por TODAS as classes
// (antes cada classe tinha o seu proprio limparBuffer)
//
// REGRA: toda leitura do teclado usa getline. Assim nunca sobra
// "lixo" no buffer e digitar letra num campo numerico nao trava
// o programa (o problema do "cin >> n").
// ============================================================
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cctype>
using namespace std;

// ---------- tela ----------
void limparTela(){
#ifdef _WIN32
    system("cls");
#else
    int r = system("clear");
    (void)r;
#endif
}

// ---------- texto ----------
// tira espacos, TAB e o '\r' que arquivos do Windows deixam no fim da linha
string trim(string s){
    int ini = 0, fim = (int)s.size() - 1;
    while (ini <= fim && isspace((unsigned char)s[ini])) ini++;
    while (fim >= ini && isspace((unsigned char)s[fim])) fim--;
    return s.substr(ini, fim - ini + 1);
}

string maiusculas(string s){
    for (int i = 0; i < (int)s.size(); i++) s[i] = (char)toupper((unsigned char)s[i]);
    return s;
}

string minusculas(string s){
    for (int i = 0; i < (int)s.size(); i++) s[i] = (char)tolower((unsigned char)s[i]);
    return s;
}

// ---------- numeros ----------
// "12", "-15.78", "3,5" -> verdadeiro | "abc", "", "1.2.3" -> falso
bool ehNumero(string s){
    if (s.empty()) return false;
    int i = 0, pontos = 0, digitos = 0;
    if (s[0] == '-') i = 1;
    for (; i < (int)s.size(); i++){
        if (isdigit((unsigned char)s[i])) digitos++;
        else if ((s[i] == '.' || s[i] == ',') && pontos == 0) pontos++;
        else return false;
    }
    return digitos > 0;
}

double paraNumero(string s){
    for (int i = 0; i < (int)s.size(); i++) if (s[i] == ',') s[i] = '.';
    return atof(s.c_str());
}

// numero -> texto sem zeros sobrando: 0.45 -> "0.45", 12 -> "12"
string txt(double v){
    ostringstream o;
    o << setprecision(10) << v;
    return o.str();
}

// numero -> texto com 2 casas: 12 -> "12.00"
string txt2(double v){
    ostringstream o;
    o << fixed << setprecision(2) << v;
    return o.str();
}

// "3/2026" -> "03/2026"
string mesAno(int mes, int ano){
    ostringstream o;
    o << setfill('0') << setw(2) << mes << "/" << ano;
    return o.str();
}

// ---------- entrada pelo teclado ----------
string lerLinha(string pergunta){
    cout << pergunta;
    string s;
    if (!getline(cin, s)){
        cout << "\nEntrada encerrada. Fechando..." << endl;
        exit(0);
    }
    return trim(s);
}

void pausar(){
    cout << endl;
#ifdef _WIN32
    system("pause");
#else
    lerLinha("Pressione ENTER para continuar...");
#endif
}

string lerTextoObrigatorio(string pergunta){
    while (true){
        string s = lerLinha(pergunta);
        if (s != "") return s;
        cout << "Campo obrigatorio." << endl;
    }
}

// pede um inteiro entre minimo e maximo; repete ate o usuario acertar
int lerInt(string pergunta, int minimo, int maximo){
    while (true){
        string s = lerLinha(pergunta);
        if (ehNumero(s)){
            double v = paraNumero(s);
            if (v >= minimo && v <= maximo && v == (int)v) return (int)v;
        }
        cout << "Valor invalido (esperado um inteiro de " << minimo << " a " << maximo << ")." << endl;
    }
}

double lerDouble(string pergunta, double minimo, double maximo){
    while (true){
        string s = lerLinha(pergunta);
        if (ehNumero(s)){
            double v = paraNumero(s);
            if (v >= minimo && v <= maximo) return v;
        }
        cout << "Valor invalido (esperado de " << txt(minimo) << " a " << txt(maximo) << ")." << endl;
    }
}

bool lerSimNao(string pergunta){
    while (true){
        string s = minusculas(lerLinha(pergunta + " (s/n): "));
        if (s == "s") return true;
        if (s == "n") return false;
        cout << "Responda s ou n." << endl;
    }
}

// acrescenta ".txt" se o usuario nao digitou extensao
string nomeComTxt(string nome){
    if (nome.find('.') == string::npos) nome += ".txt";
    return nome;
}
