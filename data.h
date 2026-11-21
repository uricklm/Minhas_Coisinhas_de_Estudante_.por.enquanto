#pragma once
#include "entrada.h"

class Data {
private:
  // VARIAVEIS DOS GETTERS E SETTERS =========
 // identificacao
    string tipo;
    string endereco;
    string cep;
 // caracteristicas fisicas
    float area;
    int comodos;
    int ano;
 // localizacao geografica(é importante para irradiacao solar, microclima)
    string latitude;
    string longitude;

public:
    // O construtor zera os numeros (antes ficavam com "lixo" de memoria)
    Data(){
        area = 0;
        comodos = 0;
        ano = 0;
    }

    // Dados das caracteristicas do imovel
    void dadosImovel();
    void identificacao();
        void set_tipoImovel(string type){tipo = type;}
        string get_tipoImovel(){return tipo;}
        void set_endereco(string adress){endereco = adress;}
        string get_endereco(){return endereco;}
        void set_cep(string num){cep = num;}
        string get_cep(){return cep;}

    void caracFisicas();
        void set_areaTotal(float cube){area = cube;}
        float get_areaTotal(){return area;}
        void set_comodos(int room){comodos = room;}
        int get_comodos(){return comodos;}
        void set_anoConstr(int year){ano = year;}
        int get_anoConstr(){return ano;}

    void locGeografica();
        void set_latitude(string lat){latitude = lat;}
        string get_latitude(){return latitude;}
        void set_longitude(string lon){longitude = lon;}
        string get_longitude(){return longitude;}

    // mostra os dados na tela (usado no relatorio e no mapa)
    void mostrar(){
        cout << "\n[Imovel] " << tipo << " - " << endereco;
        if (cep != "") cout << " (CEP " << cep << ")";
        cout << "\n   Area: " << txt2(area) << " m2 | Comodos: " << comodos << " | Construido em: " << ano << endl;
        if (latitude != "" || longitude != "") cout << "   Coordenadas: " << latitude << ", " << longitude << endl;
    }
};

void Data::dadosImovel(){ // menu principal pra mexer nos dados do imovel
    while(true){
        limparTela();
        cout << "# Preencha todas as seguintes informacoes: " << endl;
        cout << "1. Identificacao" << endl;
        cout << "2. Caracteristicas fisicas" << endl;
        cout << "3. Localizacao geografica" << endl;
        cout << "0. Voltar" << endl;
        // lerInt ja trata letras e numeros fora do intervalo
        int n = lerInt("# --> ", 0, 3);

        switch(n){
            case 1:{
                identificacao();
                break;
            }
            case 2:{
                caracFisicas();
                break;
            }
            case 3:{
                locGeografica();
                break;
            }
            case 0:{
                return;
            }
        }
        pausar();
    }
}

void Data::identificacao(){
    limparTela();
    cout << "=== IDENTIFICACAO DO IMOVEL ===" << endl;
    // guarda o tipo (ex: casa principal, curral...)
    set_tipoImovel(lerTextoObrigatorio("Digite o tipo do imovel (Ex: Casa Principal, Curral, Casa 2): "));

    // rua e numero
    set_endereco(lerTextoObrigatorio("Digite o endereco: "));

    // cep do lugar
    set_cep(lerLinha("Digite o CEP: "));
    cout << "\n[Sucesso] Dados de identificacao salvos!" << endl;
}

void Data::caracFisicas(){
    limparTela();
    cout << "=== CARACTERISTICAS FISICAS ===" << endl;
    // tamanho total do espaco
    set_areaTotal(lerDouble("Digite a area total (em metros quadrados): ", 1, 10000000));

    // quantidade de comodos
    set_comodos(lerInt("Digite a quantidade de comodos/divisoes: ", 1, 1000));

    // ano de construcao pra saber o desgaste
    set_anoConstr(lerInt("Digite o ano de construcao: ", 1500, 2100));
    cout << "\n[Sucesso] Caracteristicas fisicas salvas!" << endl;
}

void Data::locGeografica(){
    limparTela();
    cout << "=== LOCALIZACAO GEOGRAFICA ===" << endl;
    // coordenada pro sol / painel solar
    set_latitude(txt(lerDouble("Digite a Latitude (ex: -15.7938): ", -90, 90)));

    // outra coordenada geografica
    set_longitude(txt(lerDouble("Digite a Longitude (ex: -47.8827): ", -180, 180)));
    cout << "\n[Sucesso] Localizacao geografica salva!" << endl;
}
