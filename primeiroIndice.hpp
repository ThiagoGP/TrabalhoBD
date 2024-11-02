#ifndef INDICE_PRIMARIO
#define INDICE_PRIMARIO

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
// #include "config.hpp"
#include "hash1.hpp"

using namespace std;

// Ordem da árvore B+
const int ORDEM_ARVORE = 1000;

// Quantidade de ponteiros por nó
const int PONTEIROS_POR_NO = 2 * ORDEM_ARVORE;

// Quantidade de chaves por nó
const int CHAVES_POR_NO = PONTEIROS_POR_NO - 1;

// Estrutura que armazena as chaves e os ponteiros para os registros
struct DadosNo {
    int tamanhoNo;                  // Número de chaves armazenadas no nó
    int posicaoNo;                  // Posição do nó no arquivo
    int chave[PONTEIROS_POR_NO];    // Array para armazenar as chaves
    int ponteiro[PONTEIROS_POR_NO + 1]; // Array para armazenar ponteiros para filhos ou registros
};

// Estrutura auxiliar para conexão entre os nós da árvore
struct ConexaoNo {
    int chave;
    int ponteiroEsquerdo;
    int ponteiroDireito;
};

// Estrutura para armazenar o cabeçalho do índice primário
struct CabecalhoIndicePrimario {
    int posicaoRaiz;
    int numeroDeNos;
};

// Função de inserção no arquivo de índice primário
void inserirNoArquivoIndicePrimario(fstream* arquivoHash, fstream* arquivoIndicePrimario);

#endif
