#ifndef SECUNDARIO_INDICE_H
#define SECUNDARIO_INDICE_H

#include <iostream>
#include <fstream>
#include <string>
#include "hash1.hpp"

using namespace std;

// // Define a ordem da árvore B+
constexpr int ORDEM_ARVORE1 = 50;

// // Define a quantidade de apontadores e chaves para cada nó
constexpr int PONTEIROS_POR_NO1 = 2 * ORDEM_ARVORE1 + 1;
constexpr int CHAVES_POR_NO1 = 2 * ORDEM_ARVORE1;

// Estrutura do nó de índice secundário
struct NoIndiceSecundario {
    int tamanho;  // Quantidade de chaves ocupadas no nó
    int posicao;  // Posição do nó no arquivo de índice
    char chave[CHAVES_POR_NO1][300];  // Chaves de busca do nó (títulos)
    int ponteiro[PONTEIROS_POR_NO1];  // Apontadores do nó
};

// Estrutura auxiliar para divisão de nós
struct NoAuxiliar {
    char chave[300];  // Chave do nó (título)
    int ponteiroEsquerdo;  // Ponteiro da esquerda
    int ponteiroDireito;   // Ponteiro da direita
};

// Estrutura do cabeçalho do índice secundário
struct CabecalhoIndiceSecundario {
    int posicaoRaiz;      // Posição da raiz
    int quantidadeNos;    // Quantidade de nós
};

// Declaração de variáveis globais
extern CabecalhoIndiceSecundario* cabecalhoIndiceSecundario;
extern fstream* arquivoDados, *arquivoIndiceSecundario;

// Funções para manipulação do índice secundário
NoIndiceSecundario* obterNoDoArquivoIndice(int posicao);
int buscarTituloNaArvore(fstream* arquivoDados, fstream* arquivoIndice, const char chave[300]);
void inserirNoArquivoIndiceSecundario(fstream* arquivoHash, fstream* arquivoIndiceSecundario);

#endif
