#pragma once
// ==============================================================
// MODULO 2 - HISTORICO DE GASTOS
//   * ORDENACAO: Insertion Sort (por consumo em kWh)
//   * BUSCA:     Busca Binaria (mes/ano) e busca linear (filtro por ano)
// ==============================================================
#include "infra.h"

// ---------------------------------------------------------------
// ORDENACAO - INSERTION SORT (ordenacao por insercao)
// Por que escolhi este?
//  - o historico tem poucos itens (12, 24, 60 meses): nao precisa de algoritmo sofisticado;
//  - e ESTAVEL: se dois meses tem o mesmo kWh, continuam em ordem de data;
//  - e o mesmo raciocinio que o adicionarConsumo() ja usa para manter a lista ordenada.
//
// Como funciona: percorre o vetor e, para cada item ("chave"), desloca para a direita
// os itens anteriores que "vem depois" dele e coloca a chave no buraco aberto.
// Ordena uma COPIA, entao o historico original continua em ordem cronologica.
// ---------------------------------------------------------------
void ordenarPorConsumo(vector<Consumo>& v, bool decrescente){
    for (int i = 1; i < (int)v.size(); i++){
        Consumo chave = v[i];
        int j = i - 1;
        while (j >= 0 && (decrescente ? v[j].kwh < chave.kwh : v[j].kwh > chave.kwh)){
            v[j + 1] = v[j];      // abre espaco deslocando para a direita
            j--;
        }
        v[j + 1] = chave;         // coloca a chave na posicao certa
    }
}

// ---------------------------------------------------------------
// BUSCA BINARIA por mes/ano
// Por que escolhi esta? O historico ja fica em ordem cronologica (o adicionarConsumo garante),
// entao da para "cortar o vetor ao meio" a cada passo: com 1000 meses sao no maximo ~10 comparacoes,
// enquanto a busca linear faria ate 1000.
// Retorna a posicao do mes ou -1 se nao existe.
// ---------------------------------------------------------------
int buscaBinaria(Infra& infra, int mes, int ano){
    int inicio = 0, fim = infra.qtdConsumos() - 1;
    int chave = ano * 100 + mes;                     // ex: 03/2026 -> 202603
    while (inicio <= fim){
        int meio = (inicio + fim) / 2;
        Consumo c = infra.getConsumo(meio);
        int atual = c.ano * 100 + c.mes;
        if (atual == chave) return meio;             // achou
        if (atual < chave) inicio = meio + 1;        // o mes esta na metade da direita
        else fim = meio - 1;                         // o mes esta na metade da esquerda
    }
    return -1;
}

void imprimirConsumo(Consumo c){
    cout << "  " << mesAno(c.mes, c.ano) << "  " << setw(9) << txt2(c.kwh) << " kWh   R$ " << txt2(c.valor) << endl;
}

void imprimirEstatisticas(Infra& infra){
    double totalKwh = 0, totalValor = 0;
    int iMaior = 0, iMenor = 0;
    for (int i = 0; i < infra.qtdConsumos(); i++){
        Consumo c = infra.getConsumo(i);
        totalKwh += c.kwh;
        totalValor += c.valor;
        if (c.kwh > infra.getConsumo(iMaior).kwh) iMaior = i;
        if (c.kwh < infra.getConsumo(iMenor).kwh) iMenor = i;
    }
    Consumo maior = infra.getConsumo(iMaior);
    Consumo menor = infra.getConsumo(iMenor);
    cout << "  ----------------------------------------------" << endl;
    cout << "  Total: " << txt2(totalKwh) << " kWh | R$ " << txt2(totalValor) << " | media " << txt2(totalKwh / infra.qtdConsumos()) << " kWh/mes" << endl;
    cout << "  Maior: " << mesAno(maior.mes, maior.ano) << " (" << txt2(maior.kwh) << " kWh) | Menor: "
         << mesAno(menor.mes, menor.ano) << " (" << txt2(menor.kwh) << " kWh)" << endl;
}

void executarHistorico(Infra& infra){
    if (infra.qtdConsumos() == 0){
        limparTela();
        cout << "=== MODULO 2: HISTORICO DOS MESES ANTERIORES ===" << endl;
        cout << "\nHistorico vazio. Cadastre em: Infraestrutura energetica > Historico de consumo." << endl;
        pausar();
        return;
    }

    while(true){
        limparTela();
        cout << "=== MODULO 2: HISTORICO DOS MESES ANTERIORES (" << infra.qtdConsumos() << " meses) ===" << endl;
        cout << "Dados extraidos do arquivo importado:\n" << endl;
        cout << "1. Listar em ordem cronologica" << endl;
        cout << "2. Ordenar por consumo (Insertion Sort)" << endl;
        cout << "3. Buscar mes/ano (Busca Binaria)" << endl;
        cout << "4. Filtrar por ano (Busca Linear)" << endl;
        cout << "0. Voltar" << endl;
        int n = lerInt("# --> ", 0, 4);

        if (n == 0) return;
        cout << endl;

        if (n == 1){
            for (int i = 0; i < infra.qtdConsumos(); i++) imprimirConsumo(infra.getConsumo(i));
            imprimirEstatisticas(infra);

        } else if (n == 2){
            cout << "1. Do maior para o menor consumo" << endl;
            cout << "2. Do menor para o maior consumo" << endl;
            int ordem = lerInt("# --> ", 1, 2);

            vector<Consumo> copia;                     // copia: nao desorganiza o original
            for (int i = 0; i < infra.qtdConsumos(); i++) copia.push_back(infra.getConsumo(i));
            ordenarPorConsumo(copia, ordem == 1);

            cout << endl;
            for (int i = 0; i < (int)copia.size(); i++){
                cout << setw(3) << i + 1 << ".";
                imprimirConsumo(copia[i]);
            }

        } else if (n == 3){
            int mes = lerInt("Mes (1-12): ", 1, 12);
            int ano = lerInt("Ano: ", 1990, 2100);
            int pos = buscaBinaria(infra, mes, ano);
            if (pos < 0) cout << "Nao ha registro de " << mesAno(mes, ano) << "." << endl;
            else imprimirConsumo(infra.getConsumo(pos));

        } else {
            int ano = lerInt("Ano: ", 1990, 2100);
            double totalKwh = 0, totalValor = 0;
            int achados = 0;
            for (int i = 0; i < infra.qtdConsumos(); i++){     // percorre tudo, um por um
                Consumo c = infra.getConsumo(i);
                if (c.ano == ano){
                    imprimirConsumo(c);
                    totalKwh += c.kwh;
                    totalValor += c.valor;
                    achados++;
                }
            }
            if (achados == 0) cout << "Sem registros em " << ano << "." << endl;
            else cout << "  Total " << ano << ": " << txt2(totalKwh) << " kWh | R$ " << txt2(totalValor)
                      << " | media " << txt2(totalKwh / achados) << " kWh/mes" << endl;
        }
        pausar();
    }
}
