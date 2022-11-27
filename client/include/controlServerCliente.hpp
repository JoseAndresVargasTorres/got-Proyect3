#ifndef MANEJAR_JSON_H
#define MANEJAR_JSON_H

#include <iostream>
#include <dirent.h>
#include <string>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <jsoncpp/json/json.h>

using namespace std;

class Control
{
public:
    static void list_dir(const char *dir_name);
    static void control_local_server(string elem);
    static void escribir_json(string ruta, Json::Value root);
    static std::string leer_json(string ruta, string dato);
    static std::string leer_json2(string ruta, string dato);
    static bool isIgnored(string ruta);
    static std::string encriptar_texto_a_ascii(string texto);
    static std::string desencriptar_ascii_a_texto(string texto_final_ascii);
};

#endif