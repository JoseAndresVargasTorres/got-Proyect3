#ifndef COMMANDS_H
#define COMMANDS_H

#include <client.hpp>

class Command
{
public:
    static string thisPath;
    static void diff_sync(string fileAfter, string fileBefore, string patch);
    static void diff(string fileAfter, string fileBefore, string result);
    static void applyChanges(string original, string changes);
    static void updateIgnore();
    static string get_selfpath();
    static void init(string repoName);
    static void add(string archivos);
    static void commit(string mensaje);
    static string diff_a_string();
    static void status(string archivo);
    static void log();
    static void rollback(string archivo, string commit, bool ruta_externa);
    static void reset(string archivo);
    static void sync(string archivo);
};

#endif