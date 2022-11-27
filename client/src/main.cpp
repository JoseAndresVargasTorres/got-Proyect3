
#include <client.hpp>
#include <commands.hpp>

int main(int argc, char *argv[])
{
    Command::thisPath = Command::get_selfpath();
    Command::thisPath.erase(Command::thisPath.length() - 3, Command::thisPath.length());

    if (argc < 2)
    {
        spdlog::warn("Argumentos insuficientes.");
        spdlog::info("Para ver la lista de opciones disponibles utilice el comando: ./got help");
    }
    else if (!strcmp(argv[1], "init"))
    {
        if (argc != 3)
        {
            spdlog::warn("Indique un nombre para el repositorio.");
            spdlog::info("Para ver la lista de opciones disponibles utilice el comando: ./got help");
        }
        else
        {
            spdlog::info("Iniciando el repositorio en la ruta actual.");
            Command::init(argv[2]);
            spdlog::info("Repositorio {} listo.", argv[2]);
        }
    }
    else if (!strcmp(argv[1], "add"))
    {
        if (argc < 3)
        {
            spdlog::warn("Indique cuales archivos desea agregar.");
            spdlog::info("Para ver la lista de opciones disponibles utilice el comando: ./got help");
        }
        else if (!strcmp(argv[2], "-A"))
        {
            //AGREGANDO TODOS LOS ARCHIVOS
            spdlog::info("Agregando todos los archivos al commit.");
            Command::add(argv[2]);
        }
        else
        {
            for (int i = 2; i < argc; i++)
            {
                //AGREGANDO ARCHIVO
                spdlog::info("Agregando el archivo {} al commit.", argv[i]);
                Command::add(argv[2]);
            }
        }
    }
    else if (!strcmp(argv[1], "commit"))
    {
        if (argc < 3)
        {
            spdlog::warn("Agregue un mensaje para el commit.");
            spdlog::info("Para ver la lista de opciones disponibles utilice el comando: ./got help");
        }
        else
        {
            string msg = "";
            for (int i = 2; i < argc; i++)
            {
                msg += argv[i];
                msg += " ";
            }
            //REALIZA EL COMMIT
            Command::commit(argv[2]);
            spdlog::info("Commit realizado: {}", msg);
        }
    }
    else if (!strcmp(argv[1], "status"))
    {
        if (argc < 3)
        {
            Command::status("");
            spdlog::info("Mostrando historial completo...");
            //MOSTRAR HISTORIAL PARA TODOS LOS ARCHIVOS
        }
        else if (argc > 3)
        {
            spdlog::warn("Indique únicamente un archivo para mostrar.");
            spdlog::info("Para ver la lista de opciones disponibles utilice el comando: ./got help");
        }
        else
        {
            Command::status(argv[2]);
            spdlog::info("Mostrando historial para {}", argv[2]);
            //MOSTRAR HISTORIAL PARA 1 ARCHIVO
        }
    }
    else if (!strcmp(argv[1], "rollback"))
    {
        if (argc < 3)
        {
            spdlog::warn("Especifique el commit.");
            spdlog::info("Para ver la lista de opciones disponibles utilice el comando: ./got help");
        }
        else if (argc > 4)
        {
            spdlog::warn("Especifique únicamente un commit y un archivo.");
            spdlog::info("Para ver la lista de opciones disponibles utilice el comando: ./got help");
        }
        else if (argc == 3)
        {
            spdlog::info("Regresando todos los archivos al commit {}", argv[2]);
            //DEVOLVER LOS ARCHIVOS AL COMMIT ESPECIFICO
        }
        else
        {
            Command::rollback(argv[2], argv[3], false);
            spdlog::info("Regresando el archivo {} al commit {}", argv[3], argv[2]);
            //DEVOLVER EL ARCHIVO AL COMMIT ESPECIFICO
        }
    }
    else if (!strcmp(argv[1], "reset"))
    {
        if (argc < 3)
        {
            spdlog::info("Regresando todos los archivos al commit anterior.");
            //DEVOLVER LOS ARCHIVOS AL COMMIT ANTERIOR
        }
        else if (argc == 3)
        {
            Command::reset(argv[2]);
            spdlog::info("Regresando el archivo {} al commit anterior.", argv[2]);
            //DEVOLVER EL ARCHIVO AL COMMIT ANTERIOR
        }
        else
        {
            spdlog::warn("Especifique únicamente un archivo.");
            spdlog::info("Para ver la lista de opciones disponibles utilice el comando: ./got help");
        }
    }
    else if (!strcmp(argv[1], "sync"))
    {
        if (argc != 3)
        {
            spdlog::warn("Especifique un archivo.");
            spdlog::info("Para ver la lista de opciones disponibles utilice el comando: ./got help");
        }
        else
        {
            Command::sync(argv[2]);
            spdlog::info("Sincronizando el archivo {}", argv[2]);
            //SINCRONIZA EL ARCHIVO Y PERMITE EL MERGE
        }
    }
    else if (!strcmp(argv[1], "log"))
    {
        Command::log();
    }
    else if (!strcmp(argv[1], "help"))
    {
        string help = R"(

    LISTA DE COMANDOS DISPONIBLES PARA GOT:

    1. ./got init <nombre> : Inicia el repositorio en la ruta actual y lo identifica con <nombre>.

    2. ./got add -A                    : Agrega todos los archivos locales para el siguiente commit.
       ./got add <archivo1> <archivo2> : Agrega los <archivo>s para el siguiente commit.

    3. ./got commit <mensaje> : Realiza el commit de los archivos agregados con el <mensaje>.

    4. ./got status           : Muestra cuales archivos han sido modificados de acuerdo al commit anterior.
       ./got status <archivo> : Muestra los cambios realizados al <archivo> en base al commit anterior.
        
    5. ./got rollback <commit>           : Permite regresar los archivos al <commit>.
       ./got rollback <commit> <archivo> : Permite regresar el <archivo> al <commit>.

    6. ./got reset           : Deshace cambios locales y los regresa al último commit.
       ./got reset <archivo> : Deshace cambios locales para el <archivo> y lo regresa al último commit.
        
    7. ./got sync <archivo> : Sincroniza el <archivo> en el servidor con el local. Permite realizar un merge.
    )";
        spdlog::info(help);
    }
    else
    {
        spdlog::warn("Comando incorrecto.");
        spdlog::info("Para ver la lista de opciones disponibles utilice el comando: ./got help");
    }

    return 0;
}