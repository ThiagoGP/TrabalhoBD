#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "hash1.hpp"
#include "primeiroIndice.hpp"
#include "segundoIndice.hpp"

int main(int argc, const char* argv[]) {
    // Verifica se o arquivo de entrada foi especificado como argumento
    if (argc != 2) {
        std::cout << "Uso: ./upload <arquivo de entrada>" << std::endl;
        return 1;
    }

    std::string nomeArquivoEntrada = argv[1];
    std::string arquivoHash = "dataHash";
    std::string arquivoIndicePrimario = "indicePrimario";
    std::string arquivoIndiceSecundario = "indiceSecundario";
    std::string linha, aux;
    Artigo artigo;

    std::ifstream arquivoEntrada(nomeArquivoEntrada);
    
    // Abre o arquivo de hash para leitura e escrita em binário
    std::fstream* arquivoHashStream = new std::fstream(arquivoHash, std::fstream::in | std::fstream::out | std::fstream::trunc | std::ios::binary);
    abrirArquivo(arquivoHashStream);

    // Abre o arquivo de índice primário para leitura e escrita em binário
    std::fstream* arquivoIndicePrimarioStream = new std::fstream(arquivoIndicePrimario, std::fstream::in | std::fstream::out | std::fstream::trunc | std::ios::binary);
    
    // Abre o arquivo de índice secundário para leitura e escrita em binário
    std::fstream* arquivoIndiceSecundarioStream = new std::fstream(arquivoIndiceSecundario, std::fstream::in | std::fstream::out | std::fstream::trunc | std::ios::binary);

    std::cout << "Fazendo parsing e inserindo no arquivo de dados organizado por hash\n";
    
    // Lê o arquivo de entrada e insere no arquivo de dados organizado por hash
    while (std::getline(arquivoEntrada, linha)) {
        // Remove quebra de linha
        std::string linha_temp;
        for (auto& x : linha) {
            if (x != '\n' && x != 13) {
                linha_temp += x;
            }
        }
        linha = linha_temp;

        const char ultimoCaractere = linha.at(linha.length() - 1);
        if (ultimoCaractere == '\"' || ultimoCaractere == 'L') {
            // Parsing
            artigo.titulo[0] = '\0';
            artigo.autores[0] = '\0';
            artigo.dataAtualizacao[0] = '\0';
            artigo.resumo[0] = '\0';

            size_t pos = 0, indice = 0;
            std::string campo;
            std::string linha_temp2 = linha;
            std::string delimitador = "\";";

            while ((pos = linha_temp2.find(delimitador)) != std::string::npos) {
                campo = linha_temp2.substr(1, pos - 1);

                switch (indice) {
                    case 0:
                        artigo.id = std::stoi(campo);
                        break;
                    case 1:
                        std::strncpy(artigo.titulo, campo.c_str(), sizeof(artigo.titulo));
                        artigo.titulo[sizeof(artigo.titulo) - 1] = '\0';
                        break;
                    case 2:
                        artigo.ano = std::stoi(campo);
                        break;
                    case 3:
                        std::strncpy(artigo.autores, campo.c_str(), sizeof(artigo.autores));
                        artigo.autores[sizeof(artigo.autores) - 1] = '\0';
                        break;
                    case 4:
                        artigo.citacoes = std::stoi(campo);
                        break;
                    case 5:
                        std::strncpy(artigo.dataAtualizacao, campo.c_str(), sizeof(artigo.dataAtualizacao));
                        artigo.dataAtualizacao[sizeof(artigo.dataAtualizacao) - 1] = '\0';
                        break;
                    default:
                        break;
                }

                if (linha_temp2.at(pos + 2) == ';') {
                    indice += 2;
                    linha_temp2.erase(0, pos + delimitador.length() + 1);
                } else {
                    indice++;
                    linha_temp2.erase(0, pos + delimitador.length());
                }
            }

            if (linha_temp2.length() > 0) {
                if (linha_temp2 == "NULL") {
                    artigo.resumo[0] = '\0';
                } else {
                    linha_temp2.pop_back();
                    std::strncpy(artigo.resumo, linha_temp2.substr(1).c_str(), sizeof(artigo.resumo));
                    artigo.resumo[sizeof(artigo.resumo) - 1] = '\0';
                }
            }
        } else {
            std::getline(arquivoEntrada, aux);
            linha.append(aux);
            // Parsing da linha combinada
            artigo.titulo[0] = '\0';
            artigo.autores[0] = '\0';
            artigo.dataAtualizacao[0] = '\0';
            artigo.resumo[0] = '\0';

            size_t pos = 0, indice = 0;
            std::string campo;
            std::string linha_temp2 = linha;
            std::string delimitador = "\";";

            while ((pos = linha_temp2.find(delimitador)) != std::string::npos) {
                campo = linha_temp2.substr(1, pos - 1);

                switch (indice) {
                    case 0:
                        artigo.id = std::stoi(campo);
                        break;
                    case 1:
                        std::strncpy(artigo.titulo, campo.c_str(), sizeof(artigo.titulo));
                        artigo.titulo[sizeof(artigo.titulo) - 1] = '\0';
                        break;
                    case 2:
                        artigo.ano = std::stoi(campo);
                        break;
                    case 3:
                        std::strncpy(artigo.autores, campo.c_str(), sizeof(artigo.autores));
                        artigo.autores[sizeof(artigo.autores) - 1] = '\0';
                        break;
                    case 4:
                        artigo.citacoes = std::stoi(campo);
                        break;
                    case 5:
                        std::strncpy(artigo.dataAtualizacao, campo.c_str(), sizeof(artigo.dataAtualizacao));
                        artigo.dataAtualizacao[sizeof(artigo.dataAtualizacao) - 1] = '\0';
                        break;
                    default:
                        break;
                }

                if (linha_temp2.at(pos + 2) == ';') {
                    indice += 2;
                    linha_temp2.erase(0, pos + delimitador.length() + 1);
                } else {
                    indice++;
                    linha_temp2.erase(0, pos + delimitador.length());
                }
            }

            if (linha_temp2.length() > 0) {
                if (linha_temp2 == "NULL") {
                    artigo.resumo[0] = '\0';
                } else {
                    linha_temp2.pop_back();
                    std::strncpy(artigo.resumo, linha_temp2.substr(1).c_str(), sizeof(artigo.resumo));
                    artigo.resumo[sizeof(artigo.resumo) - 1] = '\0';
                }
            }
        }

        inserirRegistroNoArquivoHash(artigo, arquivoHashStream);
    }

    std::cout << "Sucesso na inserção no arquivo de hash!" << std::endl;

    // Criação do índice primário
    std::cout << "Carregando os dados para o índice primário..." << std::endl;
    inserirNoArquivoIndicePrimario(arquivoHashStream, arquivoIndicePrimarioStream);
    std::cout << "Sucesso na criação do índice primário!" << std::endl;

    // Criação do índice secundário
    std::cout << "Carregando os dados para o índice secundário..." << std::endl;
    inserirNoArquivoIndiceSecundario(arquivoHashStream, arquivoIndiceSecundarioStream);
    std::cout << "Sucesso na criação do índice secundário!" << std::endl;

    // Fecha e limpa a memória dos arquivos
    arquivoHashStream->close();
    arquivoIndicePrimarioStream->close();
    arquivoIndiceSecundarioStream->close();

    delete arquivoHashStream;
    delete arquivoIndicePrimarioStream;
    delete arquivoIndiceSecundarioStream;

    return 0;
}
