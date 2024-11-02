#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "hash1.hpp"
#include "primeiroIndice.hpp"
// #include "segundoIndice.hpp"
using namespace std;

// Copia uma string para um vetor de caracteres (priorizando registros de tamanho fixo)
void strParaChar(const string& origem, char destino[], size_t qtd_chr) {
    strncpy(destino, origem.c_str(), qtd_chr);
    destino[qtd_chr - 1] = '\0';  // Garante que a string seja nula-terminada
}

// Remove quebra de linha para tratamento da linha que entrará na função de parsing
string removerQuebraLinha(const string& s) {
    string aux;
    for (auto& x : s) {
        if (x != '\n' && x != 13) {
            aux += x;
        }
    }
    return aux;
}

// Função de parsing que extrai os campos da linha de entrada e preenche a estrutura Artigo
Artigo parser(const string& linha) {
    Artigo artigo;
    string delimitador = "\";";

    // Inicializa todos os campos de caracteres com uma string vazia
    artigo.titulo[0] = '\0';
    artigo.autores[0] = '\0';
    artigo.dataAtualizacao[0] = '\0';
    artigo.resumo[0] = '\0';

    size_t pos = 0, indice = 0;
    string campo;
    string linha_temp = linha;

    // Extrai dados dos campos da linha
    while ((pos = linha_temp.find(delimitador)) != string::npos) {
        campo = linha_temp.substr(1, pos - 1);

        switch (indice) {
            case 0:
                artigo.id = stoi(campo);
                break;
            case 1:
                strParaChar(campo, artigo.titulo, sizeof(artigo.titulo));
                break;
            case 2:
                artigo.ano = stoi(campo);
                break;
            case 3:
                strParaChar(campo, artigo.autores, sizeof(artigo.autores));
                break;
            case 4:
                artigo.citacoes = stoi(campo);
                break;
            case 5:
                strParaChar(campo, artigo.dataAtualizacao, sizeof(artigo.dataAtualizacao));
                break;
            default:
                break;
        }

        // Verifica ausência de dados em um campo da linha lida
        if (linha_temp.at(pos + 2) == ';') {
            indice += 2;
            linha_temp.erase(0, pos + delimitador.length() + 1);
        } else {
            indice++;
            linha_temp.erase(0, pos + delimitador.length());
        }
    }

    // Extrai o campo 'resumo'
    if (linha_temp.length() > 0) {
        if (linha_temp == "NULL") {
            artigo.resumo[0] = '\0';  // Resumo é NULL
        } else {
            linha_temp.pop_back();
            strParaChar(linha_temp.substr(1), artigo.resumo, sizeof(artigo.resumo));
        }
    }

    return artigo;
}

int main(int argc, const char* argv[]) {
    // Verifica se o arquivo de entrada foi especificado como argumento
    if (argc != 2) {
        cout << "Uso: ./upload <arquivo de entrada>" << endl;
        return 1;
    }

    string nomeArquivoEntrada = argv[1];
    string arquivoHash = "dataHash";
    string arquivoIndicePrimario = "indicePrimario";
    string arquivoIndiceSecundario = "indiceSecundario";
    string linha, aux;
    Artigo artigo;

    ifstream arquivoEntrada(nomeArquivoEntrada);
    
    // Abre o arquivo de hash para leitura e escrita em binário
    fstream* arquivoHashStream = new fstream(arquivoHash, fstream::in | fstream::out | fstream::trunc | ios::binary);
    abrirArquivo(arquivoHashStream);

    // Abre o arquivo de índice primário para leitura e escrita em binário
    fstream* arquivoIndicePrimarioStream = new fstream(arquivoIndicePrimario, fstream::in | fstream::out | fstream::trunc | ios::binary);
    
    // Abre o arquivo de índice secundário para leitura e escrita em binário
    // fstream* arquivoIndiceSecundarioStream = new fstream(arquivoIndiceSecundario, fstream::in | fstream::out | fstream::trunc | ios::binary);

    cout << "Fazendo parsing e inserindo no arquivo de dados organizado por hash\n";
    
    // Lê o arquivo de entrada e insere no arquivo de dados organizado por hash
    while (getline(arquivoEntrada, linha)) {
        linha = removerQuebraLinha(linha);

        const char ultimoCaractere = linha.at(linha.length() - 1);
        if (ultimoCaractere == '\"' || ultimoCaractere == 'L') {
            artigo = parser(linha);
        } else {
            getline(arquivoEntrada, aux);
            linha.append(aux);
            artigo = parser(linha);
        }
        inserirRegistroNoArquivoHash(artigo, arquivoHashStream);
    }

    cout << "Sucesso na inserção no arquivo de hash!" << endl;

    // Criação do índice primário
    cout << "Carregando os dados para o índice primário..." << endl;
    inserirNoArquivoIndicePrimario(arquivoHashStream, arquivoIndicePrimarioStream);
    cout << "Sucesso na criação do índice primário!" << endl;

    // Criação do índice secundário
    cout << "Carregando os dados para o índice secundário..." << endl;
    // inserirNoArquivoIndiceSecundario(arquivoHashStream, arquivoIndiceSecundarioStream);
    cout << "Sucesso na criação do índice secundário!" << endl;

    // Fecha e limpa a memória dos arquivos
    arquivoHashStream->close();
    arquivoIndicePrimarioStream->close();
    // arquivoIndiceSecundarioStream->close();

    delete arquivoHashStream;
    delete arquivoIndicePrimarioStream;
    // delete arquivoIndiceSecundarioStream;

    return 0;
}