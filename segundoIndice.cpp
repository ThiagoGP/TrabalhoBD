#include <iostream>
#include <fstream>
#include <cstring>
#include "segundoIndice.hpp"
#include "hash1.hpp"

using namespace std;

// Declaração de variáveis globais
CabecalhoIndiceSecundario* cabecalhoIndiceSecundario = nullptr;
fstream *arquivoDados, *arquivoIndiceSecundario;

// Atualiza o cabeçalho do arquivo de índice secundário
void atualizarCabecalhoIndiceSecundario() {
    // Posiciona o cursor no início do arquivo e escreve o cabeçalho atualizado
    arquivoIndiceSecundario->seekp(0, ios::beg);
    arquivoIndiceSecundario->write(reinterpret_cast<char*>(cabecalhoIndiceSecundario), sizeof(CabecalhoIndiceSecundario));
}

// Inicializa o cabeçalho do índice secundário
void inicializarCabecalho() {
    /* Inicializa o cabeçalho do arquivo de índice secundário */
    cabecalhoIndiceSecundario = new CabecalhoIndiceSecundario;
    if (!cabecalhoIndiceSecundario) {
        std::cerr << "Erro ao alocar memória para o cabeçalho." << std::endl;
        exit(EXIT_FAILURE);
    }
    cabecalhoIndiceSecundario->posicaoRaiz = -1;
    cabecalhoIndiceSecundario->quantidadeNos = 0;
    atualizarCabecalhoIndiceSecundario();
}

// Cria um novo nó para o índice secundário
NoIndiceSecundario* criarNoIndiceSecundario() {
    NoIndiceSecundario* no = new NoIndiceSecundario;
    no->tamanho = 0;
    no->posicao = 0;

    for (int i = 0; i < CHAVES_POR_NO1; i++) {
        strcpy(no->chave[i], "");  // Inicializa as chaves como strings vazias
        no->ponteiro[i] = 0;
    }
    no->ponteiro[PONTEIROS_POR_NO1 - 1] = 0;  // Inicializa o último ponteiro

    return no;
}
int inserirNoArquivoIndice(NoIndiceSecundario* no) {
    /* Insere um nó no arquivo de índice secundário */
    cabecalhoIndiceSecundario->quantidadeNos++;
    arquivoIndiceSecundario->seekp(cabecalhoIndiceSecundario->quantidadeNos * sizeof(NoIndiceSecundario), ios::beg);
    arquivoIndiceSecundario->write(reinterpret_cast<char*>(no), sizeof(NoIndiceSecundario));
    atualizarCabecalhoIndiceSecundario();
    return cabecalhoIndiceSecundario->quantidadeNos;
}

void atualizarNoNoArquivo(NoIndiceSecundario* no) {
    /* Atualiza um nó no arquivo de índice secundário */
    arquivoIndiceSecundario->seekp(no->posicao * sizeof(NoIndiceSecundario), ios::beg);
    arquivoIndiceSecundario->write(reinterpret_cast<char*>(no), sizeof(NoIndiceSecundario));
}


void inserirChaveEmFolhaDisponivel(NoIndiceSecundario* no, char chave[300], int ponteiro) {
    /* Insere uma chave em uma folha que ainda tem espaço disponível */
    int i = 0;
    while (i < no->tamanho && strcmp(no->chave[i], chave) <= 0) {
        i++;
    }

    int j = no->tamanho;
    while (j > i) {
        strcpy(no->chave[j], no->chave[j - 1]);
        no->ponteiro[j] = no->ponteiro[j - 1];
        j--;
    }

    strcpy(no->chave[i], chave);
    no->ponteiro[i] = ponteiro;
    no->tamanho++;
    atualizarNoNoArquivo(no);
}


void inserirChaveEmNoDisponivel(NoIndiceSecundario* no, char chave[300], int ponteiro) {
    /* Insere uma chave em uma página que ainda tem espaço disponível */
    int i = 0;
    while (i < no->tamanho && strcmp(no->chave[i], chave) <= 0) {
        i++;
    }

    int j = no->tamanho;
    while (j > i) {
        strcpy(no->chave[j], no->chave[j - 1]);
        no->ponteiro[j + 1] = no->ponteiro[j];
        j--;
    }

    strcpy(no->chave[i], chave);
    no->ponteiro[i + 1] = -1 * ponteiro;
    no->tamanho++;
    atualizarNoNoArquivo(no);
}

NoAuxiliar* inserirChaveEmFolhaCheia(NoIndiceSecundario* no, char chave[300], int ponteiro) {
    /* Trata a inserção da chave quando a página de dados já está cheia */
    int chaveInserida = 0;
    char pivo[300];
    NoIndiceSecundario* novoNo = criarNoIndiceSecundario();
    NoAuxiliar* novoPai = new NoAuxiliar;

    strcpy(pivo, no->chave[ORDEM_ARVORE1]);

    // Caso a chave seja menor que o pivô, insere na folha à esquerda
    if (strcmp(chave, pivo) < 0) {
        int i = ORDEM_ARVORE1;
        int j = 0;
        
        // Move as chaves maiores para o novo nó
        while (i <= CHAVES_POR_NO1 - 1) {
            strcpy(novoNo->chave[j], no->chave[i]);
            novoNo->ponteiro[j] = no->ponteiro[i];
            novoNo->tamanho++;
            no->ponteiro[i] = 0;
            strcpy(no->chave[i], "\0");
            no->tamanho--;
            i++;
            j++;
        }
        
        // Insere a nova chave na posição correta na folha à esquerda
        i = ORDEM_ARVORE1 - 1;
        while (i >= 0 && strcmp(chave, no->chave[i]) < 0) {
            strcpy(no->chave[i + 1], no->chave[i]);
            no->ponteiro[i + 1] = no->ponteiro[i];
            i--;
        }
        
        strcpy(no->chave[i + 1], chave);
        no->ponteiro[i + 1] = ponteiro;
        no->tamanho++;
    } 
    else {  // Caso contrário, insere na folha à direita
        int i = ORDEM_ARVORE1;
        int j = 0;

        // Move as chaves maiores para o novo nó
        while (i <= CHAVES_POR_NO1 - 1) {
            if (chaveInserida == 0 && strcmp(chave, no->chave[i]) < 0) {
                strcpy(novoNo->chave[j], chave);
                novoNo->ponteiro[j] = ponteiro;
                novoNo->tamanho++;
                j++;
                chaveInserida = 1;
            }
            strcpy(novoNo->chave[j], no->chave[i]);
            novoNo->ponteiro[j] = no->ponteiro[i];
            novoNo->tamanho++;
            strcpy(no->chave[i], "\0");
            no->ponteiro[i] = 0;
            no->tamanho--;
            i++;
            j++;
        }
        
        // Caso a nova chave ainda não tenha sido adicionada
        if (chaveInserida == 0) {
            strcpy(novoNo->chave[j], chave);
            novoNo->ponteiro[j] = ponteiro;
            novoNo->tamanho++;
        }
    }

    // Ajusta ponteiros entre o nó atual e o novo nó
    novoNo->ponteiro[PONTEIROS_POR_NO1 - 1] = no->ponteiro[PONTEIROS_POR_NO1 - 1];
    novoNo->posicao = inserirNoArquivoIndice(novoNo);
    no->ponteiro[PONTEIROS_POR_NO1 - 1] = -1 * novoNo->posicao;

    atualizarNoNoArquivo(no);
    atualizarNoNoArquivo(novoNo);

    // Configura o novo nó pai com o pivô e os ponteiros para os nós divididos
    strcpy(novoPai->chave, pivo);
    novoPai->ponteiroEsquerdo = -1 * no->posicao;
    novoPai->ponteiroDireito = -1 * novoNo->posicao;

    delete novoNo;
    return novoPai;
}

NoAuxiliar* inserirChaveEmNoCheio(NoIndiceSecundario* no, char chave[300], int ponteiro) {
    /* Trata a inserção da chave quando a página já está cheia */
    int chaveInserida = 0;
    char pivo[300];
    NoIndiceSecundario* novoNo = criarNoIndiceSecundario();
    NoAuxiliar* novoPai = new NoAuxiliar;

    strcpy(pivo, no->chave[ORDEM_ARVORE1]);
    strcpy(no->chave[ORDEM_ARVORE1], "\0");
    no->tamanho--;

    // Caso a chave seja menor que o pivô, insere à esquerda
    if (strcmp(chave, pivo) < 0) {
        int i = ORDEM_ARVORE1 + 1;
        int j = 0;

        novoNo->ponteiro[0] = no->ponteiro[ORDEM_ARVORE1 + 1];

        // Move as chaves maiores para o novo nó
        while (i <= CHAVES_POR_NO1 - 1) {
            strcpy(novoNo->chave[j], no->chave[i]);
            novoNo->ponteiro[j + 1] = no->ponteiro[i + 1];
            novoNo->tamanho++;
            strcpy(no->chave[i], "\0");
            no->ponteiro[i + 1] = 0;
            no->tamanho--;
            i++;
            j++;
        }

        // Insere a nova chave na posição correta no nó original
        i = ORDEM_ARVORE1;
        while (i > 0 && strcmp(chave, no->chave[i]) < 0) {
            strcpy(no->chave[i], no->chave[i - 1]);
            no->ponteiro[i + 1] = no->ponteiro[i];
            i--;
        }

        strcpy(no->chave[i], chave);
        no->ponteiro[i + 1] = -1 * ponteiro;
        no->tamanho++;
    } 
    else {  // Caso contrário, insere à direita
        int i = ORDEM_ARVORE1 + 1;
        int j = 0;

        novoNo->ponteiro[0] = no->ponteiro[ORDEM_ARVORE1 + 1];

        // Move as chaves maiores para o novo nó, inserindo a nova chave no local correto
        while (i <= CHAVES_POR_NO1 - 1) {
            if (!chaveInserida && strcmp(chave, no->chave[i]) < 0) {
                strcpy(novoNo->chave[j], chave);
                novoNo->ponteiro[j + 1] = -1 * ponteiro;
                novoNo->tamanho++;
                j++;
                chaveInserida = 1;
            }
            strcpy(novoNo->chave[j], no->chave[i]);
            novoNo->ponteiro[j + 1] = no->ponteiro[i + 1];
            novoNo->tamanho++;
            no->ponteiro[i + 1] = 0;
            strcpy(no->chave[i], "\0");
            no->tamanho--;
            i++;
            j++;
        }

        // Caso a chave ainda não tenha sido inserida, insere-a no final
        if (!chaveInserida) {
            strcpy(novoNo->chave[j], chave);
            novoNo->ponteiro[j + 1] = -1 * ponteiro;
            novoNo->tamanho++;
        }
    }

    // Atualizações de posições e ponteiros para o novo nó e o nó pai
    novoNo->posicao = inserirNoArquivoIndice(novoNo);
    atualizarNoNoArquivo(novoNo);
    atualizarNoNoArquivo(no);

    // Configura o novo nó pai com o pivô e os ponteiros para os nós divididos
    strcpy(novoPai->chave, pivo);
    novoPai->ponteiroEsquerdo = -1 * no->posicao;
    novoPai->ponteiroDireito = -1 * novoNo->posicao;

    delete novoNo;
    return novoPai;
}

// NoIndiceSecundario* obterNoDoArquivoIndice(int posicao) {
//     /* Retorna um nó do arquivo de índice de acordo com a posição informada */
//     if (posicao > 0) {
//         cout << "Endereço inválido" << endl;
//         return nullptr;
//     }

//     NoIndiceSecundario* no = criarNoIndiceSecundario();
//     arquivoIndiceSecundario->seekg(-1 * posicao * static_cast<int>(sizeof(NoIndiceSecundario)), ios::beg);
//     arquivoIndiceSecundario->read(reinterpret_cast<char*>(no), sizeof(NoIndiceSecundario));

//     return no;
// }

NoIndiceSecundario* obterNoDoArquivoIndice(int posicao) {
    if (posicao >= 0) {
        std::cerr << "Posição inválida para o índice secundário: " << posicao << std::endl;
        return nullptr;
    }

    NoIndiceSecundario* no = criarNoIndiceSecundario();
    if (!no) {
        std::cerr << "Erro ao alocar memória para o nó." << std::endl;
        return nullptr;
    }

    arquivoIndiceSecundario->seekg(-1 * posicao * static_cast<int>(sizeof(NoIndiceSecundario)), ios::beg);
    if (arquivoIndiceSecundario->fail()) {
        std::cerr << "Erro ao posicionar cursor no arquivo de índice." << std::endl;
        delete no;
        return nullptr;
    }

    arquivoIndiceSecundario->read(reinterpret_cast<char*>(no), sizeof(NoIndiceSecundario));
    if (arquivoIndiceSecundario->fail()) {
        std::cerr << "Erro ao ler nó do arquivo de índice." << std::endl;
        delete no;
        return nullptr;
    }

    return no;
}

NoAuxiliar* adicionarChaveNaArvore(NoIndiceSecundario* no, char chave[300], int ponteiro) {
    /* Função de inserção da chave na árvore */
    NoAuxiliar* indice = nullptr;

    // Caso seja uma página de índice
    if (no->ponteiro[0] < 0) {
        int posicao;
        int posMinima = 0;
        int posMaxima = no->tamanho - 1;

        // Encontra a posição adequada do apontador
        while (posMinima <= posMaxima) {
            posicao = (posMinima + posMaxima) / 2;
            (strcmp(chave, no->chave[posicao]) < 0) ? posMaxima = posicao - 1 : posMinima = posicao + 1;
        }

        // Define o apontador na posição correta
        if (posMaxima < 0) posMaxima = 0;
        else if (strcmp(chave, no->chave[posMaxima]) >= 0) posMaxima++;

        posicao = posMaxima;
        indice = adicionarChaveNaArvore(obterNoDoArquivoIndice(no->ponteiro[posicao]), chave, ponteiro);

        if (indice != nullptr) {
            // Se a página ainda tiver espaço
            if (strcmp(no->chave[CHAVES_POR_NO1 - 1], "\0") == 0) {
                inserirChaveEmNoDisponivel(no, indice->chave, -1 * indice->ponteiroDireito);
                delete indice;
                indice = nullptr;
            }
            // Se a página estiver cheia
            else {
                NoAuxiliar* novoRetorno = inserirChaveEmNoCheio(no, indice->chave, -1 * indice->ponteiroDireito);
                delete indice;
                indice = novoRetorno;
            }
        }
    }
    // Caso seja uma página de dados (folha)
    else {
        // Se a página ainda tiver espaço
        if (strcmp(no->chave[CHAVES_POR_NO1 - 1], "\0") == 0) {
            inserirChaveEmFolhaDisponivel(no, chave, ponteiro);
        }
        // Se a página estiver cheia
        else {
            indice = inserirChaveEmFolhaCheia(no, chave, ponteiro);
        }
    }
    delete no;
    return indice;
}


// void popularArquivo() {
//     /* Percorre todos os blocos do arquivo de dados e 
//     insere o título de cada registro na árvore */
//     Bloco buffer = {0};
//     int posicao;

//     // Posiciona o cursor no início do arquivo de dados
//     arquivoDados->clear();
//     arquivoDados->seekg(0, ios::beg);

//     int i = 0;
//     // Insere o título de todos os registros do arquivo de dados na árvore
//     while (i < NUM_BUCKETS) {
//         // Copia o bucket para o buffer
//         arquivoDados->read(reinterpret_cast<char*>(&buffer), sizeof(Bloco));

//         // Se o bucket não for vazio
//         if (buffer.numRegistros > 0) {
//             Artigo* v_artigo = buffer.artigos.data(); // Usa data() para acessar o array
            
//             int j = 0;
//             // Percorre os registros do bloco
//             while (j < buffer.numRegistros) {
//                 // Insere o título (chave) do registro na árvore
//                 NoAuxiliar* pagina = adicionarChaveNaArvore(obterNoDoArquivoIndice(cabecalhoIndiceSecundario->posicaoRaiz), v_artigo[j].titulo, i);

//                 if (pagina != nullptr) {
//                     // Aloca um novo nó
//                     NoIndiceSecundario* novoNo = criarNoIndiceSecundario();

//                     // O novo nó recebe as informações do nó criado na árvore
//                     strcpy(novoNo->chave[0], pagina->chave);
//                     novoNo->ponteiro[0] = pagina->ponteiroEsquerdo;
//                     novoNo->ponteiro[1] = pagina->ponteiroDireito;
//                     novoNo->tamanho++;

//                     // Insere o nó no arquivo
//                     int posicao = inserirNoArquivoIndice(novoNo);
//                     novoNo->posicao = posicao;
//                     atualizarNoNoArquivo(novoNo);

//                     // Atualiza a posição da raiz
//                     cabecalhoIndiceSecundario->posicaoRaiz = -1 * posicao;
//                     atualizarCabecalhoIndiceSecundario();

//                     delete novoNo;
//                     delete pagina;
//                 }
//                 j++;
//             }
//         }
//         i++;
//     }
// }

void popularArquivo() {
    Bloco buffer = {0};

    // Posiciona o cursor no início do arquivo de dados
    arquivoDados->clear();
    arquivoDados->seekg(0, ios::beg);

    for (int i = 0; i < NUM_BUCKETS; ++i) {
        arquivoDados->read(reinterpret_cast<char*>(&buffer), sizeof(Bloco));
        if (arquivoDados->fail()) {
            std::cerr << "Erro ao ler bloco do arquivo de dados." << std::endl;
            break;
        }

        if (buffer.numRegistros > 0) {
            Artigo* artigos = buffer.artigos.data();

            for (int j = 0; j < buffer.numRegistros; ++j) {
                NoAuxiliar* pagina = adicionarChaveNaArvore(
                    obterNoDoArquivoIndice(cabecalhoIndiceSecundario->posicaoRaiz), artigos[j].titulo, i
                );

                if (pagina) {
                    NoIndiceSecundario* novoNo = criarNoIndiceSecundario();
                    if (!novoNo) {
                        std::cerr << "Erro ao criar novo nó para o índice secundário." << std::endl;
                        delete pagina;
                        break;
                    }

                    strcpy(novoNo->chave[0], pagina->chave);
                    novoNo->ponteiro[0] = pagina->ponteiroEsquerdo;
                    novoNo->ponteiro[1] = pagina->ponteiroDireito;
                    novoNo->tamanho++;

                    int posicao = inserirNoArquivoIndice(novoNo);
                    novoNo->posicao = posicao;
                    atualizarNoNoArquivo(novoNo);
                    cabecalhoIndiceSecundario->posicaoRaiz = -1 * posicao;
                    atualizarCabecalhoIndiceSecundario();

                    delete novoNo;
                    delete pagina;
                }
            }
        }
    }
}

// void inserirNoArquivoIndiceSecundario(fstream* arquivoDados, fstream* arquivoIndiceSecundario) {
//     /* Faz upload dos dados no arquivo de índice secundário */
//     arquivoDados = arquivoDados;
//     arquivoIndiceSecundario = arquivoIndiceSecundario;

//     // Inicializa o cabeçalho no arquivo
//     inicializarCabecalho(); 

//     // Aloca o nó raiz
//     NoIndiceSecundario* raiz = criarNoIndiceSecundario();

//     // Escreve o nó raiz no arquivo
//     raiz->posicao = inserirNoArquivoIndice(raiz);

//     // Atualiza a posição do nó raiz no arquivo
//     atualizarNoNoArquivo(raiz);

//     // Libera a memória do nó raiz
//     delete raiz;

//     // Popula o arquivo de índices
//     popularArquivo();
// }
void inserirNoArquivoIndiceSecundario(fstream* arqDados, fstream* arqIndiceSecundario) {
    if (!arqDados || !arqIndiceSecundario) {
        std::cerr << "Erro: Arquivos de dados ou índice secundário não foram abertos." << std::endl;
        exit(EXIT_FAILURE);
    }
    
    arquivoDados = arqDados;
    arquivoIndiceSecundario = arqIndiceSecundario;

    // Inicializa o cabeçalho
    inicializarCabecalho();

    // Aloca e insere o nó raiz no arquivo
    NoIndiceSecundario* raiz = criarNoIndiceSecundario();
    if (!raiz) {
        std::cerr << "Erro ao criar nó raiz." << std::endl;
        exit(EXIT_FAILURE);
    }

    raiz->posicao = inserirNoArquivoIndice(raiz);
    atualizarNoNoArquivo(raiz);
    delete raiz;

    // Popula o arquivo de índices
    popularArquivo();
}
