#pragma once
// ==============================================================
// MODULO 5 - MAPEAMENTO DA REDE ELETRICA: ARVORE N-ARIA
//
// Uma arvore N-ARIA e uma arvore em que cada no pode ter QUALQUER
// quantidade de filhos. Aqui a arvore fica guardada em um VETOR:
//   - cada no e identificado pela sua POSICAO (indice) no vetor;
//   - cada no guarda a lista com os INDICES dos seus filhos.
//
//   Imovel
//    +-- Infraestrutura -> grupos de cargas -> aparelhos
//    +-- Geracao        -> geradores
//    +-- Baterias       -> cada bateria
// ==============================================================
#include <vector>
#include "data.h"
#include "infra.h"
#include "fonts.h"
#include "equipment.h"
#include "distributor.h"

struct NoRede {
    string nome;
    vector<int> filhos;    // indices dos filhos dentro do vetor da arvore
};

// cria um no, liga ao pai (se tiver) e devolve o indice do novo no
int novoNo(vector<NoRede>& arvore, string nome, int pai){
    NoRede n;
    n.nome = nome;
    arvore.push_back(n);
    int indice = (int)arvore.size() - 1;           // o novo no e o ultimo do vetor
    if (pai >= 0) arvore[pai].filhos.push_back(indice);   // registra como filho do pai
    return indice;
}

// RECURSAO: imprime o no e depois chama a si mesma para cada filho.
// "prefixo" guarda os tracos '|' das linhas de cima; "ultimo" diz se
// este no e o ultimo filho do pai (nesse caso a barra nao continua).
void imprimirArvore(vector<NoRede>& arvore, int no, string prefixo, bool ultimo, bool raiz){
    if (raiz) cout << arvore[no].nome << endl;
    else cout << prefixo << "+-- " << arvore[no].nome << endl;

    string novoPrefixo = prefixo;
    if (!raiz) novoPrefixo += ultimo ? "    " : "|   ";

    for (int i = 0; i < (int)arvore[no].filhos.size(); i++){
        bool eUltimo = (i == (int)arvore[no].filhos.size() - 1);
        imprimirArvore(arvore, arvore[no].filhos[i], novoPrefixo, eUltimo, false);   // chamada recursiva
    }
}

void executarMapeamento(Data& d, Infra& infra, Fonts& fontes, Equipment& equip, Distributor& dist){
    limparTela();
    vector<NoRede> arvore;

    // ---- raiz ----
    int imovel = novoNo(arvore, "[IMOVEL] " + d.get_tipoImovel() + " - " + d.get_endereco(), -1);

    // ---- ramo 1: infraestrutura -> grupos de cargas -> aparelhos ----
    int infraNo = novoNo(arvore, "[INFRAESTRUTURA] Painel " + txt(infra.get_capacidade()) + " kW / "
                                 + to_string(infra.get_voltagem()) + " V (rede: " + dist.get_distribuidora() + ")", imovel);

    if (equip.qtdEletros() > 0){
        int g = novoNo(arvore, "[CARGAS] Eletrodomesticos (" + to_string(equip.qtdEletros()) + " itens, "
                               + txt2(equip.consumoEletros()) + " kWh/dia)", infraNo);
        for (int i = 0; i < equip.qtdEletros(); i++){
            Eletro x = equip.getEletro(i);
            novoNo(arvore, x.nome + " (" + to_string(x.qtd) + "x " + txt(x.potenciaW) + " W)", g);
        }
    }
    if (equip.qtdHvac() > 0){
        int g = novoNo(arvore, "[CARGAS] HVAC (" + to_string(equip.qtdHvac()) + " itens, "
                               + txt2(equip.consumoHvac()) + " kWh/dia)", infraNo);
        for (int i = 0; i < equip.qtdHvac(); i++){
            Hvac x = equip.getHvac(i);
            novoNo(arvore, x.tipo + " (" + to_string(x.qtd) + "x " + txt(x.potenciaW) + " W)", g);
        }
    }
    if (equip.qtdLampadas() > 0){
        int g = novoNo(arvore, "[CARGAS] Iluminacao (" + to_string(equip.qtdLampadas()) + " ambientes, "
                               + txt2(equip.consumoIluminacao()) + " kWh/dia)", infraNo);
        for (int i = 0; i < equip.qtdLampadas(); i++){
            Lampada x = equip.getLampada(i);
            novoNo(arvore, x.ambiente + " (" + to_string(x.qtd) + "x " + txt(x.potenciaW) + " W, " + x.controle + ")", g);
        }
    }
    if (equip.qtdEV() > 0){
        int g = novoNo(arvore, "[CARGAS] Carregadores EV (" + to_string(equip.qtdEV()) + " itens, "
                               + txt2(equip.consumoEV()) + " kWh/dia)", infraNo);
        for (int i = 0; i < equip.qtdEV(); i++){
            Carregador x = equip.getEV(i);
            novoNo(arvore, x.nome + " (" + txt(x.potenciaKw) + " kW)", g);
        }
    }

    // ---- ramo 2: geracao ----
    int geracaoNo = novoNo(arvore, "[GERACAO] " + txt2(fontes.geracaoDiariaKwh()) + " kWh/dia estimados", imovel);
    for (int i = 0; i < fontes.qtdGeracoes(); i++){
        Geracao x = fontes.getGeracao(i);
        novoNo(arvore, x.tipo + " (" + to_string(x.qtd) + " x " + txt(x.kwp) + " kWp)", geracaoNo);
    }
    for (int i = 0; i < fontes.qtdOutras(); i++){
        OutraFonte x = fontes.getOutra(i);
        novoNo(arvore, "Backup: " + x.tipo + " (" + to_string(x.qtd) + " x " + txt(x.kw) + " kW)", geracaoNo);
    }

    // ---- ramo 3: baterias ----
    int bateriasNo = novoNo(arvore, "[BATERIAS] " + txt2(fontes.energiaArmazenadaKwh()) + " de "
                                    + txt2(fontes.capacidadeBateriasKwh()) + " kWh armazenados", imovel);
    for (int i = 0; i < fontes.qtdBaterias(); i++){
        Bateria x = fontes.getBateria(i);
        novoNo(arvore, x.id + " (" + txt(x.capKwh) + " kWh, carga " + to_string(x.nivel) + "%)", bateriasNo);
    }

    cout << "=== MODULO 5: TOPOLOGIA DOS EQUIPAMENTOS CADASTRADOS (Arvore N-aria) ===" << endl;
    cout << "Equipamentos e cargas extraidas do arquivo:\n" << endl;
    imprimirArvore(arvore, 0, "", true, true);     // comeca pela raiz (indice 0)
    cout << "\nTotal de nos no mapa: " << arvore.size() << endl;
    pausar();
}
