#ifndef HASH_H
#define HASH_H

#include <fstream>
#include <cstring>
#include <array>

const int NUM_BUCKETS = 270973;
const int NUM_REGISTROS_POR_BLOCO = 7;

using namespace std;

struct Artigo {
    int id;
    char titulo[300];
    int ano;
    char autores[150];
    int citacoes;
    char dataAtualizacao[20];
    char resumo[1024];
};

struct Bloco {
    unsigned int numRegistros;
    array<Artigo, NUM_REGISTROS_POR_BLOCO> artigos;  // Array de registros do tipo Artigo
};

int funcaoHash(int id);
void imprimirArtigo(const Artigo& artigo);
void abrirArquivo(fstream* arquivo);
bool inserirRegistroNoArquivoHash(const Artigo& artigo, fstream* arquivo);
Bloco obterBloco(int id, fstream* arquivo);

#endif
