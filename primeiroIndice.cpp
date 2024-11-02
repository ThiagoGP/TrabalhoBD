#include "primeiroIndice.hpp"

// Variáveis globais para manipulação dos arquivos
CabecalhoIndicePrimario* cabecalhoIndicePrimario = nullptr;
fstream* arquivoIndicePrimario;
fstream* arquivoHashEntrada;

// Atualiza o cabeçalho no arquivo de índice primário
void atualizarCabecalhoIndicePrimario() {
	// Move o cursor para o início do arquivo de índice primário
	arquivoIndicePrimario->seekp(0, arquivoIndicePrimario->beg);
	// Escreve o cabeçalho no arquivo de índice primário
	arquivoIndicePrimario->write(reinterpret_cast<char*>(cabecalhoIndicePrimario), sizeof(CabecalhoIndicePrimario));
}

// Inicializa o cabeçalho do arquivo de índice primário
void inicializarCabecalhoIndicePrimario() {
	cabecalhoIndicePrimario = new CabecalhoIndicePrimario;
	cabecalhoIndicePrimario->posicaoRaiz = -1;
	cabecalhoIndicePrimario->numeroDeNos = 0;

	// Atualiza o cabeçalho no arquivo
	atualizarCabecalhoIndicePrimario();
}

// Cria um novo nó de dados
DadosNo* criarNoDeDados() {
	DadosNo* no = new DadosNo;
	no->tamanhoNo = 0;
	no->posicaoNo = 0;

	// Inicializa as chaves e os ponteiros
	for (int i = 0; i < CHAVES_POR_NO; i++) {
		no->chave[i] = 0;
		no->ponteiro[i] = 0;
	}
	// Inicializa a última posição do vetor de ponteiros
	no->ponteiro[PONTEIROS_POR_NO] = 0;
	return no;
}

// Insere um nó de dados no arquivo de índice primário
int inserirNoDeDadosNaPosicao(DadosNo* bloco) {
	// Incrementa a quantidade de nós
	cabecalhoIndicePrimario->numeroDeNos++;

	// Move o cursor para a posição de inserção
	arquivoIndicePrimario->seekp(cabecalhoIndicePrimario->numeroDeNos * sizeof(DadosNo), arquivoIndicePrimario->beg);
	
	// Escreve o nó no arquivo de índice primário
	arquivoIndicePrimario->write(reinterpret_cast<char*>(bloco), sizeof(DadosNo));

	// Atualiza o cabeçalho com a nova quantidade de nós	
	atualizarCabecalhoIndicePrimario();
	return cabecalhoIndicePrimario->numeroDeNos;
}

// Atualiza a posição de um nó no arquivo de índice primário
void atualizarPosicaoNoDeDados(DadosNo* bloco) {
	// Move o cursor para a posição especificada
	arquivoIndicePrimario->seekp(bloco->posicaoNo * sizeof(DadosNo), arquivoIndicePrimario->beg);
	// Escreve o nó atualizado no arquivo
	arquivoIndicePrimario->write(reinterpret_cast<char*>(bloco), sizeof(DadosNo));
}

// Insere uma chave em uma folha que ainda contém espaço disponível
void inserirChaveEmFolhaDisponivel(DadosNo* no, int chave, int ponteiro) {
    int i, j;
    // Percorre as chaves existentes no nó
    for (i = 0; i < no->tamanhoNo; i++) {
        // Caso a chave existente na posição i seja maior que a chave atual
        if (no->chave[i] > chave) {
            // Realiza o deslocamento das chaves para abrir espaço para a nova
            for (j = no->tamanhoNo; j > i; j--) {
                no->chave[j] = no->chave[j - 1];
                no->ponteiro[j] = no->ponteiro[j - 1];
            }
            break;
        }
    }
    // Insere a chave no nó correspondente
    no->chave[i] = chave;
    no->ponteiro[i] = ponteiro;
    no->tamanhoNo++;

    // Atualiza o nó no arquivo de índice primário
    atualizarPosicaoNoDeDados(no);
}

// Insere uma chave em um nó de índice que possui espaço disponível
void inserirChaveEmNoDisponivel(DadosNo* no, int chave, int ponteiro) {
    int i, j;
    // Percorre as chaves existentes no nó
    for (i = 0; i < no->tamanhoNo; i++) {
        // Se a chave existente na posição i for maior que a chave atual
        if (no->chave[i] > chave) {
            // Desloca as chaves e ponteiros existentes para abrir espaço para a nova chave
            for (j = no->tamanhoNo; j > i; j--) {
                no->chave[j] = no->chave[j - 1];
                no->ponteiro[j + 1] = no->ponteiro[j];
            }
            break;
        }
    }
    // Insere a nova chave no nó
    no->chave[i] = chave;
    no->ponteiro[i + 1] = -ponteiro;
    no->tamanhoNo++;

    // Atualiza o nó no arquivo de índice primário
    atualizarPosicaoNoDeDados(no);
}

// Caso a inserção de dados na folha já esteja cheia, esta função realiza o tratamento adequado
ConexaoNo* inserirChaveEmFolhaCheia(DadosNo* no, int chave, int ponteiro) {
    int i, j, pivo, inserido = 0;
    DadosNo* novoNoDeDados = nullptr;
    ConexaoNo* noPai = nullptr;

    // Aloca memória para um novo nó de dados e para um nó de conexão
    novoNoDeDados = criarNoDeDados();
    noPai = new ConexaoNo;

    // Salva o valor do pivô e ajusta o nó atual
    pivo = no->chave[ORDEM_ARVORE];
    no->chave[ORDEM_ARVORE] = 0;
    no->tamanhoNo--;

    // Verifica se a chave deve ser inserida no lado esquerdo ou direito do pivô
    if (chave < pivo) {
        j = 0;
        novoNoDeDados->ponteiro[0] = no->ponteiro[ORDEM_ARVORE + 1];
        // Move as chaves maiores para o novo nó de dados
        for (i = ORDEM_ARVORE + 1; i <= CHAVES_POR_NO; i++) {
            novoNoDeDados->chave[j] = no->chave[i];
            novoNoDeDados->ponteiro[j + 1] = no->ponteiro[i + 1];
            novoNoDeDados->tamanhoNo++;
            no->chave[i] = 0;
            no->ponteiro[i + 1] = 0;
            no->tamanhoNo--;
            j++;
        }
        // Move as chaves menores para abrir espaço para a nova chave
        for (i = ORDEM_ARVORE; i > 0; i--) {
            if (chave > no->chave[i]) {
                break;
            } else {
                no->chave[i] = no->chave[i - 1];
                no->ponteiro[i + 1] = no->ponteiro[i];
            }
        }
        // Insere a nova chave
        no->chave[i] = chave;
        no->ponteiro[i + 1] = -ponteiro;
        no->tamanhoNo++;

    } else {
        j = 0;
        novoNoDeDados->ponteiro[0] = no->ponteiro[ORDEM_ARVORE + 1];
        // Move as chaves maiores que a nova chave para o novo nó de dados
        for (i = ORDEM_ARVORE + 1; i <= CHAVES_POR_NO; i++) {
            if (inserido == 0 && chave < no->chave[i]) {
                novoNoDeDados->chave[j] = chave;
                novoNoDeDados->ponteiro[j + 1] = -ponteiro;
                novoNoDeDados->tamanhoNo++;
                j++;
                inserido = 1;
            }
            novoNoDeDados->chave[j] = no->chave[i];
            novoNoDeDados->ponteiro[j + 1] = no->ponteiro[i + 1];
            novoNoDeDados->tamanhoNo++;
            no->chave[i] = 0;
            no->ponteiro[i + 1] = 0;
            no->tamanhoNo--;
            j++;
        }
        // Se a nova chave ainda não foi inserida, insere-a agora
        if (inserido == 0) {
            novoNoDeDados->chave[j] = chave;
            novoNoDeDados->ponteiro[j + 1] = -ponteiro;
            novoNoDeDados->tamanhoNo++;
        }
    }
    
    // Insere o novo nó de dados na posição correta e atualiza as posições dos nós
    novoNoDeDados->posicaoNo = inserirNoDeDadosNaPosicao(novoNoDeDados);
    atualizarPosicaoNoDeDados(novoNoDeDados);
    atualizarPosicaoNoDeDados(no);

    // Preenche o nó de conexão com os valores corretos
    noPai->chave = pivo;
    noPai->ponteiroEsquerdo = -no->posicaoNo;
    noPai->ponteiroDireito = -novoNoDeDados->posicaoNo;

    // Libera a memória alocada para o novo nó de dados e retorna o nó de conexão
    delete novoNoDeDados;
    return noPai;
}

// Caso a inserção no nó esteja inviável (página cheia), esta função realiza o tratamento necessário
ConexaoNo* inserirChaveEmNoCheio(DadosNo* no, int chave, int ponteiro) {
    int i, j, pivo, inserido = 0;
    DadosNo* novoNoDeDados = nullptr;
    ConexaoNo* noPai = nullptr;

    // Cria um novo nó de dados e um novo nó de conexão
    novoNoDeDados = criarNoDeDados();
    noPai = new ConexaoNo;

    // Salva o valor do pivô
    pivo = no->chave[ORDEM_ARVORE];

    // Verifica se a chave deve ser inserida à esquerda ou à direita do pivô
    if (chave < pivo) {
        j = 0;
        // Move as chaves maiores para o novo nó de dados
        i = ORDEM_ARVORE;
        while (i <= CHAVES_POR_NO) {
            novoNoDeDados->chave[j] = no->chave[i];
            novoNoDeDados->ponteiro[j] = no->ponteiro[i];
            novoNoDeDados->tamanhoNo++;
            no->chave[i] = 0;
            no->ponteiro[i] = 0;
            no->tamanhoNo--;
            i++;
            j++;
        }
        // Move as chaves menores para abrir espaço para a nova chave
        i = ORDEM_ARVORE - 1;
        while (i >= 0) {
            if (chave > no->chave[i]) {
                break;
            } else {
                no->chave[i + 1] = no->chave[i];
                no->ponteiro[i + 1] = no->ponteiro[i];
            }
            i--;
        }
        no->chave[i + 1] = chave;
        no->ponteiro[i + 1] = ponteiro;
        no->tamanhoNo++;
    } else {
        j = 0;
        // Move as chaves maiores que a nova chave para o novo nó de dados
        i = ORDEM_ARVORE;
        while (i <= CHAVES_POR_NO) {
            if (inserido == 0 && chave < no->chave[i]) {
                novoNoDeDados->chave[j] = chave;
                novoNoDeDados->ponteiro[j] = ponteiro;
                novoNoDeDados->tamanhoNo++;
                j++;
                inserido = 1;
            }
            novoNoDeDados->chave[j] = no->chave[i];
            novoNoDeDados->ponteiro[j] = no->ponteiro[i];
            novoNoDeDados->tamanhoNo++;
            no->chave[i] = 0;
            no->ponteiro[i] = 0;
            no->tamanhoNo--;
            i++;
            j++;
        }
        // Se a nova chave ainda não foi inserida, insere-a agora
        if (inserido == 0) {
            novoNoDeDados->chave[j] = chave;
            novoNoDeDados->ponteiro[j] = ponteiro;
            novoNoDeDados->tamanhoNo++;
        }
    }

    // Atualiza o ponteiro para o novo nó de dados e as posições dos nós
    novoNoDeDados->ponteiro[PONTEIROS_POR_NO] = no->ponteiro[PONTEIROS_POR_NO];
    novoNoDeDados->posicaoNo = inserirNoDeDadosNaPosicao(novoNoDeDados);
    no->ponteiro[PONTEIROS_POR_NO] = -novoNoDeDados->posicaoNo;
    atualizarPosicaoNoDeDados(no);
    atualizarPosicaoNoDeDados(novoNoDeDados);
    
    // Preenche o nó de conexão com os valores corretos
    noPai->chave = pivo;
    noPai->ponteiroEsquerdo = -no->posicaoNo;
    noPai->ponteiroDireito = -novoNoDeDados->posicaoNo;
    
    // Libera a memória alocada para o novo nó de dados e retorna o nó de conexão
    delete novoNoDeDados;
    return noPai;
}

// Retorna um nó do arquivo de índice primário conforme a posição solicitada
DadosNo* obterNoDeDadosDoArquivoIndicePrimario(int posicaoNo) {
    if (posicaoNo > 0) {
        cout << "Endereço inválido" << endl;
        return nullptr;
    }

    DadosNo* no = criarNoDeDados();
    // Posiciona o cursor na posição informada
    arquivoIndicePrimario->seekg(-posicaoNo * static_cast<int>(sizeof(DadosNo)), arquivoIndicePrimario->beg);

    // Lê e retorna o nó solicitado
    arquivoIndicePrimario->read(reinterpret_cast<char*>(no), sizeof(DadosNo));
    return no;
}

// Função que insere uma chave na árvore
ConexaoNo* inserirChaveNaArvore(DadosNo* no, int chave, int ponteiro) {
    int posicaoPonteiro;
    ConexaoNo* conexaoNo = nullptr;

    // Verifica se o nó é uma página de índice
    if (no->ponteiro[0] < 0) {
        int menor = 0;
        int maior = no->tamanhoNo - 1;

        // Encontra a posição correta do ponteiro
        while (menor <= maior) {
            posicaoPonteiro = (menor + maior) / 2;
            if (chave < no->chave[posicaoPonteiro]) {
                maior = posicaoPonteiro - 1;
            } else {
                menor = posicaoPonteiro + 1;
            }
        }

        posicaoPonteiro = (maior < 0) ? 0 : (chave >= no->chave[maior] ? maior + 1 : maior);

        // Insere a chave recursivamente na árvore
        conexaoNo = inserirChaveNaArvore(obterNoDeDadosDoArquivoIndicePrimario(no->ponteiro[posicaoPonteiro]), chave, ponteiro);

        if (conexaoNo != nullptr) {
            if (no->chave[CHAVES_POR_NO] == 0) {
                inserirChaveEmNoDisponivel(no, conexaoNo->chave, -conexaoNo->ponteiroDireito);
                delete conexaoNo;
                conexaoNo = nullptr;
            } else {
                ConexaoNo* novaConexaoNo = inserirChaveEmNoCheio(no, conexaoNo->chave, -conexaoNo->ponteiroDireito);
                delete conexaoNo;
                conexaoNo = novaConexaoNo;
            }
        }
    }
    // Caso o nó seja uma página de dados (folha)
    else {
        if (no->chave[CHAVES_POR_NO] == 0) {
            inserirChaveEmFolhaDisponivel(no, chave, ponteiro);
        } else {
            conexaoNo = inserirChaveEmFolhaCheia(no, chave, ponteiro);
        }
    }

    // Libera o nó atual e retorna o nó de conexão
    delete no;
    return conexaoNo;
}

// Insere uma chave na árvore e salva no arquivo de índice primário
void inserirNaArvore(int chave, int ponteiro) {
    ConexaoNo* conexaoNo;
    // Insere a chave na árvore e recebe o nó de conexão, se houver
    conexaoNo = inserirChaveNaArvore(obterNoDeDadosDoArquivoIndicePrimario(cabecalhoIndicePrimario->posicaoRaiz), chave, ponteiro);

    if (conexaoNo != nullptr) {
        DadosNo* novoNoDeDados = criarNoDeDados();
        int posicao;

        // Preenche o novo nó de dados com os valores do nó de conexão
        novoNoDeDados->chave[0] = conexaoNo->chave;
        novoNoDeDados->ponteiro[0] = conexaoNo->ponteiroEsquerdo;
        novoNoDeDados->ponteiro[1] = conexaoNo->ponteiroDireito;
        novoNoDeDados->tamanhoNo++;

        // Insere o novo nó de dados na posição correta e atualiza a posição
        posicao = inserirNoDeDadosNaPosicao(novoNoDeDados);
        novoNoDeDados->posicaoNo = posicao;
        atualizarPosicaoNoDeDados(novoNoDeDados);

        // Atualiza a posição da raiz no cabeçalho do arquivo de índice primário
        cabecalhoIndicePrimario->posicaoRaiz = -posicao;
        atualizarCabecalhoIndicePrimario();

        // Libera a memória alocada para os nós
        delete novoNoDeDados;
        delete conexaoNo;
    }
}

// Pega o ID do primeiro elemento de cada bloco no arquivo de entrada (hash) e insere na árvore
void inserirOutrosDadosNoArquivoIndicePrimario() {
    Bloco bloco = {0};

    // Posiciona o cursor no início do arquivo de dados
    arquivoHashEntrada->clear(); // Limpa o estado de erro do arquivo
    arquivoHashEntrada->seekg(0, arquivoHashEntrada->beg); // Posiciona o cursor no início do arquivo

    // Insere o ID inicial de cada bloco na árvore
    for (int i = 0; i < NUM_BUCKETS; i++) {
        // Faz a leitura de um bloco
        arquivoHashEntrada->read(reinterpret_cast<char*>(&bloco), sizeof(Bloco));
        
        // Verifica se o bloco não está vazio
        if (bloco.numRegistros > 0) {
            // Acessa o primeiro artigo diretamente do array `artigos` dentro de `Bloco`
            Artigo& primeiroArtigo = bloco.artigos[0];
            // Insere o ID do primeiro registro do bloco na árvore
            inserirNaArvore(primeiroArtigo.id, i);
        }
    }
}


// Função que faz o upload dos dados (arquivo hash) para o arquivo de índice primário
// void inserirNoArquivoIndicePrimario(fstream* arquivoHash, fstream* arquivoIndicePrimario) {
//     // Define os arquivos globais a serem utilizados
//     arquivoIndicePrimario = arquivoIndicePrimario;
//     arquivoHashEntrada = arquivoHash;

//     // Inicializa o cabeçalho no arquivo de índice primário
//     inicializarCabecalhoIndicePrimario();

//     // Aloca um nó raiz
//     DadosNo* raiz = criarNoDeDados();
//     // Escreve o nó raiz no arquivo
//     raiz->posicaoNo = inserirNoDeDadosNaPosicao(raiz);
//     // Atualiza a posição do nó raiz no arquivo
//     atualizarPosicaoNoDeDados(raiz);
//     delete raiz;

//     // Popula o arquivo de índice primário com os IDs dos primeiros elementos de cada bloco do arquivo de dados
//     inserirOutrosDadosNoArquivoIndicePrimario();

//     // Fecha os arquivos abertos
//     // arquivoIndicePrimario->close();
//     // arquivoHashEntrada->close();
// }

void inserirNoArquivoIndicePrimario(fstream* hashFile, fstream* indicePrimarioFile) {
    // Assign to global variables after renaming parameters
    arquivoIndicePrimario = indicePrimarioFile;
    arquivoHashEntrada = hashFile;

    // Initialize the primary index header
    inicializarCabecalhoIndicePrimario();

    // Create and initialize the root node
    DadosNo* raiz = criarNoDeDados();
    int rootPosition = inserirNoDeDadosNaPosicao(raiz);
    raiz->posicaoNo = rootPosition;
    atualizarPosicaoNoDeDados(raiz);
    
    // Set the root position in the header and update it
    cabecalhoIndicePrimario->posicaoRaiz = -rootPosition;
    atualizarCabecalhoIndicePrimario();

    delete raiz;
    inserirOutrosDadosNoArquivoIndicePrimario();
}
