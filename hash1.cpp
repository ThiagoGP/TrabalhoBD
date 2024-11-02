#include <iostream>
#include <fstream>
#include <cstring>
#include "hash1.hpp"

using namespace std;

// Inicializa o arquivo para leitura e gravação, preenchendo-o com blocos vazios.
void abrirArquivo(fstream *arquivo) {
    cout << "Inicializando o arquivo (dataHash)..." << endl;
    Bloco blocoVazio = {0};

    for (int i = 0; i < NUM_BUCKETS; i++) {
        arquivo->write(reinterpret_cast<char*>(&blocoVazio), sizeof(Bloco));
    }

    if (arquivo->fail()) {
        cerr << "Erro ao escrever no arquivo de dados." << endl;
        exit(1);
    }
    cout << "Arquivo de dados inicializado com sucesso." << endl;
}

// Retorna o bloco correspondente ao ID, calculando o endereço do bloco com base no hash do ID
Bloco obterBloco(int id, fstream *arquivo) {
    int enderecoBloco = id % NUM_BUCKETS;
    Bloco bloco = {0};

    arquivo->seekg(enderecoBloco * sizeof(Bloco), ios::beg);
    arquivo->read(reinterpret_cast<char*>(&bloco), sizeof(Bloco));
    
    return bloco;
}

// Insere um registro no arquivo de dados organizado por hashing de forma ordenada
bool inserirRegistroNoArquivoHash(const Artigo& artigo, fstream *arquivo) {
    int enderecoBloco = artigo.id % NUM_BUCKETS;  // Calcula o endereço do bloco com base no ID
    Bloco bloco = obterBloco(artigo.id, arquivo);
    Artigo* listaArtigos = bloco.artigos.data();

    if (bloco.numRegistros < NUM_REGISTROS_POR_BLOCO) {
        int posicaoInsercao = bloco.numRegistros;

        // Encontra a posição de inserção para manter a ordem crescente
        for (int i = 0; i < bloco.numRegistros; i++) {
            if (artigo.id < listaArtigos[i].id) {
                posicaoInsercao = i;
                break;
            }
        }

        // Desloca registros para abrir espaço
        for (int j = bloco.numRegistros; j > posicaoInsercao; j--) {
            listaArtigos[j] = listaArtigos[j - 1];
        }

        // Insere o novo registro
        listaArtigos[posicaoInsercao] = artigo;
        bloco.numRegistros++;

        // Posiciona o ponteiro no início do bloco para sobrescrever com a nova informação
        arquivo->seekp(enderecoBloco * sizeof(Bloco), ios::beg);
        arquivo->write(reinterpret_cast<const char*>(&bloco), sizeof(Bloco));
        
        return true;
    } else {
        return false;
    }
}

// Imprime os dados de um artigo
void imprimirArtigo(const Artigo& artigo) {
    cout << "Id: " << artigo.id << endl;
    cout << "Título: " << artigo.titulo << endl;
    cout << "Ano: " << artigo.ano << endl;
    cout << "Autores: " << artigo.autores << endl;
    cout << "Citações: " << artigo.citacoes << endl;
    cout << "Data de Atualização: " << artigo.dataAtualizacao << endl;
    cout << "Resumo: " << artigo.resumo << endl;
}
