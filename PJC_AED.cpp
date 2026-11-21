// PROJETO FINAL COMPUTACIONAL - AED
// Gestao de Energia Inteligente
//
// Compilar:  g++ -std=c++11 PJC_AED.cpp -o PJC_AED

#include <iostream>
#include <string>
#include <fstream> // arquivos
#include <cstdlib> // limpar tela
#include "GERAL.h"
#include "persistencia.h"
using namespace std;

class Gestor {
    Cadastro MenuCadastro;
    // O Gestor e o DONO dos dados: um unico conjunto de objetos,
    // usado pelo cadastro, pela importacao e pelo menu principal.
    Data dadosImovel;
    Infra infraObj;
    Fonts fontsObj;
    Equipment equipmentObj;
    Distributor distributorObj;

    string arquivoAtual;   // arquivo em uso ("" = nenhum)
    bool carregado;        // true depois de importar, editar ou salvar um cadastro novo
public:
    Gestor(){
        arquivoAtual = "";
        carregado = false;
    }

    // ---------------- ARQUIVOS ----------------

    // Opcao 1 do menu inicial: importar um arquivo pronto
    void importarArquivoDinamico() {
        limparTela();
        cout << "--- IMPORTAR ARQUIVO DE MODELO ---" << endl;
        string nomeArq = nomeComTxt(lerTextoObrigatorio("Digite o nome do arquivo (ex: MeuTeste.txt): "));

        if (carregado && !lerSimNao("Isso SUBSTITUI o cadastro atual. Continuar?")){
            cout << "\nCancelado." << endl;
            pausar();
            return;
        }

        int aceitos, desconhecidos, invalidos;
        int r = Persistencia::importarArquivo(nomeArq, dadosImovel, infraObj, fontsObj, equipmentObj, distributorObj,
                                              aceitos, desconhecidos, invalidos);
        if (r == 1){
            cout << "\n[Erro] Falha ao abrir o arquivo '" << nomeArq << "'. Verifique o nome e tente novamente." << endl;
        } else if (r == 2){
            cout << "\n[Erro] Nenhum bloco reconhecido em '" << nomeArq << "'. Cadastro atual mantido." << endl;
        } else {
            arquivoAtual = nomeArq;
            carregado = true;
            cout << "\n[Sucesso] Informacoes do arquivo '" << nomeArq << "' importadas com sucesso!" << endl;
            cout << "Blocos aceitos: " << aceitos << " | tag desconhecida: " << desconhecidos
                 << " | dado invalido (ignorado): " << invalidos << endl;
            dadosImovel.mostrar();
            infraObj.mostrar();
            fontsObj.mostrar();
            equipmentObj.mostrar();
            distributorObj.mostrar();
        }
        pausar();
    }

    // Opcao 2 do menu inicial: criar um arquivo novo (cadastro manual)
    void CADASTRO(){
        limparTela();
        cout << "--- CRIAR NOVO ARQUIVO (CADASTRO) ---" << endl;
        string nomeArq = nomeComTxt(lerTextoObrigatorio("Nome do novo arquivo (ex: minha_casa.txt): "));

        ifstream teste(nomeArq.c_str());
        bool jaExiste = teste.is_open();
        teste.close();
        if (jaExiste && !lerSimNao("O arquivo '" + nomeArq + "' ja existe e sera SOBRESCRITO ao salvar. Continuar?")){
            cout << "\nCancelado." << endl;
            pausar();
            return;
        }

        // comeca um cadastro limpo (objetos novos, zerados)
        dadosImovel = Data();
        infraObj = Infra();
        fontsObj = Fonts();
        equipmentObj = Equipment();
        distributorObj = Distributor();
        arquivoAtual = nomeArq;
        carregado = false;      // so libera o Menu Principal depois de salvar

        MenuCadastro.MenuCadastro(dadosImovel, infraObj, fontsObj, equipmentObj, distributorObj, arquivoAtual, carregado);
    }

    // Opcao 3 do menu inicial: alterar/editar um arquivo existente
    // (importa o arquivo e abre o mesmo menu do cadastro; no fim, salva no mesmo arquivo)
    void editarArquivo(){
        limparTela();
        cout << "--- ALTERAR / EDITAR ARQUIVO EXISTENTE ---" << endl;
        string nomeArq = nomeComTxt(lerTextoObrigatorio("Nome do arquivo existente: "));

        int aceitos, desconhecidos, invalidos;
        int r = Persistencia::importarArquivo(nomeArq, dadosImovel, infraObj, fontsObj, equipmentObj, distributorObj,
                                              aceitos, desconhecidos, invalidos);
        if (r == 1){
            cout << "\n[Erro] Arquivo '" << nomeArq << "' nao encontrado." << endl;
            pausar();
            return;
        }
        if (r == 2){
            cout << "\n[Erro] Nenhum bloco reconhecido em '" << nomeArq << "'." << endl;
            pausar();
            return;
        }

        arquivoAtual = nomeArq;
        carregado = true;
        cout << "\n[Sucesso] '" << nomeArq << "' carregado (" << aceitos << " blocos). Abrindo o menu de edicao..." << endl;
        pausar();

        MenuCadastro.MenuCadastro(dadosImovel, infraObj, fontsObj, equipmentObj, distributorObj, arquivoAtual, carregado);
    }

    // ---------------- RELATORIO ----------------
    // Opcao 1 do menu principal: mostra tudo que esta cadastrado
    void mostrarRelatorio(){
        limparTela();
        cout << "============ RELATORIO CADASTRADO ============" << endl;
        cout << "Arquivo: " << arquivoAtual << endl;
        dadosImovel.mostrar();
        infraObj.mostrar();
        fontsObj.mostrar();
        equipmentObj.mostrar();
        distributorObj.mostrar();

        double dia = equipmentObj.consumoDiarioKwh();
        double pico = equipmentObj.potenciaInstaladaKw() * FATOR_SIMULTANEIDADE;
        cout << "\n--- Indicadores estimados ---" << endl;
        cout << "Consumo dos aparelhos: " << txt2(dia) << " kWh/dia | " << txt2(dia * DIAS_MES) << " kWh/mes" << endl;
        cout << "Custo estimado: R$ " << txt2(dia * DIAS_MES * distributorObj.get_tarifa()) << "/mes" << endl;
        cout << "Pico estimado: " << txt2(pico) << " kW (fator " << FATOR_SIMULTANEIDADE << ") de "
             << txt2(infraObj.get_capacidade()) << " kW do painel";
        if (infraObj.get_capacidade() > 0) cout << " (" << txt2(100.0 * pico / infraObj.get_capacidade()) << "%)";
        cout << endl;
        cout << "Geracao propria estimada: " << txt2(fontsObj.geracaoDiariaKwh()) << " kWh/dia";
        if (dia > 0 && fontsObj.geracaoDiariaKwh() > 0) cout << " (cobre " << txt2(100.0 * fontsObj.geracaoDiariaKwh() / dia) << "% do consumo)";
        cout << endl;
        pausar();
    }

    // ---------------- ACESSO AO MENU PRINCIPAL ----------------
    bool podeAcessarMenuPrincipal(){
        if (carregado) return true;
        cout << "\n[Aviso] Importe um arquivo (opcao 1), edite um existente (opcao 3)" << endl;
        cout << "        ou crie e SALVE um cadastro (opcao 2) antes de acessar o Menu Principal." << endl;
        pausar();
        return false;
    }

    // APOS CADASTRO ====================
    void MainMenu(); // assinada sem parametro para uso em main()

    // TRANSFORMAR EM CLASSE FILHA
    void visualizarReservaEnergia(){ executarReservaEnergia(fontsObj, equipmentObj); }
    void historicoGastosAnteriores(){ executarHistorico(infraObj); }
    void previsaoEnergeticaSazonal();
    void diagnosticoEscassez();
    void mapeamentoRedeEletrica(){ executarMapeamento(dadosImovel, infraObj, fontsObj, equipmentObj, distributorObj); }
};

// ====================================================================
// CADASTRO & MAIN
// ====================================================================

int main(){
    Gestor g;

    while(true){
        limparTela();
        cout << "\n========= GESTAO DE ENERGIA INTELIGENTE =========" << endl;
        cout << "# Bem vindo ao seu mais novo gestor de energia!" << endl;
        cout << "# Para prosseguir, escolha uma das opcoes: " << endl;
        cout << "1. Importar um arquivo pronto" << endl;
        cout << "2. Criar um arquivo novo (cadastro)" << endl;
        cout << "3. Alterar/Editar um arquivo existente" << endl;
        cout << "4. Acessar Menu Principal (Apos cadastro/importacao)" << endl;
        cout << "0. Fechar programa" << endl;
        cout << "=================================================" << endl;
        int n = lerInt("# ---> ", 0, 4);

        switch(n){
            case 1:
                g.importarArquivoDinamico(); // Pede o nome do arquivo ao usuario
                break;
            case 2:
                g.CADASTRO();
                break;
            case 3:
                g.editarArquivo();
                break;
            case 4:
                if (g.podeAcessarMenuPrincipal()) g.MainMenu();
                break;
            case 0:
                cout << "\nFechando..." << endl;
                return 0;
        }
    }
    return 0;
}

// ====================================================================
// CONTINUACAO APOS CADASTRO
// ====================================================================

void Gestor::MainMenu(){
    while(true){
        limparTela();
        cout << "\n========= GESTAO DE ENERGIA INTELIGENTE - MENU PRINCIPAL =========" << endl;
        cout << "Arquivo: " << arquivoAtual << endl;
        cout << "1. Mostrar Relatorio Cadastrado" << endl;
        cout << "2. Visualizar Reserva de Energia (Tabela Hash / Baterias)" << endl;
        cout << "3. Historico de Gastos Anteriores (Busca e Ordenacao)" << endl;
        cout << "4. Previsao Energetica Sazonal (Medias Moveis)" << endl;
        cout << "5. Diagnostico de Escassez (Arvore de Decisao Booleana)" << endl;
        cout << "6. Mapeamento da Rede Eletrica (Arvore N-aria para organizacao)" << endl;
        cout << "0. Voltar ao menu anterior" << endl;
        cout << "================================================================" << endl;
        int n = lerInt("# ---> ", 0, 6);

        switch(n){
            case 1:
                mostrarRelatorio();
                break;
            case 2:
                visualizarReservaEnergia();
                break;
            case 3:
                historicoGastosAnteriores();
                break;
            case 4:
                previsaoEnergeticaSazonal();
                break;
            case 5:
                diagnosticoEscassez();
                break;
            case 6:
                mapeamentoRedeEletrica();
                break;
            case 0:
                return;
        }
    }
}

// Estes dois modulos continuam como placeholders (so foram adaptados para as listas)
void Gestor::previsaoEnergeticaSazonal(){
    limparTela();
    cout << "=== MODULO 3: PREVISAO E GERACAO LOCAL ===" << endl;
    cout << "Dados extraidos do arquivo importado:\n" << endl;
    if (fontsObj.qtdGeracoes() == 0) cout << "-> Nenhuma geracao local cadastrada." << endl;
    for (int i = 0; i < fontsObj.qtdGeracoes(); i++){
        Geracao g = fontsObj.getGeracao(i);
        cout << "-> Tipo de Geracao de Energia: " << g.tipo << endl;
        cout << "-> Quantidade de Paineis/Geradores: " << g.qtd << endl;
        cout << "-> Potencia Nominal: " << txt(g.kwp) << " kWp (cada)" << endl;
    }
    cout << "-> Geracao diaria estimada: " << txt2(fontsObj.geracaoDiariaKwh()) << " kWh/dia" << endl;
    cout << "\n[Em construcao] A analise por medias moveis ainda sera implementada." << endl;
    pausar();
}

void Gestor::diagnosticoEscassez(){
    limparTela();
    cout << "=== MODULO 4: DIAGNOSTICO DE RISCO DE ESCASSEZ ===" << endl;
    cout << "Avaliando parametros reais da infraestrutura:\n" << endl;
    cout << "- Voltagem da Rede: " << infraObj.get_voltagem() << " V" << endl;
    cout << "- Capacidade Maxima Suportada: " << txt2(infraObj.get_capacidade()) << " kW" << endl;
    double pico = equipmentObj.potenciaInstaladaKw() * FATOR_SIMULTANEIDADE;
    cout << "- Pico de demanda estimado: " << txt2(pico) << " kW" << endl;

    if (infraObj.get_capacidade() <= 0){
        cout << "\n[Alerta] Dados de capacidade nao encontrados no arquivo!" << endl;
    } else if (pico > infraObj.get_capacidade()){
        cout << "\n[Resultado] RISCO ALTO: o pico estimado excede a capacidade do painel." << endl;
    } else {
        cout << "\n[Resultado] O pico estimado esta dentro da capacidade do painel." << endl;
    }
    cout << "[Em construcao] A arvore de decisao completa ainda sera implementada." << endl;
    pausar();
}
