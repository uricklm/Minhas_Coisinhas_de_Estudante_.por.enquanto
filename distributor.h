#pragma once
#include "entrada.h"

// Contrato com a distribuidora (o arquivo MeuTeste.txt tem o bloco
// [DISTRIBUIDORA], entao a classe voltou para nao perder esses dados)
class Distributor {
private:
    string distribuidora;
    string tipo_tarifa;
    double tarifa;      // R$/kWh

public:
    Distributor(){ tarifa = 0; }

    void contratoDistribuidora(); // MENU
        void set_distribuidora(string name){distribuidora = name;}
        string get_distribuidora(){return distribuidora;}
        void set_tipo_tarifa(string t){tipo_tarifa = t;}
        string get_tipo_tarifa(){return tipo_tarifa;}
        void set_tarifa(double t){tarifa = t;}
        double get_tarifa(){return tarifa;}

    void mostrar(){
        cout << "\n[Distribuidora] " << distribuidora << " | tarifa " << tipo_tarifa << " | R$ " << txt2(tarifa) << "/kWh" << endl;
    }
};

void Distributor::contratoDistribuidora(){ // MENU
    while(true){
        limparTela();
        cout << "\n# Preencha todos as seguintes informacoes: " << endl;
        cout << "1. Empresa fornecedora" << endl;
        cout << "2. Tipo de tarifa" << endl;
        cout << "3. Valor da tarifa (R$/kWh)" << endl;
        cout << "0. Voltar" << endl;
        int n = lerInt("# --> ", 0, 3);

        switch(n){
            case 1:{
                set_distribuidora(lerTextoObrigatorio("Empresa fornecedora: "));
                break;
            }
            case 2:{
                set_tipo_tarifa(lerTextoObrigatorio("Tipo de tarifa (convencional, branca...): "));
                break;
            }
            case 3:{
                set_tarifa(lerDouble("Valor da tarifa (R$/kWh): ", 0.001, 100));
                break;
            }
            case 0:{
                return;
            }
        }
    }
}
