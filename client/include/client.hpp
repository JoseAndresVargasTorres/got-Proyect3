#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <cpr/cpr.h>
#include <huffman.hpp>
#include "md5.h"
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <commands.hpp>

using namespace std;

class Client
{
private:
    static Client *client_instance;
    string post_body, received_body, url;
    int status_response;

private:
    /**
     * Constructor de la clase cliente.
     */
    Client()
    {
        url = "http://localhost:3000/";
        post_body = "";
        received_body = "";
        status_response = 0;
    }
    /**
     * Destructor de la clase cliente.
     */
    ~Client()
    {
    }

public:
    /**
     * Método que obtiene la instancia del cliente
     * @return - Instancia del cliente
     */
    static Client *getI();
    /**
     * Método que evita la reasignación de la instancia del cliente
     * @param other - Cliente creado
     */
    Client(Client &other) = delete;
    /**
     * Método que evita la reasignación de la instancia del cliente
     */
    void operator=(const Client &) = delete;

public:
    void setBody(string jsonFile);
    int getStatus();
    string getPath();

    void POST(string _url, string jsonFile);
    void GET(string _url, string jsonFile);
    void PUT(string _url, string jsonFile);
    void DELETE(string _url);
    void limpiar();
};

#endif