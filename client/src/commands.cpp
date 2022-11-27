
#include <client.hpp>
#include <controlServerCliente.hpp>

using namespace std;

string Command::thisPath = "";

string Command::diff_a_string()
{

    fstream ficheroEntrada;
    string linea_texto = "";
    string texto_final = "";

    ficheroEntrada.open(Command::thisPath + "../repo/.got/test.patch", ios::in);
    if (ficheroEntrada.is_open())
    {
        while (!ficheroEntrada.eof())
        {
            getline(ficheroEntrada, linea_texto);
            texto_final += linea_texto + "\n";
        }
        ficheroEntrada.close();
    }
    else
    {
        spdlog::error("Fichero inexistente o faltan permisos para abrirlo (Diff)");
    }
    texto_final.erase(texto_final.size() - 1);
    return texto_final;
}

void Command::diff_sync(string fileAfter, string fileBefore, string patch)
{
    string command = "diff -DVERSION1 " + fileBefore + " " + fileAfter + " > " + patch;
    int sin_uso = system(command.c_str());
}

void Command::diff(string fileAfter, string fileBefore, string patch)
{
    string command = "diff -e " + fileBefore + " " + fileAfter + " > " + patch;
    int sin_uso = system(command.c_str());
}

void Command::applyChanges(string original, string changes)
{
    string command = "patch " + original + " " + changes;
    int sin_uso = system(command.c_str());
}

string Command::get_selfpath()
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return std::string(result, (count > 0) ? count : 0);
}

void Command::updateIgnore()
{
    ifstream ifs(Command::thisPath + ".got/control_cliente.json");
    Json::Reader reader;
    Json::Value root;
    reader.parse(ifs, root);
    string aux = "archivos";
    for (Json::Value::const_iterator it = root[aux].begin(); it != root[aux].end(); ++it)
    {
        string file = it.name();
        if (Control::isIgnored(file))
        {
            root["archivos"][it.key().asString()] = "ignorado";
        }
    }

    Control::escribir_json(".got/control_cliente.json", root);

    ifs.close();
}

void Command::init(string repoName)
{
    if (mkdir(".got", 0777) == -1)
    {
        spdlog::error("Error : {}", strerror(errno));
    }
    else
    {
        spdlog::info("Directorio .got creado");
    }

    ofstream file;
    file.open(thisPath + ".gotignore");
    string mensaje = "PARA IGNORAR CARPETAS: <carpeta>/\nPARA IGNORAR ARCHIVOS: <archivo>.<extension>\n";
    mensaje += "PARA IGNORAR ARCHIVOS DENTRO DE CARPETAS: <carpeta>/<archivo>.<extension>\n";
    file << mensaje;
    file.close();
    spdlog::info("Agregado el archivo .gotignore");

    //Agrega los archivos JSON
    file.open(thisPath + ".got/enviado.json");
    file.close();
    file.open(thisPath + ".got/recibido.json");
    file.close();
    file.open(thisPath + ".got/control_cliente.json");
    file.close();

    // Crea la estructura control cliente-server
    Json::Value root;
    root["id_repositorio"] = 0;
    root["commit"] = 0;
    root["hash_commit"] = "";
    root["archivos"];
    Control::escribir_json(".got/control_cliente.json", root);

    // Agregar directorios al json
    const char *dir_name = thisPath.c_str();
    Control::list_dir(dir_name);

    // Escribe la estructura repositorio y envia datos
    Json::Value root0;
    root0["nombre_repositorio"] = repoName;
    Control::escribir_json(".got/enviado.json", root0);
    Client::getI()->POST("repositorio", thisPath + ".got/enviado.json");
    spdlog::info("Repositorio agregado en el server!");
    spdlog::info(Client::getI()->getStatus());

    // Guardar el # del repositorio
    string dato_retornado = Control::leer_json(".got/recibido.json", "id_repositorio");
    // Leer archivo para acualizarlo
    ifstream ifs1(Command::thisPath + ".got/control_cliente.json");
    Json::Reader reader1;
    Json::Value root1;
    reader1.parse(ifs1, root1);
    root1["id_repositorio"] = dato_retornado;
    Control::escribir_json(".got/control_cliente.json", root1);
}

void Command::add(string solicitud_archivos)
{
    // Antes de agregar los archivos, actualizar directorio
    const char *dir_name = thisPath.c_str();
    Control::list_dir(dir_name);

    // Leer control_cliente.json
    ifstream ifs(Command::thisPath + ".got/control_cliente.json");
    Json::Reader reader;
    Json::Value root;
    reader.parse(ifs, root);
    bool archivo_existente = true;

    if (solicitud_archivos == "-A")
    {
        string aux = "archivos";
        for (Json::Value::const_iterator it = root[aux].begin(); it != root[aux].end(); ++it)
        {

            if (root["archivos"][it.key().asString()] == "no_controlado")
            {
                root["archivos"][it.key().asString()] = "agregado";
                spdlog::info("Archivo agregado!");
            }
            else if (root["archivos"][it.key().asString()] == "server")
            {
                root["archivos"][it.key().asString()] = "modificado";
                spdlog::info("El archivo ha sido modificado!");
            }
            else if (root["archivos"][it.key().asString()] == "agregado")
            {
                spdlog::info("El archivo ya ha sido agregado!");
            }
        }
    }
    else
    {
        //Agregar uno
        if (root["archivos"][solicitud_archivos] == "no_controlado")
        {
            root["archivos"][solicitud_archivos] = "agregado";
            spdlog::info("Archivo agregado!");
        }
        else if (root["archivos"][solicitud_archivos] == "server")
        {
            root["archivos"][solicitud_archivos] = "modificado";
            spdlog::info("El archivo ha sido modificado!");
        }
        else if (root["archivos"][solicitud_archivos] == "agregado")
        {
            spdlog::info("El archivo ya ha sido agregado!");
        }
        else
        {
            archivo_existente = false;
            spdlog::info("El archivo no existe!");
        }
    }

    if (archivo_existente == true)
    {
        Control::escribir_json(".got/control_cliente.json", root);
    }
    ifs.close();
}

void Command::commit(string comentario)
{

    // Guardar el # repo actual
    string repo_actual = Control::leer_json(".got/control_cliente.json", "id_repositorio");

    // Guardar el # commit actual
    string dato_retornado = Control::leer_json(".got/control_cliente.json", "commit");
    string dato_retornado2;

    int relacion_actual_commit = stoi(dato_retornado) + 1;

    // Escribe la estructura commit y envia datos
    Json::Value root;
    root["hash_commit"] = relacion_actual_commit;
    root["comentario"] = comentario;
    root["relacion_repositorio"] = stoi(repo_actual);
    Control::escribir_json(".got/enviado.json", root);
    Client::getI()->POST("commit", thisPath + ".got/enviado.json");
    spdlog::info("Commit agregado en el server!");
    spdlog::info(Client::getI()->getStatus());

    // Guardar HASH generado
    dato_retornado = Control::leer_json(".got/recibido.json", "hash_commit");

    // Leer archivo para acualizarlo
    ifstream ifs1(Command::thisPath + ".got/control_cliente.json");
    Json::Reader reader1;
    Json::Value root1;
    reader1.parse(ifs1, root1);
    root1["hash_commit"] = dato_retornado;
    root1["commit"] = relacion_actual_commit;
    Control::escribir_json(".got/control_cliente.json", root1);

    // Enviar archivos
    string aux = "archivos";
    string texto_final = "";
    string cod_binario = "";
    string sim_cod = "";
    string temporal_ascii_texto = "";
    string temporal_texto_ascii = "";
    for (Json::Value::const_iterator it = root1[aux].begin(); it != root1[aux].end(); ++it)
    {

        if (root1["archivos"][it.key().asString()] == "agregado")
        {
            // Comprimir datos
            texto_final = generar_string_de_archivo(Command::thisPath + it.key().asString());
            buildHuffmanTree(texto_final);
            cod_binario = pedir_codigoBinario();
            sim_cod = pedir_simboloCodigo();

            // Encriptar a ASCII para que no haya problemas en json
            temporal_ascii_texto = Control::encriptar_texto_a_ascii(sim_cod);

            // Escribe la estructura  archivo y envia datos
            Json::Value root2;
            root2["nombre_archivo"] = it.key().asString();
            root2["codigo_huffman"] = cod_binario;
            root2["simbolo_codigo"] = temporal_ascii_texto;
            root2["relacion_commit"] = relacion_actual_commit;
            Control::escribir_json(".got/enviado.json", root2);

            Client::getI()->POST("archivo", thisPath + ".got/enviado.json");
            spdlog::info("Archivo agregado en el server!");
            spdlog::info(Client::getI()->getStatus());

            // Cambiar estado agregado -> server
            root1["archivos"][it.key().asString()] = "server";
            Control::escribir_json(".got/control_cliente.json", root1);
        }
        else if (root1["archivos"][it.key().asString()] == "modificado")
        {
            string comparacion_diff = "";
            if (relacion_actual_commit == 2)
            {
                spdlog::info("Agregando diff's del commit 2!");
                // Pedir Huffman del commit 1
                Json::Value root3;
                root3["id_commit"] = 1; // Directamente al commit 1
                root3["nombre_archivo"] = it.key().asString();
                Control::escribir_json(".got/enviado.json", root3);
                Client::getI()->GET("codigo_huffman", thisPath + ".got/enviado.json");
                spdlog::info("Obteniendo codigo huffman!");
                spdlog::info(Client::getI()->getStatus());
                // Guardar hufmman
                dato_retornado = Control::leer_json2(".got/recibido.json", "CONVERT(archivo.codigo_huffman USING utf8)");
                dato_retornado2 = Control::leer_json2(".got/recibido.json", "CONVERT(archivo.simbolo_codigo USING utf8)");

                // Desencriptar ASCII a TEXTO
                temporal_texto_ascii = Control::desencriptar_ascii_a_texto(dato_retornado2);

                // Crear archivo con huffman
                ofstream fs(Command::thisPath + "../repo/.got/test.txt");
                fs << descomprimir_data(dato_retornado, temporal_texto_ascii);
                fs.close();

                // Saca diferencias del Antes con el Despues. Poner RUTA DESPUES, LUEGO RUTA ANTES
                Command::diff(Command::thisPath + it.key().asString(),
                              Command::thisPath + "../repo/.got/test.txt",
                              Command::thisPath + "../repo/.got/test.patch");

                comparacion_diff = Command::diff_a_string();
            }
            else
            {
                spdlog::info("Agregando diff's del commit > 2!");
                // Pedir diferencias (diff anterior)
                // Escribe la estructura diff_anterior y envia datos
                Json::Value root4;
                root4["id_commit"] = relacion_actual_commit - 1; // Se resta 1 para obtener diff_anterior
                root4["nombre_archivo"] = it.key().asString();
                Control::escribir_json(".got/enviado.json", root4);
                Client::getI()->GET("commit_anterior", thisPath + ".got/enviado.json");
                spdlog::info("Obteniendo codigo diff!");
                spdlog::info(Client::getI()->getStatus());
                // Obtener codigo_diff_anterior
                dato_retornado = Control::leer_json2(".got/recibido.json", "CONVERT(diff.codigo_diff_anterior USING utf8)");

                // Desencriptar ASCII a TEXTO
                temporal_texto_ascii = Control::desencriptar_ascii_a_texto(dato_retornado);

                // Para revisar si hay cambios
                // Copiar codigo_diff_anterio en test.patch
                ofstream fs(Command::thisPath + "../repo/.got/test.patch");
                fs << temporal_texto_ascii;
                fs.close();

                // Regresar el archivo al commmit pasado
                Command::applyChanges(Command::thisPath + "../repo/.got/test.txt",
                                      Command::thisPath + "../repo/.got/test.patch");

                // Compararlo con el actual
                Command::diff(Command::thisPath + it.key().asString(),
                              Command::thisPath + "../repo/.got/test.txt",
                              Command::thisPath + "../repo/.got/test.patch");

                comparacion_diff = Command::diff_a_string();
            }

            // Si el archivo .patch esta en blanco, no hay diferencia
            if (comparacion_diff == "")
            {
                spdlog::info("No hay cambios en el archivo!");
            }
            else
            {
                // Sacar MD5
                string codigo_checksum = md5(to_string(relacion_actual_commit));

                // Encriptar a ASCII para que no haya problemas en json
                temporal_ascii_texto = Control::encriptar_texto_a_ascii(comparacion_diff);

                // Escribe la estructura diff y envia datos
                Json::Value root5;
                root5["nombre_archivo"] = it.key().asString();
                root5["codigo_diff_anterior"] = temporal_ascii_texto;
                root5["codigo_diff_posterior"] = ""; // Va vacio, el proximo commit lo actualiza
                root5["checksum"] = codigo_checksum;
                root5["relacion_commit"] = relacion_actual_commit;
                Control::escribir_json(".got/enviado.json", root5);

                Client::getI()->POST("diff", thisPath + ".got/enviado.json");
                spdlog::info("Diff agregado en el server!");
                spdlog::info(Client::getI()->getStatus());
            }

            // Cambiar estado modificado -> server
            root1["archivos"][it.key().asString()] = "server";
            Control::escribir_json(".got/control_cliente.json", root1);
        }
    }
    ifs1.close();
}

void Command::status(string archivo)
{
    vector<pair<string, string>> vect_historial_archivos;

    if (archivo == "")
    {
        // Leer control_cliente.json
        ifstream ifs(Command::thisPath + ".got/control_cliente.json");
        Json::Reader reader;
        Json::Value root;
        reader.parse(ifs, root);
        string aux = "archivos";
        for (Json::Value::const_iterator it = root[aux].begin(); it != root[aux].end(); ++it)
        {
            if (root["archivos"][it.key().asString()] == "no_controlado" || root["archivos"][it.key().asString()] == "agregado")
            {
                spdlog::info("El archivo " + it.key().asString() + " ha sido agregado respecto al commit anterior");
            }
            else if (root["archivos"][it.key().asString()] == "modificado")
            {
                spdlog::info("El archivo " + it.key().asString() + " ha sido modificado respecto al commit anterior");
            }
        }
    }
    else
    {
        // Guardar el # commit actual
        int commit_anterior = stoi(Control::leer_json(".got/control_cliente.json", "commit"));
        string archivo_retornado;
        while (commit_anterior > 0)
        {
            // Pedir archivo agregado del commit anterior
            Json::Value root3;
            root3["id_commit"] = commit_anterior;
            root3["nombre_archivo"] = archivo;
            Control::escribir_json(".got/enviado.json", root3);
            Client::getI()->GET("agregados_commit", thisPath + ".got/enviado.json");
            spdlog::info("Obteniendo archivo agregado del commit anterior!");
            spdlog::info(Client::getI()->getStatus());

            // Guardar si/no esta generado
            archivo_retornado = Control::leer_json(".got/recibido.json", "existencia");
            if (archivo_retornado == "si")
            {
                vect_historial_archivos.push_back(make_pair(to_string(commit_anterior), "agregado"));
            }
            else
            {
                // Pedir archivo modificado del commit anterior
                Json::Value root4;
                root4["id_commit"] = commit_anterior;
                root4["nombre_archivo"] = archivo;
                Control::escribir_json(".got/enviado.json", root4);
                Client::getI()->GET("modificaciones_commit", thisPath + ".got/enviado.json");
                spdlog::info("Obteniendo archivo modificado del commit anterior!");
                spdlog::info(Client::getI()->getStatus());

                // Guardar si/no esta generado
                archivo_retornado = Control::leer_json(".got/recibido.json", "existencia");
                if (archivo_retornado == "si")
                {
                    vect_historial_archivos.push_back(make_pair(to_string(commit_anterior), "modificado"));
                }
            }

            commit_anterior -= 1;
        }
    }

    // Mostrar los archivos agregados/modificados
    for (int i = 0; i < vect_historial_archivos.size(); i++)
    {
        spdlog::info("En el commit # " + vect_historial_archivos[i].first + " el archivo " + archivo + " fue " + vect_historial_archivos[i].second);
    }
}

void Command::log()
{

    // Guardar el # commit actual
    int commit_anterior = stoi(Control::leer_json(".got/control_cliente.json", "commit"));
    string hash_retornado;
    string comentario_retornado;
    while (commit_anterior != 0)
    {

        // Pedir hash's
        Client::getI()->GET("hash_comentario/" + to_string(commit_anterior), thisPath + ".got/enviado.json");
        spdlog::info("Obteniendo hash y comentario!");
        spdlog::info(Client::getI()->getStatus());

        // Guardar resultados
        hash_retornado = Control::leer_json2(".got/recibido.json", "hash_commit");
        comentario_retornado = Control::leer_json2(".got/recibido.json", "comentario");

        // Mostrar hash
        spdlog::info("Version: {}", commit_anterior);
        spdlog::info("Hash: {}", hash_retornado);
        spdlog::info("Comentario: {}", comentario_retornado);

        commit_anterior -= 1;
    }
}

void Command::rollback(string archivo, string commit, bool ruta_externa)
{

    // Escribe el contenido en ruta actual o temporal
    string ruta = "";
    if (ruta_externa)
    {
        ruta = Command::thisPath + "../repo/.got/test.txt";
    }
    else
    {
        ruta = Command::thisPath + archivo;
    }

    string temporal_ascii_texto = "";
    string temporal_texto_ascii = "";

    // Consulta cual fue el commit inicial del archivo
    // Se supone que solo existe un archivo con ese nombre!
    Json::Value root0;
    root0["nombre_archivo"] = archivo;
    Control::escribir_json(".got/enviado.json", root0);
    Client::getI()->GET("solicitar_commit", thisPath + ".got/enviado.json");
    spdlog::info("Obteniendo commit inicial del archivo!");
    spdlog::info(Client::getI()->getStatus());

    // Guardar relacion_commit del archivo
    string relacion_commit_retornado = Control::leer_json2(".got/recibido.json", "relacion_commit");

    // Crea el primer archivo
    // Pedir Huffman del commit inicial del archivo
    Json::Value root;
    root["id_commit"] = relacion_commit_retornado;
    root["nombre_archivo"] = archivo;
    Control::escribir_json(".got/enviado.json", root);
    Client::getI()->GET("codigo_huffman", thisPath + ".got/enviado.json");
    spdlog::info("Obteniendo codigo huffman!");
    spdlog::info(Client::getI()->getStatus());
    // Guardar hufmman
    string dato_retornado = Control::leer_json2(".got/recibido.json", "CONVERT(archivo.codigo_huffman USING utf8)");
    string dato_retornado2 = Control::leer_json2(".got/recibido.json", "CONVERT(archivo.simbolo_codigo USING utf8)");

    // Desencriptar ASCII a TEXTO
    temporal_texto_ascii = Control::desencriptar_ascii_a_texto(dato_retornado2);

    // Crear archivo con huffman (en la ruta actual/temporal)
    ofstream fs(ruta);
    fs << descomprimir_data(dato_retornado, temporal_texto_ascii);
    fs.close();

    if (stoi(commit) > 1)
    {
        // Aplicar diff hasta el commit pedido
        int commit_pedido = stoi(commit);
        int commit_actual = 2;
        string diff_retornado;
        while (commit_actual <= commit_pedido)
        {
            // Pedir diferencias (diff)
            // Escribe la estructura diff_anterior y envia datos
            Json::Value root2;
            root2["id_commit"] = commit_actual;
            root2["nombre_archivo"] = archivo;
            Control::escribir_json(".got/enviado.json", root2);
            Client::getI()->GET("commit_anterior", thisPath + ".got/enviado.json");
            spdlog::info("Obteniendo codigo diff!");
            spdlog::info(Client::getI()->getStatus());
            // Obtener codigo_diff_anterior
            diff_retornado = Control::leer_json2(".got/recibido.json", "CONVERT(diff.codigo_diff_anterior USING utf8)");

            // Desencriptar ASCII a TEXTO
            temporal_texto_ascii = Control::desencriptar_ascii_a_texto(diff_retornado);

            // Para revisar si hay cambios
            // Copiar codigo_diff_anterio en test.patch
            ofstream fs(Command::thisPath + "../repo/.got/test.patch");
            fs << temporal_texto_ascii;
            fs.close();

            // Regresar el archivo al commmit pasado (en la ruta actual/temporal)
            Command::applyChanges(ruta,
                                  Command::thisPath + "../repo/.got/test.patch");

            commit_actual += 1;
        }
    }
}

void Command::reset(string archivo)
{

    // Guardar el # commit actual
    string commit_retornado = Control::leer_json(".got/control_cliente.json", "commit");

    // Reset funciona similar a rollback
    Command::rollback(archivo, commit_retornado, false);
}

void Command::sync(string archivo)
{
    // Guardar el # repositorio actual
    string repositorio_retornado = Control::leer_json(".got/control_cliente.json", "id_repositorio");

    // Pedir commit actual (solicitado del server)
    // Escribe la estructura ultimo_commit y envia datos
    Json::Value root;
    root["relacion_repositorio"] = repositorio_retornado;
    Control::escribir_json(".got/enviado.json", root);
    Client::getI()->GET("ultimo_commit", thisPath + ".got/enviado.json");
    spdlog::info("Obteniendo ultimo commit!");
    spdlog::info(Client::getI()->getStatus());

    // Guardar el # commit actual
    string commit_retornado = Control::leer_json2(".got/recibido.json", "id_commit");

    // Se aplica rollback para llegar hasta el commit actual
    Command::rollback(archivo, commit_retornado, true);

    // Se verifica si hay cambios del commit actual con el trabajo actual
    // Compararlo con el actual
    Command::diff(Command::thisPath + archivo,
                  Command::thisPath + "../repo/.got/test.txt",
                  Command::thisPath + "../repo/.got/test.patch");

    string comparacion_diff = Command::diff_a_string();

    // Si hay cambios se aplica sync
    // Si el archivo .patch esta en blanco, no hay diferencia
    if (comparacion_diff == "")
    {
        spdlog::info("No hay cambios en el archivo!");
    }
    else
    {
        // Sincroniza los dos archivos
        Command::diff_sync(Command::thisPath + archivo,
                           Command::thisPath + "../repo/.got/test.txt",
                           Command::thisPath + "../repo/.got/test.patch");

        // Copiar de test.patch a archivo
        string patch_a_archivo = Command::diff_a_string();
        ofstream fs(Command::thisPath + archivo);
        fs << patch_a_archivo;
        fs.close();
    }
}