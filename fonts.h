#pragma once
#include <vector>
#include "entrada.h"

// Antes: UMA geracao, UMA bateria, UM "outros".
// Agora cada uma e uma LISTA (o arquivo pode ter varias).
struct Geracao {
    string tipo;
    int qtd = 1;
    double kwp = 0;     // potencia de cada unidade
};

struct Bateria {
    string id;
    double capKwh = 0;
    int nivel = 0;      // carga atual em %
};

struct OutraFonte {
    string tipo;
    int qtd = 1;
    double kw = 0;
};

// Premissas para estimar a geracao diaria (horas equivalentes na potencia nominal)
const double HORAS_SOL = 4.5;
const double HORAS_OUTRAS = 6.0;

class Fonts {
private:
    // fontes de energia
    vector<Geracao> geracoes;
    vector<Bateria> baterias;
    vector<OutraFonte> outras;

public:
    // Dados das fontes de energia
    void fontesEnergia();   // MENU
    void menuGeracao();
    void menuBaterias();
    void menuOutros();

    // ---- geracao local ----
    int qtdGeracoes(){ return (int)geracoes.size(); }
    Geracao getGeracao(int pos){ return geracoes[pos]; }
    bool adicionarGeracao(Geracao g){
        if (g.tipo == "" || g.qtd <= 0 || g.kwp <= 0) return false;
        geracoes.push_back(g);
        return true;
    }

    // ---- baterias ----
    int qtdBaterias(){ return (int)baterias.size(); }
    Bateria getBateria(int pos){ return baterias[pos]; }
    bool existeBateria(string id){
        for (int i = 0; i < (int)baterias.size(); i++)
            if (minusculas(baterias[i].id) == minusculas(id)) return true;
        return false;
    }
    bool adicionarBateria(Bateria b){
        if (b.id == "" || b.capKwh <= 0 || b.nivel < 0 || b.nivel > 100) return false;
        if (existeBateria(b.id)) return false;      // o ID nao pode repetir
        baterias.push_back(b);
        return true;
    }

    // ---- outras fontes ----
    int qtdOutras(){ return (int)outras.size(); }
    OutraFonte getOutra(int pos){ return outras[pos]; }
    bool adicionarOutra(OutraFonte o){
        if (o.tipo == "" || o.qtd <= 0 || o.kw <= 0) return false;
        outras.push_back(o);
        return true;
    }

    // ---- calculos ----
    double geracaoDiariaKwh(){
        double total = 0;
        for (int i = 0; i < (int)geracoes.size(); i++){
            bool solar = minusculas(geracoes[i].tipo).find("solar") != string::npos;
            total += geracoes[i].qtd * geracoes[i].kwp * (solar ? HORAS_SOL : HORAS_OUTRAS);
        }
        return total;
    }
    double capacidadeBateriasKwh(){
        double total = 0;
        for (int i = 0; i < (int)baterias.size(); i++) total += baterias[i].capKwh;
        return total;
    }
    double energiaArmazenadaKwh(){
        double total = 0;
        for (int i = 0; i < (int)baterias.size(); i++) total += baterias[i].capKwh * baterias[i].nivel / 100.0;
        return total;
    }

    void mostrar(){
        cout << "\n[Fontes proprias]";
        if (geracoes.empty() && baterias.empty() && outras.empty()){
            cout << " nenhuma cadastrada" << endl;
            return;
        }
        cout << endl;
        for (int i = 0; i < (int)geracoes.size(); i++)
            cout << "   Geracao: " << geracoes[i].tipo << " - " << geracoes[i].qtd << " x " << txt(geracoes[i].kwp) << " kWp" << endl;
        if (!geracoes.empty()) cout << "   Geracao diaria estimada: " << txt2(geracaoDiariaKwh()) << " kWh" << endl;
        for (int i = 0; i < (int)baterias.size(); i++)
            cout << "   Bateria " << baterias[i].id << ": " << txt(baterias[i].capKwh) << " kWh, carga " << baterias[i].nivel << "%" << endl;
        for (int i = 0; i < (int)outras.size(); i++)
            cout << "   Outra: " << outras[i].tipo << " - " << outras[i].qtd << " x " << txt(outras[i].kw) << " kW" << endl;
    }
};

void Fonts::fontesEnergia(){ // MENU
    limparTela();

    // so pergunta se ainda nao tem nenhuma fonte cadastrada
    if (geracoes.empty() && baterias.empty() && outras.empty()){
        if (!lerSimNao("# Voce possui fontes de energia proprias?")){
            cout << "\nVoltando a tela de cadastro..." << endl;
            pausar();
            return;
        }
    }

    while(true){
        limparTela();
        cout << "\n# Preencha todas as seguintes informacoes: " << endl;
        cout << "1. Geracao local" << endl;
        cout << "2. Baterias/Armazenamento" << endl;
        cout << "3. Outros(geradores, turbinas, etc)" << endl;
        cout << "0. Voltar" << endl;
        int n = lerInt("# --> ", 0, 3);

        switch(n){
            case 1:{
                menuGeracao();
                break;
            }
            case 2:{
                menuBaterias();
                break;
            }
            case 3:{
                menuOutros();
                break;
            }
            case 0:{
                return;
            }
        }
    }
}

void Fonts::menuGeracao(){
    while(true){
        limparTela();
        cout << "===== GERACAO LOCAL =====" << endl;
        if (geracoes.empty()) cout << "(vazio)" << endl;
        for (int i = 0; i < (int)geracoes.size(); i++)
            cout << setw(3) << i + 1 << ". " << geracoes[i].tipo << " | " << geracoes[i].qtd << " x " << txt(geracoes[i].kwp) << " kWp" << endl;
        cout << "\n1. Adicionar\n2. Remover\n0. Voltar" << endl;
        int op = lerInt("# --> ", 0, 2);

        if (op == 0) return;
        if (op == 1){
            Geracao g;
            g.tipo = lerTextoObrigatorio("Tipo (ex: solar fotovoltaica, eolica): ");
            g.qtd = lerInt("Quantidade de unidades/paineis: ", 1, 100000);
            g.kwp = lerDouble("Potencia por unidade (kWp): ", 0.001, 100000);
            adicionarGeracao(g);
        } else if (geracoes.empty()){
            cout << "Nada para remover." << endl;
            pausar();
        } else {
            int n = lerInt("Numero do item a remover (0 cancela): ", 0, (int)geracoes.size());
            if (n > 0) geracoes.erase(geracoes.begin() + (n - 1));
        }
    }
}

void Fonts::menuBaterias(){
    while(true){
        limparTela();
        cout << "===== BATERIAS / ARMAZENAMENTO =====" << endl;
        if (baterias.empty()) cout << "(vazio)" << endl;
        for (int i = 0; i < (int)baterias.size(); i++)
            cout << setw(3) << i + 1 << ". " << baterias[i].id << " | " << txt(baterias[i].capKwh) << " kWh | carga " << baterias[i].nivel << "%" << endl;
        cout << "\n1. Adicionar\n2. Remover\n3. Atualizar nivel de carga\n0. Voltar" << endl;
        int op = lerInt("# --> ", 0, 3);

        if (op == 0) return;
        if (op == 1){
            Bateria b;
            while(true){
                b.id = lerTextoObrigatorio("ID da bateria (ex: BAT-01): ");
                if (!existeBateria(b.id)) break;
                cout << "Esse ID ja existe." << endl;
            }
            b.capKwh = lerDouble("Capacidade (kWh): ", 0.1, 1000000);
            b.nivel = lerInt("Nivel de carga atual (%): ", 0, 100);
            adicionarBateria(b);
        } else if (baterias.empty()){
            cout << "Nenhuma bateria cadastrada." << endl;
            pausar();
        } else if (op == 2){
            int n = lerInt("Numero do item a remover (0 cancela): ", 0, (int)baterias.size());
            if (n > 0) baterias.erase(baterias.begin() + (n - 1));
        } else {
            int n = lerInt("Numero da bateria (0 cancela): ", 0, (int)baterias.size());
            if (n > 0) baterias[n - 1].nivel = lerInt("Novo nivel de carga (%): ", 0, 100);
        }
    }
}

void Fonts::menuOutros(){
    while(true){
        limparTela();
        cout << "===== OUTRAS FONTES (geradores, turbinas...) =====" << endl;
        if (outras.empty()) cout << "(vazio)" << endl;
        for (int i = 0; i < (int)outras.size(); i++)
            cout << setw(3) << i + 1 << ". " << outras[i].tipo << " | " << outras[i].qtd << " x " << txt(outras[i].kw) << " kW" << endl;
        cout << "\n1. Adicionar\n2. Remover\n0. Voltar" << endl;
        int op = lerInt("# --> ", 0, 2);

        if (op == 0) return;
        if (op == 1){
            OutraFonte o;
            o.tipo = lerTextoObrigatorio("Tipo (ex: gerador a diesel): ");
            o.qtd = lerInt("Quantidade: ", 1, 100000);
            o.kw = lerDouble("Potencia por unidade (kW): ", 0.001, 1000000);
            adicionarOutra(o);
        } else if (outras.empty()){
            cout << "Nada para remover." << endl;
            pausar();
        } else {
            int n = lerInt("Numero do item a remover (0 cancela): ", 0, (int)outras.size());
            if (n > 0) outras.erase(outras.begin() + (n - 1));
        }
    }
}
