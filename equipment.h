#pragma once
#include <vector>
#include "entrada.h"

// Antes: UM eletrodomestico, UM HVAC, UMA lampada.
// Agora cada categoria e uma LISTA. horasDia = horas de uso por dia.
struct Eletro {
    string nome;
    int qtd = 1;
    double potenciaW = 0;
    double horasDia = 0;
    string modo;
};

struct Hvac {
    string tipo;
    int qtd = 1;
    double potenciaW = 0;
    double horasDia = 0;
    string modo;
};

struct Lampada {
    string ambiente;
    string tipo;
    int qtd = 1;
    double potenciaW = 0;
    double horasDia = 0;
    string controle;
};

struct Carregador {
    string nome;
    double potenciaKw = 0;
    double horasDia = 0;
};

// nem tudo liga ao mesmo tempo: pico = potencia instalada x fator
const double FATOR_SIMULTANEIDADE = 0.6;
// periodos usados nas projecoes (mes = ano / 12, assim 12 meses = 1 ano)
const double DIAS_ANO = 365.0;
const double DIAS_MES = DIAS_ANO / 12.0;

class Equipment {
private:
    // equipamentos
    vector<Eletro> eletros;
    vector<Hvac> hvacs;         // (ar condicionado, aquecedor, ventilacao)
    vector<Lampada> lampadas;
    vector<Carregador> evs;     // carregadores ev, se possuir

public:
    // Dados dos equipamentos do imovel
    void equipamentos();        // MENU
    void menuEletros();
    void menuEV();
    void menuHvac();
    void menuIluminacao();

    // ---- eletrodomesticos ----
    int qtdEletros(){ return (int)eletros.size(); }
    Eletro getEletro(int pos){ return eletros[pos]; }
    bool adicionarEletro(Eletro e){
        if (e.nome == "" || e.qtd <= 0 || e.potenciaW <= 0 || e.horasDia < 0 || e.horasDia > 24) return false;
        eletros.push_back(e);
        return true;
    }

    // ---- HVAC ----
    int qtdHvac(){ return (int)hvacs.size(); }
    Hvac getHvac(int pos){ return hvacs[pos]; }
    bool adicionarHvac(Hvac h){
        if (h.tipo == "" || h.qtd <= 0 || h.potenciaW <= 0 || h.horasDia < 0 || h.horasDia > 24) return false;
        hvacs.push_back(h);
        return true;
    }

    // ---- iluminacao ----
    int qtdLampadas(){ return (int)lampadas.size(); }
    Lampada getLampada(int pos){ return lampadas[pos]; }
    bool adicionarLampada(Lampada l){
        if (l.ambiente == "" || l.qtd <= 0 || l.potenciaW <= 0 || l.horasDia < 0 || l.horasDia > 24) return false;
        lampadas.push_back(l);
        return true;
    }

    // ---- carregadores EV ----
    int qtdEV(){ return (int)evs.size(); }
    Carregador getEV(int pos){ return evs[pos]; }
    bool adicionarEV(Carregador c){
        if (c.nome == "" || c.potenciaKw <= 0 || c.horasDia < 0 || c.horasDia > 24) return false;
        evs.push_back(c);
        return true;
    }

    int totalItens(){ return (int)(eletros.size() + hvacs.size() + lampadas.size() + evs.size()); }

    // ---- calculos: consumo diario (kWh/dia) ----
    double consumoEletros(){
        double t = 0;
        for (int i = 0; i < (int)eletros.size(); i++) t += eletros[i].qtd * eletros[i].potenciaW * eletros[i].horasDia / 1000.0;
        return t;
    }
    double consumoHvac(){
        double t = 0;
        for (int i = 0; i < (int)hvacs.size(); i++) t += hvacs[i].qtd * hvacs[i].potenciaW * hvacs[i].horasDia / 1000.0;
        return t;
    }
    double consumoIluminacao(){
        double t = 0;
        for (int i = 0; i < (int)lampadas.size(); i++) t += lampadas[i].qtd * lampadas[i].potenciaW * lampadas[i].horasDia / 1000.0;
        return t;
    }
    double consumoEV(){
        double t = 0;
        for (int i = 0; i < (int)evs.size(); i++) t += evs[i].potenciaKw * evs[i].horasDia;
        return t;
    }
    double consumoDiarioKwh(){ return consumoEletros() + consumoHvac() + consumoIluminacao() + consumoEV(); }

    // ---- calculos: potencia instalada (kW) ----
    double potenciaInstaladaKw(){
        double t = 0;
        for (int i = 0; i < (int)eletros.size(); i++) t += eletros[i].qtd * eletros[i].potenciaW / 1000.0;
        for (int i = 0; i < (int)hvacs.size(); i++) t += hvacs[i].qtd * hvacs[i].potenciaW / 1000.0;
        for (int i = 0; i < (int)lampadas.size(); i++) t += lampadas[i].qtd * lampadas[i].potenciaW / 1000.0;
        for (int i = 0; i < (int)evs.size(); i++) t += evs[i].potenciaKw;
        return t;
    }

    void mostrar(){
        cout << "\n[Equipamentos] " << totalItens() << " cadastros" << endl;
        cout << "   Eletrodomesticos: " << eletros.size() << " (" << txt2(consumoEletros()) << " kWh/dia)" << endl;
        cout << "   HVAC:             " << hvacs.size() << " (" << txt2(consumoHvac()) << " kWh/dia)" << endl;
        cout << "   Iluminacao:       " << lampadas.size() << " (" << txt2(consumoIluminacao()) << " kWh/dia)" << endl;
        cout << "   Carregadores EV:  " << evs.size() << " (" << txt2(consumoEV()) << " kWh/dia)" << endl;
        cout << "   Potencia instalada: " << txt2(potenciaInstaladaKw()) << " kW" << endl;
    }
};

void Equipment::equipamentos(){ // MENU
    while(true){
        limparTela();
        cout << "\n# Preencha todos as seguintes informacoes: " << endl;
        cout << "1. Eletrodomesticos inteligentes" << endl;
        cout << "2. Carregadores EV" << endl; // se nao possuir, nao coloca no arquivo
        cout << "3. Sistemas HVAC(ar condicionado, aquecimento, ventilacao)" << endl;
        cout << "4. Iluminacao" << endl;
        cout << "0. Voltar" << endl;
        int n = lerInt("# --> ", 0, 4);

        switch(n){
            case 1:{
                menuEletros();
                break;
            }
            case 2:{
                menuEV();
                break;
            }
            case 3:{
                menuHvac();
                break;
            }
            case 4:{
                menuIluminacao();
                break;
            }
            case 0:{
                return;
            }
        }
    }
}

void Equipment::menuEletros(){
    while(true){
        limparTela();
        cout << "===== ELETRODOMESTICOS =====" << endl;
        if (eletros.empty()) cout << "(vazio)" << endl;
        for (int i = 0; i < (int)eletros.size(); i++)
            cout << setw(3) << i + 1 << ". " << eletros[i].nome << " | " << eletros[i].qtd << "x " << txt(eletros[i].potenciaW)
                 << " W | " << txt(eletros[i].horasDia) << " h/dia | " << eletros[i].modo << endl;
        cout << "\n1. Adicionar\n2. Remover\n0. Voltar" << endl;
        int op = lerInt("# --> ", 0, 2);

        if (op == 0) return;
        if (op == 1){
            Eletro e;
            e.nome = lerTextoObrigatorio("Tipo/Nome do eletrodomestico: ");
            e.qtd = lerInt("Quantidade: ", 1, 1000);
            e.potenciaW = lerDouble("Potencia (W): ", 1, 1000000);
            e.horasDia = lerDouble("Uso tipico (horas por dia): ", 0, 24);
            e.modo = lerTextoObrigatorio("Modo de operacao (inteligente, convencional...): ");
            adicionarEletro(e);
        } else if (eletros.empty()){
            cout << "Nada para remover." << endl;
            pausar();
        } else {
            int n = lerInt("Numero do item a remover (0 cancela): ", 0, (int)eletros.size());
            if (n > 0) eletros.erase(eletros.begin() + (n - 1));
        }
    }
}

void Equipment::menuEV(){
    while(true){
        limparTela();
        cout << "===== CARREGADORES EV =====" << endl;
        if (evs.empty()) cout << "(vazio)" << endl;
        for (int i = 0; i < (int)evs.size(); i++)
            cout << setw(3) << i + 1 << ". " << evs[i].nome << " | " << txt(evs[i].potenciaKw) << " kW | " << txt(evs[i].horasDia) << " h/dia" << endl;
        cout << "\n1. Adicionar\n2. Remover\n0. Voltar" << endl;
        int op = lerInt("# --> ", 0, 2);

        if (op == 0) return;
        if (op == 1){
            Carregador c;
            c.nome = lerTextoObrigatorio("Nome (ex: Wallbox garagem): ");
            c.potenciaKw = lerDouble("Potencia do carregador EV (kW): ", 0.1, 1000);
            c.horasDia = lerDouble("Uso tipico (horas por dia): ", 0, 24);
            adicionarEV(c);
        } else if (evs.empty()){
            cout << "Nada para remover." << endl;
            pausar();
        } else {
            int n = lerInt("Numero do item a remover (0 cancela): ", 0, (int)evs.size());
            if (n > 0) evs.erase(evs.begin() + (n - 1));
        }
    }
}

void Equipment::menuHvac(){
    while(true){
        limparTela();
        cout << "===== SISTEMAS HVAC =====" << endl;
        if (hvacs.empty()) cout << "(vazio)" << endl;
        for (int i = 0; i < (int)hvacs.size(); i++)
            cout << setw(3) << i + 1 << ". " << hvacs[i].tipo << " | " << hvacs[i].qtd << "x " << txt(hvacs[i].potenciaW)
                 << " W | " << txt(hvacs[i].horasDia) << " h/dia | " << hvacs[i].modo << endl;
        cout << "\n1. Adicionar\n2. Remover\n0. Voltar" << endl;
        int op = lerInt("# --> ", 0, 2);

        if (op == 0) return;
        if (op == 1){
            Hvac h;
            h.tipo = lerTextoObrigatorio("Tipo de sistema (ex: Ar Condicionado Inverter): ");
            h.qtd = lerInt("Quantidade de unidades: ", 1, 1000);
            h.potenciaW = lerDouble("Potencia por unidade (W): ", 1, 1000000);
            h.horasDia = lerDouble("Uso tipico (horas por dia): ", 0, 24);
            h.modo = lerTextoObrigatorio("Modo de operacao: ");
            adicionarHvac(h);
        } else if (hvacs.empty()){
            cout << "Nada para remover." << endl;
            pausar();
        } else {
            int n = lerInt("Numero do item a remover (0 cancela): ", 0, (int)hvacs.size());
            if (n > 0) hvacs.erase(hvacs.begin() + (n - 1));
        }
    }
}

void Equipment::menuIluminacao(){
    while(true){
        limparTela();
        cout << "===== ILUMINACAO =====" << endl;
        if (lampadas.empty()) cout << "(vazio)" << endl;
        for (int i = 0; i < (int)lampadas.size(); i++)
            cout << setw(3) << i + 1 << ". " << lampadas[i].ambiente << " | " << lampadas[i].tipo << " | " << lampadas[i].qtd << "x "
                 << txt(lampadas[i].potenciaW) << " W | " << txt(lampadas[i].horasDia) << " h/dia | " << lampadas[i].controle << endl;
        cout << "\n1. Adicionar\n2. Remover\n0. Voltar" << endl;
        int op = lerInt("# --> ", 0, 2);

        if (op == 0) return;
        if (op == 1){
            Lampada l;
            l.ambiente = lerTextoObrigatorio("Ambiente (ex: Sala): ");
            l.tipo = lerTextoObrigatorio("Tipo de lampada (ex: LED): ");
            l.qtd = lerInt("Quantidade no ambiente: ", 1, 10000);
            l.potenciaW = lerDouble("Potencia por lampada (W): ", 0.1, 100000);
            l.horasDia = lerDouble("Uso tipico (horas por dia): ", 0, 24);
            l.controle = lerTextoObrigatorio("Modo de controle (ex: Sensor de presenca, manual): ");
            adicionarLampada(l);
        } else if (lampadas.empty()){
            cout << "Nada para remover." << endl;
            pausar();
        } else {
            int n = lerInt("Numero do item a remover (0 cancela): ", 0, (int)lampadas.size());
            if (n > 0) lampadas.erase(lampadas.begin() + (n - 1));
        }
    }
}
