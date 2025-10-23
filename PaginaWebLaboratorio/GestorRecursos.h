#ifndef GESTOR_RECURSOS_H
#define GESTOR_RECURSOS_H

#include <mysql.h>
#include <string>
#include <cgicc/Cgicc.h>
#include <fstream> // Para manejar archivos
#include "Utilidades.h" // Se usa Utilidades::sanitizarNombreArchivo

class GestorRecursos {
private:
    MYSQL* conexionDB;

public:
    GestorRecursos(MYSQL* conexion);

    void listarRecursos();
    void eliminarRecurso(const std::string& id);
    void registrarRecurso(const std::string& titulo, const std::string& descripcion, const std::string& categoria,
        const std::string& tipo, const std::string& autor, const std::string& palabrasClave,
        const std::string& fecha, const std::string& enlace, cgicc::Cgicc& formulario); // Se pasa formulario para manejar subidas de archivo
};

#endif // GESTOR_RECURSOS_H