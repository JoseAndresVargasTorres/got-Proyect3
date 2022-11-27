#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <fstream>
#include <string>
#include "spdlog/spdlog.h"

using namespace std;

void buildHuffmanTree(string texto_final);
string descomprimir_data(string codigo_binario, string simbolo_codigo);
string pedir_codigoBinario();
string pedir_simboloCodigo();
string generar_string_de_archivo(string ruta);

#endif