#include "primeiroIndice.hpp"

// Variáveis globais para manipulação dos arquivos
CabecalhoIndicePrimario* cabecalhoBuscaIndicePrimario = nullptr;
fstream* arquivoBuscaIndicePrimario;
fstream* arquivoBuscaHash;

// Obtém o cabeçalho do arquivo de índice primário
void obterCabecalhoArquivoIndicePrimario() {
    // Aloca espaço para a estrutura de cabeçalho, se ainda não existir
    if (cabecalhoBuscaIndicePrimario == nullptr) {
        cabecalhoBuscaIndicePrimario = new CabecalhoIndicePrimario();
    }

    // Posiciona o cursor no início do arquivo e lê o cabeçalho
    arquivoBuscaIndicePrimario->seekg(0, ios::beg);
    arquivoBuscaIndicePrimario->read(reinterpret_cast<char*>(cabecalhoBuscaIndicePrimario), sizeof(CabecalhoIndicePrimario));

    if (arquivoBuscaIndicePrimario->fail()) {
        cout << "Erro ao ler o cabeçalho do arquivo de índice primário." << endl;
        exit(EXIT_FAILURE);
    }
}

// Abre o arquivo de índice primário
void abrirArquivoIndicePrimario(const char* caminhoArquivoIndice) {
    // Abre o arquivo de índice primário para leitura e escrita em modo binário
    arquivoBuscaIndicePrimario = new fstream(caminhoArquivoIndice, fstream::in | fstream::out | ios::binary);
    
    if (!arquivoBuscaIndicePrimario->is_open()) {
        cout << "O arquivo de índice primário não pôde ser aberto. Cancelando as operações." << endl;
        delete arquivoBuscaIndicePrimario;
        exit(EXIT_FAILURE);
    }
}

// Abre o arquivo de dados (hash)
void abrirArquivoHash(const char* caminhoArquivoDados) {
    // Abre o arquivo de dados em modo binário para leitura
    arquivoBuscaHash = new fstream(caminhoArquivoDados, fstream::in | ios::binary);
    
    if (!arquivoBuscaHash->is_open()) {
        cout << "O arquivo de dados não pôde ser aberto. Cancelando as operações." << endl;
        delete arquivoBuscaHash;
        exit(EXIT_FAILURE);
    }
}

// Função para fechar os arquivos abertos
void fecharArquivos() {
    if (arquivoBuscaIndicePrimario) {
        arquivoBuscaIndicePrimario->close();
        delete arquivoBuscaIndicePrimario;
        arquivoBuscaIndicePrimario = nullptr;
    }

    if (arquivoBuscaHash) {
        arquivoBuscaHash->close();
        delete arquivoBuscaHash;
        arquivoBuscaHash = nullptr;
    }

    if (cabecalhoBuscaIndicePrimario) {
        delete cabecalhoBuscaIndicePrimario;
        cabecalhoBuscaIndicePrimario = nullptr;
    }
}

// // Fecha os arquivos abertos e libera a memória alocada
// void fecharArquivos() {
//     // Fecha os arquivos abertos, se existirem
//     if (arquivoBuscaIndicePrimario) {
//         arquivoBuscaIndicePrimario->close();
//         delete arquivoBuscaIndicePrimario;
//         arquivoBuscaIndicePrimario = nullptr;
//     }

//     if (arquivoBuscaHash) {
//         arquivoBuscaHash->close();
//         delete arquivoBuscaHash;
//         arquivoBuscaHash = nullptr;
//     }

//     // Libera a memória alocada para o cabeçalho do índice primário
//     if (cabecalhoBuscaIndicePrimario) {
//         delete cabecalhoBuscaIndicePrimario;
//         cabecalhoBuscaIndicePrimario = nullptr;
//     }
// }

// Cria um novo nó de dados, alocando memória e inicializando seus atributos
DadosNo* criarNoDeDadosBusca() {
    DadosNo* no = new DadosNo();

    // Inicializa os atributos do nó
    no->tamanhoNo = 0;
    no->posicaoNo = 0;

    // Inicializa chaves e ponteiros
    for (int i = 0; i < CHAVES_POR_NO; i++) {
        no->chave[i] = 0;
        no->ponteiro[i] = 0;
    }

    // Inicializa a última posição do vetor de ponteiros
    no->ponteiro[PONTEIROS_POR_NO] = 0;
    return no;
}

// Obtém um nó de dados do arquivo de índice primário com base na posição fornecida
DadosNo* obterNoDeDadosBusca(int posicaoNo) {
    if (posicaoNo > 0) {
        cout << "Endereço inválido" << endl;
        return nullptr;
    }

    DadosNo* no = criarNoDeDadosBusca();

    // Posiciona o cursor na posição informada e lê o nó
    arquivoBuscaIndicePrimario->seekg(-1 * posicaoNo * static_cast<int>(sizeof(DadosNo)), ios::beg);
    arquivoBuscaIndicePrimario->read(reinterpret_cast<char*>(no), sizeof(DadosNo));

    return no;
}

// Busca uma chave no arquivo de índice primário
int buscarChaveNoArquivoIndicePrimario(const char* caminhoArquivoDados, const char* caminhoArquivoIndice, int chave) {
    int ponteiro, posicao, menor, maior, posicaoPonteiro, numBlocosLidos = 0;
    DadosNo *noAtual;

    abrirArquivoIndicePrimario(caminhoArquivoIndice);
    abrirArquivoHash(caminhoArquivoDados);
    obterCabecalhoArquivoIndicePrimario();

    numBlocosLidos++;  // Incrementa o contador de leitura de blocos
    posicao = cabecalhoBuscaIndicePrimario->posicaoRaiz;  // Posição inicial na raiz

    // Busca iterativa até encontrar uma folha
    while (true) {
        noAtual = obterNoDeDadosBusca(posicao);
        numBlocosLidos++;

        // Verificação do carregamento do nó
        if (noAtual == nullptr) {
            cout << "Erro: nó não pôde ser carregado na função buscarChaveNoArquivoIndicePrimario" << endl;
            fecharArquivos();
            return -1;
        }

        // Verifica se o nó é uma página de índice
        if (noAtual->ponteiro[0] < 0) {
            menor = 0;
            maior = noAtual->tamanhoNo - 1;

            // Busca binária na página de índice
            while (menor <= maior) {
                posicaoPonteiro = (menor + maior) / 2;
                menor = (chave >= noAtual->chave[posicaoPonteiro]) ? posicaoPonteiro + 1 : menor;
                maior = (chave < noAtual->chave[posicaoPonteiro]) ? posicaoPonteiro - 1 : maior;
            }

            // Determina a posição do ponteiro para a próxima página
            posicaoPonteiro = (maior < 0) ? 0 : (chave >= noAtual->chave[maior] ? maior + 1 : maior);
            posicao = noAtual->ponteiro[posicaoPonteiro];
        } else {
            break;  // Nó é uma folha de dados
        }
    }

    cout << "Quantidade de blocos lidos: " << numBlocosLidos << endl;

    // Busca binária na folha de dados para localizar a chave
    menor = 0;
    maior = noAtual->tamanhoNo - 1;
    while (menor <= maior) {
        posicaoPonteiro = (menor + maior) / 2;
        if (chave == noAtual->chave[posicaoPonteiro]) {
            fecharArquivos();
            return noAtual->ponteiro[posicaoPonteiro];
        }
        menor = (chave > noAtual->chave[posicaoPonteiro]) ? posicaoPonteiro + 1 : menor;
        maior = (chave < noAtual->chave[posicaoPonteiro]) ? posicaoPonteiro - 1 : maior;
    }

    fecharArquivos();
    return -1;  // Chave não encontrada
}

// Lê um bloco do arquivo de dados
Bloco* lerBloco(fstream* arq, int posicaoNo) {
    Bloco* buffer = new Bloco;

    // Move o cursor para a posição do bloco no arquivo
    arq->seekg(posicaoNo * static_cast<int>(sizeof(Bloco)), ios::beg);

    // Leitura do bloco
    arq->read(reinterpret_cast<char*>(buffer), sizeof(Bloco));
    return buffer;
}

// Copia os dados de um artigo de origem para um artigo de destino
void copiarDadosArtigo(Artigo* destino, const Artigo* origem) {
    destino->id = origem->id;
    strcpy(destino->titulo, origem->titulo);
    destino->ano = origem->ano;
    strcpy(destino->autores, origem->autores);
    destino->citacoes = origem->citacoes;
    strcpy(destino->dataAtualizacao, origem->dataAtualizacao);
    strcpy(destino->resumo, origem->resumo);
}

// Obtém um artigo com base na posição do nó e no ID fornecido
// Obtém um artigo com base na posição do nó e no ID fornecido
Artigo* obterArtigoPorPosicaoENo(fstream* arquivoEntrada, int posicaoNo, int id) {
    Bloco* bloco = lerBloco(arquivoEntrada, posicaoNo);
    Artigo* resultado = new Artigo;

    int menor = 0;
    int maior = bloco->numRegistros - 1;

    // Realiza uma busca binária no bloco pelo ID
    for (; menor <= maior;) {
        int posicaoPonteiro = (menor + maior) / 2;
        
        if (id == bloco->artigos[posicaoPonteiro].id) {
            copiarDadosArtigo(resultado, &bloco->artigos[posicaoPonteiro]);
            break;
        }

        (id < bloco->artigos[posicaoPonteiro].id) ? maior = posicaoPonteiro - 1 : menor = posicaoPonteiro + 1;
    }

    delete bloco;
    return resultado;
}


// Busca uma chave por ID no arquivo de índice primário e imprime seus dados
void buscarPorID(const char* caminhoArquivoDados, const char* caminhoArquivoIndice, int chave) {
    int posicaoNo = 0;
    Artigo* artigo;

    posicaoNo = buscarChaveNoArquivoIndicePrimario(caminhoArquivoDados, caminhoArquivoIndice, chave);

    // Verifica se a chave não foi encontrada
    if (posicaoNo < 0) {
        cout << "Registro com o ID " << chave << " não encontrado" << endl;
        return;
    }

    // Se a chave foi encontrada, busca-a no arquivo de dados e imprime
    abrirArquivoHash(caminhoArquivoDados);
    artigo = obterArtigoPorPosicaoENo(arquivoBuscaHash, posicaoNo, chave);

    if (artigo != nullptr) {
        imprimirArtigo(*artigo);
        delete artigo;  // Libera a memória alocada para o artigo
    }

    arquivoBuscaHash->close();
    delete arquivoBuscaHash;
    arquivoBuscaHash = nullptr;
}
void imprimirArtigo(const Artigo& artigo) {
    cout << "Id: " << artigo.id << endl;
    cout << "Título: " << artigo.titulo << endl;
    cout << "Ano: " << artigo.ano << endl;
    cout << "Autores: " << artigo.autores << endl;
    cout << "Citações: " << artigo.citacoes << endl;
    cout << "Data de Atualização: " << artigo.dataAtualizacao << endl;
    cout << "Resumo: " << artigo.resumo << endl;
}

// Função principal
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " <ID>" << endl;
        return 1;
    }

    buscarPorID("dataHash", "indicePrimario", atoi(argv[1]));
    return 0;
}

