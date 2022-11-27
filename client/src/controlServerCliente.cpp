#include <client.hpp>
#include <controlServerCliente.hpp>

//Revisa si el archivo o carpeta esta en la lista del .gotignore
bool Control::isIgnored(string ruta)
{
    fstream gotignore;
    string line = "";

    gotignore.open(Command::thisPath + ".gotignore");
    if (gotignore.is_open())
    {
        while (!gotignore.eof())
        {
            getline(gotignore, line);
            if (line.back() == '/')
            {
                if (ruta.find(line) != std::string::npos)
                {
                    return true;
                }
            }
            else
            {
                if (strcmp(ruta.c_str(), line.c_str()) == 0)
                {
                    return true;
                }
            }
        }
        gotignore.close();
    }
    return false;
}

// Listar los directorios que hay dentro
void Control::list_dir(const char *dir_name)
{
    DIR *d;
    d = opendir(dir_name);
    if (!d)
    {
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        struct dirent *entry;
        const char *d_name;
        entry = readdir(d);
        if (!entry)
        {
            break;
        }
        d_name = entry->d_name;

        if (!(entry->d_type & DT_DIR))
        {
            string ruta1 = dir_name;
            string ruta2 = d_name;
            string ruta3 = ruta1 + "/" + ruta2;
            ruta3.erase(0, Command::thisPath.size() + 1);

            if (strcmp(ruta3.c_str(), "got"))
            {
                control_local_server(ruta3);
            }
        }

        if (entry->d_type & DT_DIR)
        {
            if (strcmp(d_name, "..") != 0 &&
                strcmp(d_name, ".") != 0)
            {
                int path_length;
                char path[PATH_MAX];

                path_length = snprintf(path, PATH_MAX,
                                       "%s/%s", dir_name, d_name);
                if (path_length >= PATH_MAX)
                {
                    exit(EXIT_FAILURE);
                }
                string temp = path;
                temp.erase(0, Command::thisPath.length() + 1);
                if (strcmp(temp.c_str(), ".got"))
                {
                    list_dir(path);
                }
            }
        }
    }

    if (closedir(d))
    {
        exit(EXIT_FAILURE);
    }

    Command::updateIgnore();
}

void Control::control_local_server(string elem)
{

    ifstream ifs(Command::thisPath + ".got/control_cliente.json");
    Json::Reader reader;
    Json::Value root;
    reader.parse(ifs, root);
    Json::StyledStreamWriter writer;
    std::ofstream outFile;

    bool guardar = true;
    // No copiar los archivos nuevamente
    for (Json::Value::const_iterator it = root["archivos"].begin(); it != root["archivos"].end(); ++it)
    {
        if (it.key().asString() == elem)
        {
            guardar = false;
        }
    }

    if (guardar)
    {
        root["archivos"][elem] = "no_controlado";
    }

    outFile.open(Command::thisPath + ".got/control_cliente.json");
    writer.write(outFile, root);
    outFile.close();
}

void Control::escribir_json(string ruta, Json::Value root)
{

    Json::StyledStreamWriter writer;
    std::ofstream outFile;
    outFile.open(Command::thisPath + ruta);
    writer.write(outFile, root);
    outFile.close();
}

std::string Control::leer_json(string ruta, string llave)
{

    ifstream ifs(Command::thisPath + ruta);
    Json::Reader reader;
    Json::Value root;
    reader.parse(ifs, root);

    return root[llave].asString();
}

std::string Control::leer_json2(string ruta, string key)
{

    ifstream ifs(Command::thisPath + ruta);
    Json::Reader reader;
    Json::Value root;
    reader.parse(ifs, root);
    const Json::Value &characters = root["rows"]; // array of characters
    string valor;
    for (int i = 0; i < characters.size(); i++)
    {
        for (Json::Value::const_iterator it = characters[i].begin(); it != characters[i].end(); ++it)
        {
            if (it.key().asString() == key)
            {
                valor = it->asString();
                break;
            }
        }
    }

    return valor;
}

std::string Control::encriptar_texto_a_ascii(string texto)
{
    char letra;
    string texto_final_ascii = "";

    while (texto.size() > 0)
    {
        letra = texto[0];
        texto_final_ascii += to_string(int(letra)) + "/";
        texto.erase(0, 1);
    }

    return texto_final_ascii;
}

std::string Control::desencriptar_ascii_a_texto(string texto_final_ascii)
{

    string valor_ascii = "";
    string cadena_descriptada = "";
    while (texto_final_ascii.size() > 0)
    {
        while (texto_final_ascii[0] != '/')
        {
            valor_ascii += texto_final_ascii[0];
            texto_final_ascii.erase(0, 1);
        }
        texto_final_ascii.erase(0, 1);
        cadena_descriptada += char(stoi(valor_ascii));
        valor_ascii = "";
    }

    return cadena_descriptada;
}