#pragma once
#include "data.h"
#include "infra.h"
#include "fonts.h"
#include "equipment.h"
#include "distributor.h"
#include "persistencia.h"

/*
 arquivo "geral" de cadastro: agrega os headers
 de cada parte do cadastro alem do main menu do cadastro
*/

#include <iostream>
using namespace std;

class Cadastro {
public:
    // salva o cadastro no arquivo (pergunta o nome se ainda nao tiver um)
    bool salvarCadastro(Data& d, Infra& i, Fonts& f, Equipment& e, Distributor& dist,
                        string& arquivoAtual, bool& carregado){
        if (arquivoAtual == "")
            arquivoAtual = nomeComTxt(lerTextoObrigatorio("Nome do arquivo para salvar (ex: minha_casa.txt): "));

        if (Persistencia::salvarDados(arquivoAtual, d, i, f, e, dist)){
            cout << "\n[Sucesso] Informacoes salvas em '" << arquivoAtual << "'." << endl;
            carregado = true;      // agora existe um arquivo valido: libera o Menu Principal
            return true;
        }
        cout << "\n[Erro] Nao foi possivel salvar em '" << arquivoAtual << "'." << endl;
        return false;
    }

    void MenuCadastro(Data& d, Infra& i, Fonts& f, Equipment& e, Distributor& dist,
                      string& arquivoAtual, bool& carregado){
        while(true){
            limparTela();
            cout << "\n============ CADASTRO ============" << endl;
            cout << "Arquivo: " << (arquivoAtual == "" ? "(ainda nao salvo)" : arquivoAtual) << endl;
            cout << "# Cadastre as seguintes informacoes" << endl;
            cout << "  para conseguir utilizar o gestor!" << endl;
            cout << "1. Dados do imovel" << endl;
            cout << "2. Infrastrutura energetica" << endl;
            cout << "3. Fontes de energia (se possuir)" << endl;
            cout << "4. Equipamentos conectados" << endl;
            cout << "5. Contrato com a distribuidora" << endl;
            cout << "6. Salvar informacoes cadastradas" << endl;
            cout << "0. Voltar" << endl;
            cout << "==================================" << endl;
            int n = lerInt("# ---> ", 0, 6);

            switch(n){
                case 1:
                    d.dadosImovel();
                    break;
                case 2:
                    i.infraestrutura();
                    break;
                case 3:
                    f.fontesEnergia();
                    break;
                case 4:
                    e.equipamentos();
                    break;
                case 5:
                    dist.contratoDistribuidora();
                    break;
                case 6:
                    salvarCadastro(d, i, f, e, dist, arquivoAtual, carregado);
                    pausar();
                    break;
                case 0:
                    if (lerSimNao("Deseja salvar as informacoes antes de voltar?"))
                        salvarCadastro(d, i, f, e, dist, arquivoAtual, carregado);
                    return;
            }
        }
    }
};
