#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

#include "segundoIndice.hpp"
#include "hash1.hpp"

// Função para obter um bloco no arquivo de dados pela posição do ID
Bloco obterBlocoPorPosicao(fstream* arquivo, int id) {
    Bloco buffer = {0};
    int enderecoBucket = (id % NUM_BUCKETS) * sizeof(Bloco);

    // Posiciona o cursor no arquivo de dados na posição do bucket
    arquivo->seekg(enderecoBucket, std::ios::beg);
    arquivo->read(reinterpret_cast<char*>(&buffer), sizeof(Bloco));

    return buffer;
}

// Busca um registro no arquivo de dados por título
Artigo buscarRegistroPorTitulo(fstream* arquivo, int posicao, const char titulo[300]) {
    Bloco buffer = obterBlocoPorPosicao(arquivo, posicao);
    Artigo* artigos = buffer.artigos.data();

    // Percorre os registros do bloco para encontrar o título
    for (int i = 0; i < buffer.numRegistros; ++i) {
        if (strcmp(titulo, artigos[i].titulo) == 0) {
            imprimirArtigo(artigos[i]);
            return artigos[i];
        }
    }

    std::cout << "Registro não encontrado.\n";
    return Artigo();
}

// Função para carregar o cabeçalho do índice secundário
void obterCabecalho() {
    if (!cabecalhoIndiceSecundario) {
        cabecalhoIndiceSecundario = new CabecalhoIndiceSecundario;
    }

    arquivoIndiceSecundario->seekg(0, std::ios::beg);
    arquivoIndiceSecundario->read(reinterpret_cast<char*>(cabecalhoIndiceSecundario), sizeof(CabecalhoIndiceSecundario));
}

// Função para buscar título na árvore do índice secundário
int buscarTituloNaArvore(fstream* arquivoDados, fstream* arquivoIndice, const char titulo[300]) {
    arquivoDados = arquivoDados;
    arquivoIndiceSecundario = arquivoIndice;

    int posicao, minPos, maxPos, posPonteiro, blocosLidos = 0;
    NoIndiceSecundario* noAtual;

    // Carrega o cabeçalho do índice secundário
    obterCabecalho();
    posicao = cabecalhoIndiceSecundario->posicaoRaiz;

    // Percorre a árvore até encontrar o título ou a folha
    while (true) {
        noAtual = obterNoDoArquivoIndice(posicao);
        blocosLidos++;

        if (!noAtual) {
            std::cout << "Erro: Nó não carregado corretamente.\n";
            std::cout << "Registro com o título " << titulo << " não encontrado.\n";
            return -1;
        }

        if (noAtual->ponteiro[0] < 0) {
            minPos = 0;
            maxPos = noAtual->tamanho - 1;

            // Busca binária para encontrar a posição correta do ponteiro
            while (minPos <= maxPos) {
                posPonteiro = (minPos + maxPos) / 2;
                if (strcmp(titulo, noAtual->chave[posPonteiro]) < 0) {
                    maxPos = posPonteiro - 1;
                } else {
                    minPos = posPonteiro + 1;
                }
            }

            // Ajuste da posição do ponteiro
            posPonteiro = (maxPos < 0) ? 0 : maxPos + (strcmp(titulo, noAtual->chave[maxPos]) >= 0);
            posicao = noAtual->ponteiro[posPonteiro];
        } else {
            break;
        }
    }

    minPos = 0;
    maxPos = noAtual->tamanho - 1;
    while (minPos <= maxPos) {
        posPonteiro = (minPos + maxPos) / 2;
        if (strcmp(titulo, noAtual->chave[posPonteiro]) == 0) {
            break;
        } else if (strcmp(titulo, noAtual->chave[posPonteiro]) < 0) {
            maxPos = posPonteiro - 1;
        } else {
            minPos = posPonteiro + 1;
        }
    }

    buscarRegistroPorTitulo(arquivoDados, noAtual->ponteiro[posPonteiro], titulo);
    std::cout << "------------------------------------------------\n";
    std::cout << "Total de blocos de índice: " << cabecalhoIndiceSecundario->quantidadeNos << "\n";
    std::cout << "Quantidade de blocos lidos: " << blocosLidos << "\n";

    arquivoIndiceSecundario->close();
    arquivoDados->close();
    delete cabecalhoIndiceSecundario;

    return -1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Digite o título do registro a ser procurado.\nEx: ./seek2 <Titulo>\n";
        return 1;
    }

    std::string HASH_DATA = "dataHash";
    std::string SEC_INDEX = "indiceSecundario";
    fstream arquivoHash(HASH_DATA, std::fstream::in | std::ios::binary);
    fstream arquivoSecIndex(SEC_INDEX, std::fstream::in | std::ios::binary);

    buscarTituloNaArvore(&arquivoHash, &arquivoSecIndex, argv[1]);
    return 0;
}
