#ifndef GESTOR_GRUPOS_H
#define GESTOR_GRUPOS_H

#include <mysql.h>
#include <string>
#include <iostream>

class GestorGrupos {
private:
    MYSQL* conexionDB;

public:
    GestorGrupos(MYSQL* conexion);

    void registrarGrupo(const std::string& nombre, const std::string& descripcion, const std::string& idProfesor);
    void registrarEstudianteEnGrupo(const std::string& idGrupo, const std::string& idEstudiante);
    void listarGrupos(); // Para administradores, listar todos los grupos
    void listarGruposProfesor(const std::string& idProfesor); // Para profesores, listar sus grupos
};

#endif // GESTOR_GRUPOS_H