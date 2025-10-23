#ifndef GESTOR_CALIFICACIONES_H
#define GESTOR_CALIFICACIONES_H

#include <mysql.h>
#include <string>
#include <iostream>

class GestorCalificaciones {
private:
    MYSQL* conexionDB;

public:
    GestorCalificaciones(MYSQL* conexion);

    void registrarCalificacionFinal(const std::string& idExamen, const std::string& idEstudiante, long long mejorIntentoId,
        const std::string& calificacionFinal, const std::string& puntosObtenidos, const std::string& puntosTotales,
        const std::string& porcentaje, const std::string& comentarios);

    // Puedes añadir métodos para listar calificaciones, obtener la mejor calificación de un estudiante, etc.
    // void listarCalificacionesEstudiante(const std::string& idEstudiante);
    // void listarCalificacionesExamen(const std::string& idExamen);
};

#endif // GESTOR_CALIFICACIONES_H