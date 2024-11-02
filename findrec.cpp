#include <iostream>
#include <fstream>
#include <string>
#include "hash1.hpp"

using namespace std;

bool registroEncontrado = false;

/* Utiliza a função hash para obter o bloco correspondente ao ID informado.
   Em seguida, faz uma busca binária no bloco para encontrar o registro. */
Artigo buscarRegistroPorId(fstream* arquivo, int id) {
    Bloco bloco = obterBloco(id, arquivo);  // Obtém o bloco correspondente com base no ID dado
    Artigo* vetorArtigos = bloco.artigos.data();  // Aponta para o vetor de artigos dentro do bloco

    int inicio = 0;
    int fim = bloco.numRegistros - 1;

    // Busca binária para localizar o artigo no bloco
    while (fim >= inicio) {
        int meio = (inicio + fim) / 2;

        if (vetorArtigos[meio].id == id) {  // Registro encontrado
            registroEncontrado = true;
            return vetorArtigos[meio];
        }
        if (vetorArtigos[meio].id < id) {  // Registro está na metade superior do intervalo
            inicio = meio + 1;
        } else {                           // Registro está na metade inferior do intervalo
            fim = meio - 1;
        }
    }

    return {};  // Retorna um artigo vazio se não encontrar o ID
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "ID não fornecido.\nUse: ./findrec <ID>\n";
        exit(1);
    }

    int id = atoi(argv[1]);
    Artigo artigo;

    fstream* arquivoHash = new fstream("dataHash", fstream::in | ios::binary);

    if (!arquivoHash->is_open()) {
        cout << "Erro ao abrir o arquivo.\n";
        exit(1);
    } else {
        artigo = buscarRegistroPorId(arquivoHash, id);

        if (registroEncontrado) {
            cout << "Blocos lidos: 1" << endl;
            cout << "Quantidade total de blocos no arquivo de dados: " << NUM_BUCKETS << endl;
            cout << "-------------------------------------------------------------------\n";
            cout << "--> Artigo encontrado:\n";
            imprimirArtigo(artigo);
        } else {
            cout << "Registro não encontrado.\n";
        }
    }

    arquivoHash->close();
    delete arquivoHash;
    return 0;
}
