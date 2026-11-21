#pragma once
#include <vector>
#include "entrada.h"

// Um mes do historico de consumo (os valores ja comecam zerados)
struct Consumo {
    int ano = 0;
    int mes = 0;
    double kwh = 0;
    double valor = 0; // R$ pagos
};

class Infra {
private:
    // infraestrutura energetica (painel eletrico)
    double capacidade; // kW (agora aceita decimais, ex: 12.5)
    int voltagem;
    // historico de gastos: uma lista de meses, SEMPRE em ordem cronologica
    vector<Consumo> historico;

public:
    Infra(){
        capacidade = 0;
        voltagem = 0;
    }

    // Dados da infraestrutura energetica
    void infraestrutura(); // MENU: painel eletrico e historico
    void painel();
    void menuHistorico();
    void listarHistorico();
        void set_capacidade(double kw){capacidade = kw;}
        double get_capacidade(){return capacidade;}
        void set_voltagem(int v){voltagem = v;}
        int get_voltagem(){return voltagem;}

    // ---- historico de consumo ----
    int qtdConsumos(){ return (int)historico.size(); }
    Consumo getConsumo(int pos){ return historico[pos]; }

    // valida o mes, coloca na posicao cronologica certa e,
    // se o mes/ano ja existe, apenas substitui
    bool adicionarConsumo(Consumo c){
        if (c.ano < 1990 || c.ano > 2100 || c.mes < 1 || c.mes > 12) return false;
        if (c.kwh < 0 || c.valor < 0) return false;

        for (int i = 0; i < (int)historico.size(); i++){
            if (historico[i].ano == c.ano && historico[i].mes == c.mes){
                historico[i] = c;
                return true;
            }
        }
        historico.push_back(c);
        // insercao ordenada: o novo mes "sobe" ate a posicao certa
        int j = (int)historico.size() - 1;
        while (j > 0 && historico[j - 1].ano * 100 + historico[j - 1].mes > historico[j].ano * 100 + historico[j].mes){
            Consumo tmp = historico[j];
            historico[j] = historico[j - 1];
            historico[j - 1] = tmp;
            j--;
        }
        return true;
    }

    void removerConsumo(int pos){ historico.erase(historico.begin() + pos); }

    double mediaKwh(){
        if (historico.empty()) return 0;
        double soma = 0;
        for (int i = 0; i < (int)historico.size(); i++) soma += historico[i].kwh;
        return soma / historico.size();
    }

    void mostrar(){
        cout << "\n[Infraestrutura] Painel: " << txt2(capacidade) << " kW, " << voltagem << " V" << endl;
        if (historico.empty()) cout << "   Historico de consumo: vazio" << endl;
        else cout << "   Historico: " << historico.size() << " meses (" << mesAno(historico[0].mes, historico[0].ano)
                  << " a " << mesAno(historico.back().mes, historico.back().ano) << ") | media "
                  << txt2(mediaKwh()) << " kWh/mes" << endl;
    }
};

void Infra::infraestrutura(){ // MENU
    while(true){
        limparTela();
        cout << "# Preencha todas as seguintes informacoes: " << endl;
        cout << "1. Painel eletrico" << endl;
        cout << "2. Historico de consumo" << endl;
        cout << "0. Voltar" << endl;
        int n = lerInt("# --> ", 0, 2);

        switch(n){
            case 1:{
                painel();
                pausar();
                break;
            }
            case 2:{
                menuHistorico();
                break;
            }
            case 0:{
                return;
            }
        }
    }
}

void Infra::painel(){
    limparTela();
    cout << "=== PAINEL ELETRICO ===" << endl;
    set_capacidade(lerDouble("Capacidade do painel (kW): ", 0.1, 100000));
    set_voltagem(lerInt("Voltagem (V) - ex: 127, 220, 380: ", 1, 100000));
    cout << "\n[Sucesso] Painel eletrico salvo!" << endl;
}

void Infra::listarHistorico(){
    if (historico.empty()) cout << "(vazio)" << endl;
    for (int i = 0; i < (int)historico.size(); i++){
        cout << setw(3) << i + 1 << ". " << mesAno(historico[i].mes, historico[i].ano) << "  "
             << setw(9) << txt2(historico[i].kwh) << " kWh   R$ " << txt2(historico[i].valor) << endl;
    }
}

void Infra::menuHistorico(){
    while(true){
        limparTela();
        cout << "===== HISTORICO DE CONSUMO =====" << endl;
        listarHistorico();
        cout << "\n1. Adicionar mes" << endl;
        cout << "2. Remover mes" << endl;
        cout << "0. Voltar" << endl;
        int op = lerInt("# --> ", 0, 2);

        if (op == 0) return;
        if (op == 1){
            Consumo c;
            c.ano = lerInt("Ano: ", 1990, 2100);
            c.mes = lerInt("Mes (1-12): ", 1, 12);
            c.kwh = lerDouble("Consumo (kWh): ", 0, 10000000);
            c.valor = lerDouble("Valor pago (R$): ", 0, 100000000);
            adicionarConsumo(c);
        } else if (historico.empty()){
            cout << "Nada para remover." << endl;
            pausar();
        } else {
            int n = lerInt("Numero do item a remover (0 cancela): ", 0, (int)historico.size());
            if (n > 0) removerConsumo(n - 1);
        }
    }
}
