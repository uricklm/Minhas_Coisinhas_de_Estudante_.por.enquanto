#pragma once
// ==============================================================
// MODULO 1 - RESERVA DE ENERGIA
// Usa uma TABELA HASH para buscar uma bateria pelo ID.
// ==============================================================
#include "fonts.h"
#include "equipment.h"

const int TAM_HASH = 7;   // numero de "gavetas" (baldes) da tabela

// Tabela hash com ENCADEAMENTO: cada gaveta guarda uma lista de baterias.
// Se duas baterias caem na mesma gaveta (colisao), ficam as duas na lista.
class TabelaHashBaterias {
private:
    vector<Bateria> balde[TAM_HASH];   // um vetor de listas

    // FUNCAO HASH: transforma o ID num numero de 0 a TAM_HASH-1
    int funcaoHash(string id){
        int h = 0;
        for (int i = 0; i < (int)id.size(); i++)
            h = (h * 31 + (unsigned char)id[i]) % TAM_HASH;
        return h;
    }

public:
    void inserir(Bateria b){
        int gaveta = funcaoHash(minusculas(b.id));
        balde[gaveta].push_back(b);
    }

    // procura so dentro da gaveta do ID (nao precisa olhar todas as baterias)
    bool buscar(string id, Bateria& achada, int& gaveta){
        gaveta = funcaoHash(minusculas(id));
        for (int i = 0; i < (int)balde[gaveta].size(); i++){
            if (minusculas(balde[gaveta][i].id) == minusculas(id)){
                achada = balde[gaveta][i];
                return true;
            }
        }
        return false;
    }

    void listar(){
        for (int h = 0; h < TAM_HASH; h++){
            cout << "  gaveta " << h << ": ";
            if (balde[h].empty()) cout << "(vazia)";
            for (int i = 0; i < (int)balde[h].size(); i++)
                cout << "[" << balde[h][i].id << " | " << txt(balde[h][i].capKwh) << " kWh | " << balde[h][i].nivel << "%] ";
            cout << endl;
        }
    }
};

void executarReservaEnergia(Fonts& fontes, Equipment& equip){
    // monta a tabela com as baterias do cadastro
    TabelaHashBaterias tabela;
    for (int i = 0; i < fontes.qtdBaterias(); i++) tabela.inserir(fontes.getBateria(i));

    while(true){
        limparTela();
        cout << "=== MODULO 1: MONITOR DE BATERIAS (RESERVA DE ENERGIA) ===" << endl;
        cout << "Dados extraidos do arquivo importado:\n" << endl;
        cout << "-> Baterias cadastradas: " << fontes.qtdBaterias() << endl;
        cout << "-> Capacidade total de armazenamento: " << txt2(fontes.capacidadeBateriasKwh()) << " kWh" << endl;
        cout << "-> Energia armazenada agora: " << txt2(fontes.energiaArmazenadaKwh()) << " kWh" << endl;

        if (fontes.qtdBaterias() == 0){
            cout << "\nNenhuma bateria cadastrada." << endl;
            pausar();
            return;
        }

        cout << "\n1. Mostrar a tabela hash (por gaveta)" << endl;
        cout << "2. Buscar bateria por ID" << endl;
        cout << "3. Autonomia da reserva" << endl;
        cout << "0. Voltar" << endl;
        int n = lerInt("# --> ", 0, 3);

        if (n == 0) return;
        cout << endl;

        if (n == 1){
            tabela.listar();
        } else if (n == 2){
            string id = lerLinha("ID da bateria: ");
            Bateria b;
            int gaveta;
            if (tabela.buscar(id, b, gaveta)){
                cout << "Achada na gaveta " << gaveta << ": " << b.id << " - " << txt(b.capKwh) << " kWh, carga "
                     << b.nivel << "% (" << txt2(b.capKwh * b.nivel / 100.0) << " kWh disponiveis)" << endl;
            } else {
                cout << "Bateria '" << id << "' nao encontrada." << endl;
            }
        } else {
            double armazenado = fontes.energiaArmazenadaKwh();
            double consumoHora = equip.consumoDiarioKwh() / 24.0;
            cout << "Energia armazenada: " << txt2(armazenado) << " kWh" << endl;
            if (consumoHora > 0)
                cout << "Autonomia com o consumo medio dos aparelhos (" << txt2(consumoHora) << " kW): "
                     << txt2(armazenado / consumoHora) << " horas" << endl;
            else
                cout << "Cadastre equipamentos para calcular a autonomia." << endl;
        }
        pausar();
    }
}
