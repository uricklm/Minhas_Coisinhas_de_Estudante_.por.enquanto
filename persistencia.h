#pragma once
#include <fstream>
#include <vector>
#include "entrada.h"
#include "data.h"
#include "infra.h"
#include "fonts.h"
#include "equipment.h"
#include "distributor.h"

// ==============================================================
// FORMATO DO ARQUIVO (o MESMO para salvar e importar):
//
//   [TAG]
//   chave=valor
//   [/TAG]
//
// Linhas vazias e linhas que comecam com # sao comentarios.
// Blocos unicos:      DATA, INFRA, DISTRIBUIDORA
// Blocos repetiveis:  CONSUMO, GERACAO, BATERIA, OUTRA_FONTE,
//                     ELETRO, HVAC, LAMPADA, EV
// ==============================================================

// Um bloco lido do arquivo: a tag e a lista de pares chave/valor
struct Bloco {
    string tag;
    vector<string> chaves;
    vector<string> valores;
};

class Persistencia {
private:
    // devolve o valor de uma chave do bloco ("" se nao existir)
    static string valorDe(Bloco& b, string chave){
        for (int k = 0; k < (int)b.chaves.size(); k++)
            if (b.chaves[k] == chave) return b.valores[k];
        return "";
    }

    // devolve o valor numerico da chave (ou "padrao" se faltar ou estiver invalido)
    // aceita decimais: "0.45" vira 0.45 (antes o stoi() transformava em 0)
    static double numeroDe(Bloco& b, string chave, double padrao){
        string v = valorDe(b, chave);
        if (!ehNumero(v)) return padrao;
        return paraNumero(v);
    }

    // Le o proximo bloco a partir da posicao "pos" da lista de linhas.
    // Retorna false quando nao ha mais blocos.
    static bool proximoBloco(vector<string>& linhas, int& pos, Bloco& b){
        b.tag = "";
        b.chaves.clear();
        b.valores.clear();

        while (pos < (int)linhas.size()){
            string linha = linhas[pos];

            if (linha == "" || linha[0] == '#'){ pos++; continue; }   // vazia ou comentario

            if (linha[0] == '['){
                if (linha.size() > 1 && linha[1] == '/'){              // [/TAG] fecha o bloco
                    pos++;
                    if (b.tag != "") return true;
                    continue;
                }
                if (b.tag != "") return true;                          // abriu outro sem fechar o anterior
                size_t fim = linha.find(']');
                if (fim != string::npos) b.tag = maiusculas(trim(linha.substr(1, fim - 1)));
                pos++;
                continue;
            }

            size_t igual = linha.find('=');
            if (igual != string::npos && b.tag != ""){
                b.chaves.push_back(minusculas(trim(linha.substr(0, igual))));
                b.valores.push_back(trim(linha.substr(igual + 1)));
            }
            pos++;
        }
        return b.tag != "";
    }

    // Entrega o bloco para a classe certa.
    // Retorna: 1 = aceito | 0 = tag desconhecida | -1 = reconhecido mas com dado invalido
    static int aplicarBloco(Bloco& b, Data& d, Infra& i, Fonts& f, Equipment& e, Distributor& dist){
        if (b.tag == "DATA"){
            d.set_tipoImovel(valorDe(b, "tipo"));
            d.set_endereco(valorDe(b, "endereco"));
            d.set_cep(valorDe(b, "cep"));
            d.set_areaTotal((float)numeroDe(b, "area_m2", 0));
            d.set_comodos((int)numeroDe(b, "comodos", 0));
            d.set_anoConstr((int)numeroDe(b, "ano_construcao", 0));
            d.set_latitude(valorDe(b, "latitude"));
            d.set_longitude(valorDe(b, "longitude"));
            return 1;
        }
        if (b.tag == "INFRA"){
            i.set_capacidade(numeroDe(b, "capacidade_kw", 0));
            i.set_voltagem((int)numeroDe(b, "voltagem_v", 0));
            return 1;
        }
        if (b.tag == "DISTRIBUIDORA"){
            dist.set_distribuidora(valorDe(b, "empresa"));
            dist.set_tipo_tarifa(valorDe(b, "tipo_tarifa"));
            dist.set_tarifa(numeroDe(b, "tarifa_kwh", 0));
            return 1;
        }
        if (b.tag == "CONSUMO"){
            Consumo c;
            c.ano = (int)numeroDe(b, "ano", 0);
            c.mes = (int)numeroDe(b, "mes", 0);
            c.kwh = numeroDe(b, "kwh", -1);
            c.valor = numeroDe(b, "valor", 0);
            return i.adicionarConsumo(c) ? 1 : -1;
        }
        if (b.tag == "GERACAO"){
            Geracao g;
            g.tipo = valorDe(b, "tipo");
            g.qtd = (int)numeroDe(b, "qtd", 1);
            g.kwp = numeroDe(b, "potencia_kwp", 0);
            return f.adicionarGeracao(g) ? 1 : -1;
        }
        if (b.tag == "BATERIA"){
            Bateria bat;
            bat.id = valorDe(b, "id");
            bat.capKwh = numeroDe(b, "capacidade_kwh", 0);
            bat.nivel = (int)numeroDe(b, "nivel_pct", 0);
            return f.adicionarBateria(bat) ? 1 : -1;
        }
        if (b.tag == "OUTRA_FONTE"){
            OutraFonte o;
            o.tipo = valorDe(b, "tipo");
            o.qtd = (int)numeroDe(b, "qtd", 1);
            o.kw = numeroDe(b, "potencia_kw", 0);
            return f.adicionarOutra(o) ? 1 : -1;
        }
        if (b.tag == "ELETRO"){
            Eletro x;
            x.nome = valorDe(b, "nome");
            x.qtd = (int)numeroDe(b, "qtd", 1);
            x.potenciaW = numeroDe(b, "potencia_w", 0);
            x.horasDia = numeroDe(b, "horas_dia", 0);
            x.modo = valorDe(b, "modo");
            if (x.modo == "") x.modo = "convencional";
            return e.adicionarEletro(x) ? 1 : -1;
        }
        if (b.tag == "HVAC"){
            Hvac h;
            h.tipo = valorDe(b, "tipo");
            h.qtd = (int)numeroDe(b, "qtd", 1);
            h.potenciaW = numeroDe(b, "potencia_w", 0);
            h.horasDia = numeroDe(b, "horas_dia", 0);
            h.modo = valorDe(b, "modo");
            if (h.modo == "") h.modo = "convencional";
            return e.adicionarHvac(h) ? 1 : -1;
        }
        if (b.tag == "LAMPADA"){
            Lampada l;
            l.ambiente = valorDe(b, "ambiente");
            l.tipo = valorDe(b, "tipo");
            if (l.tipo == "") l.tipo = "LED";
            l.qtd = (int)numeroDe(b, "qtd", 1);
            l.potenciaW = numeroDe(b, "potencia_w", 0);
            l.horasDia = numeroDe(b, "horas_dia", 0);
            l.controle = valorDe(b, "controle");
            if (l.controle == "") l.controle = "interruptor";
            return e.adicionarLampada(l) ? 1 : -1;
        }
        if (b.tag == "EV"){
            Carregador c;
            c.nome = valorDe(b, "nome");
            if (c.nome == "") c.nome = "Carregador EV";
            c.potenciaKw = numeroDe(b, "potencia_kw", 0);
            c.horasDia = numeroDe(b, "horas_dia", 0);
            return e.adicionarEV(c) ? 1 : -1;
        }
        return 0;
    }

public:
    // Salva TUDO no formato acima (inclusive as listas inteiras)
    static bool salvarDados(const string& nomeArquivo, Data& d, Infra& i, Fonts& f, Equipment& e, Distributor& dist){
        ofstream arquivo(nomeArquivo.c_str());
        if (!arquivo.is_open()){
            cerr << "Erro ao abrir o arquivo para salvamento!" << endl;
            return false;
        }
        arquivo << setprecision(10);
        arquivo << "# Gestao de Energia Inteligente - dados cadastrados\n\n";

        // ---- Data ----
        arquivo << "[DATA]\n";
        arquivo << "tipo=" << d.get_tipoImovel() << "\n";
        arquivo << "endereco=" << d.get_endereco() << "\n";
        arquivo << "cep=" << d.get_cep() << "\n";
        arquivo << "area_m2=" << d.get_areaTotal() << "\n";
        arquivo << "comodos=" << d.get_comodos() << "\n";
        arquivo << "ano_construcao=" << d.get_anoConstr() << "\n";
        arquivo << "latitude=" << d.get_latitude() << "\n";
        arquivo << "longitude=" << d.get_longitude() << "\n";
        arquivo << "[/DATA]\n\n";

        // ---- Infra ----
        arquivo << "[INFRA]\n";
        arquivo << "capacidade_kw=" << i.get_capacidade() << "\n";
        arquivo << "voltagem_v=" << i.get_voltagem() << "\n";
        arquivo << "[/INFRA]\n\n";

        for (int k = 0; k < i.qtdConsumos(); k++){
            Consumo c = i.getConsumo(k);
            arquivo << "[CONSUMO]\n";
            arquivo << "ano=" << c.ano << "\n";
            arquivo << "mes=" << c.mes << "\n";
            arquivo << "kwh=" << c.kwh << "\n";
            arquivo << "valor=" << c.valor << "\n";
            arquivo << "[/CONSUMO]\n\n";
        }

        // ---- Fonts ----
        for (int k = 0; k < f.qtdGeracoes(); k++){
            Geracao g = f.getGeracao(k);
            arquivo << "[GERACAO]\n";
            arquivo << "tipo=" << g.tipo << "\n";
            arquivo << "qtd=" << g.qtd << "\n";
            arquivo << "potencia_kwp=" << g.kwp << "\n";
            arquivo << "[/GERACAO]\n\n";
        }
        for (int k = 0; k < f.qtdBaterias(); k++){
            Bateria b = f.getBateria(k);
            arquivo << "[BATERIA]\n";
            arquivo << "id=" << b.id << "\n";
            arquivo << "capacidade_kwh=" << b.capKwh << "\n";
            arquivo << "nivel_pct=" << b.nivel << "\n";
            arquivo << "[/BATERIA]\n\n";
        }
        for (int k = 0; k < f.qtdOutras(); k++){
            OutraFonte o = f.getOutra(k);
            arquivo << "[OUTRA_FONTE]\n";
            arquivo << "tipo=" << o.tipo << "\n";
            arquivo << "qtd=" << o.qtd << "\n";
            arquivo << "potencia_kw=" << o.kw << "\n";
            arquivo << "[/OUTRA_FONTE]\n\n";
        }

        // ---- Equipment ----
        for (int k = 0; k < e.qtdEletros(); k++){
            Eletro x = e.getEletro(k);
            arquivo << "[ELETRO]\n";
            arquivo << "nome=" << x.nome << "\n";
            arquivo << "qtd=" << x.qtd << "\n";
            arquivo << "potencia_w=" << x.potenciaW << "\n";
            arquivo << "horas_dia=" << x.horasDia << "\n";
            arquivo << "modo=" << x.modo << "\n";
            arquivo << "[/ELETRO]\n\n";
        }
        for (int k = 0; k < e.qtdHvac(); k++){
            Hvac h = e.getHvac(k);
            arquivo << "[HVAC]\n";
            arquivo << "tipo=" << h.tipo << "\n";
            arquivo << "qtd=" << h.qtd << "\n";
            arquivo << "potencia_w=" << h.potenciaW << "\n";
            arquivo << "horas_dia=" << h.horasDia << "\n";
            arquivo << "modo=" << h.modo << "\n";
            arquivo << "[/HVAC]\n\n";
        }
        for (int k = 0; k < e.qtdLampadas(); k++){
            Lampada l = e.getLampada(k);
            arquivo << "[LAMPADA]\n";
            arquivo << "ambiente=" << l.ambiente << "\n";
            arquivo << "tipo=" << l.tipo << "\n";
            arquivo << "qtd=" << l.qtd << "\n";
            arquivo << "potencia_w=" << l.potenciaW << "\n";
            arquivo << "horas_dia=" << l.horasDia << "\n";
            arquivo << "controle=" << l.controle << "\n";
            arquivo << "[/LAMPADA]\n\n";
        }
        for (int k = 0; k < e.qtdEV(); k++){
            Carregador c = e.getEV(k);
            arquivo << "[EV]\n";
            arquivo << "nome=" << c.nome << "\n";
            arquivo << "potencia_kw=" << c.potenciaKw << "\n";
            arquivo << "horas_dia=" << c.horasDia << "\n";
            arquivo << "[/EV]\n\n";
        }

        // ---- Distributor ----
        arquivo << "[DISTRIBUIDORA]\n";
        arquivo << "empresa=" << dist.get_distribuidora() << "\n";
        arquivo << "tipo_tarifa=" << dist.get_tipo_tarifa() << "\n";
        arquivo << "tarifa_kwh=" << dist.get_tarifa() << "\n";
        arquivo << "[/DISTRIBUIDORA]\n";

        arquivo.close();
        return true;
    }

    // Importa um arquivo para as classes.
    // Retorna: 0 = sucesso | 1 = nao abriu o arquivo | 2 = nenhum bloco reconhecido
    // Os contadores dizem quantos blocos foram aceitos, de tag desconhecida e invalidos.
    // So substitui o cadastro atual se o arquivo for valido (le primeiro em objetos temporarios).
    static int importarArquivo(const string& nomeArquivo, Data& d, Infra& i, Fonts& f, Equipment& e, Distributor& dist,
                               int& aceitos, int& desconhecidos, int& invalidos){
        aceitos = 0;
        desconhecidos = 0;
        invalidos = 0;

        ifstream arquivo(nomeArquivo.c_str());
        if (!arquivo.is_open()) return 1;

        // 1) le o arquivo inteiro para uma lista de linhas (sem espacos e sem o '\r' do Windows)
        vector<string> linhas;
        string linha;
        while (getline(arquivo, linha)) linhas.push_back(trim(linha));
        arquivo.close();

        // 2) percorre bloco por bloco, preenchendo objetos NOVOS (temporarios)
        Data d2;
        Infra i2;
        Fonts f2;
        Equipment e2;
        Distributor dist2;

        int pos = 0;
        Bloco b;
        while (proximoBloco(linhas, pos, b)){
            int r = aplicarBloco(b, d2, i2, f2, e2, dist2);
            if (r == 1) aceitos++;
            else if (r == 0) desconhecidos++;
            else invalidos++;
        }
        if (aceitos == 0) return 2;

        // 3) arquivo bom: troca o cadastro atual pelo novo
        d = d2;
        i = i2;
        f = f2;
        e = e2;
        dist = dist2;
        return 0;
    }
};
