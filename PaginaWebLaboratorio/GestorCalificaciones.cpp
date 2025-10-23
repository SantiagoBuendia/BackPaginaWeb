#include "GestorCalificaciones.h"

GestorCalificaciones::GestorCalificaciones(MYSQL* conexion) : conexionDB(conexion) {}

void GestorCalificaciones::registrarCalificacionFinal(const std::string& idExamen, const std::string& idEstudiante, long long mejorIntentoId,
    const std::string& calificacionFinal, const std::string& puntosObtenidos, const std::string& puntosTotales,
    const std::string& porcentaje, const std::string& comentarios) {

    // Usar INSERT ... ON DUPLICATE KEY UPDATE para actualizar si la calificación ya existe
    std::string query = "INSERT INTO calificaciones (examen_id, estudiante_id, mejor_intento_id, calificacion_final, puntos_obtenidos, puntos_totales, porcentaje, comentarios) VALUES ('" +
        idExamen + "', '" + idEstudiante + "', '" + std::to_string(mejorIntentoId) + "', '" + calificacionFinal + "', '" + puntosObtenidos + "', '" + puntosTotales + "', '" + porcentaje + "', '" + comentarios + "') "
        "ON DUPLICATE KEY UPDATE "
        "mejor_intento_id = VALUES(mejor_intento_id), "
        "calificacion_final = VALUES(calificacion_final), "
        "puntos_obtenidos = VALUES(puntos_obtenidos), "
        "puntos_totales = VALUES(puntos_totales), "
        "porcentaje = VALUES(porcentaje), "
        "fecha_calificacion = CURRENT_TIMESTAMP, " // Actualizar la fecha de calificación
        "comentarios = VALUES(comentarios)";

    if (mysql_query(conexionDB, query.c_str()) != 0) {
        std::cerr << "Error al registrar/actualizar calificación final: " << mysql_error(conexionDB) << std::endl;
        // Podrías imprimir un alert en HTML si es un error crítico visible al usuario
    }
}