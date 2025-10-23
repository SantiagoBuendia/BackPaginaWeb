#include "GestorIntentosExamen.h"
#include <iostream>

GestorIntentosExamen::GestorIntentosExamen(MYSQL* conexion) : conexionDB(conexion) {}

long long GestorIntentosExamen::iniciarIntento(const std::string& idExamen, const std::string& idEstudiante) {
    // Primero, determinar el número de intento
    int numeroIntento = 1;
    std::string queryConteoIntentos = "SELECT MAX(numero_intento) FROM intentos_examen WHERE examen_id = " + idExamen + " AND estudiante_id = " + idEstudiante;
    if (mysql_query(conexionDB, queryConteoIntentos.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conexionDB);
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row && row[0]) {
            numeroIntento = std::stoi(row[0]) + 1;
        }
        mysql_free_result(res);
    }

    std::string query = "INSERT INTO intentos_examen (examen_id, estudiante_id, numero_intento, completado) VALUES ('" +
        idExamen + "', '" + idEstudiante + "', " + std::to_string(numeroIntento) + ", FALSE)";

    if (mysql_query(conexionDB, query.c_str()) == 0) {
        return static_cast<long long>(mysql_insert_id(conexionDB));
    }
    else {
        std::cerr << "Error al iniciar intento de examen: " << mysql_error(conexionDB) << std::endl;
        return -1; // Indicar error
    }
}

void GestorIntentosExamen::registrarRespuesta(long long idIntento, const std::string& idPregunta, const std::string& idOpcionSeleccionada, bool esCorrecta, const std::string& puntosObtenidos) {
    std::string query = "INSERT INTO respuestas_estudiante (intento_id, pregunta_id, opcion_seleccionada_id, es_correcta, puntos_obtenidos) VALUES ('" +
        std::to_string(idIntento) + "', '" + idPregunta + "', '" + idOpcionSeleccionada + "', " + (esCorrecta ? "TRUE" : "FALSE") + ", '" + puntosObtenidos + "')";

    if (mysql_query(conexionDB, query.c_str()) != 0) {
        std::cerr << "Error al registrar respuesta del estudiante: " << mysql_error(conexionDB) << std::endl;
    }
}

void GestorIntentosExamen::finalizarIntento(long long idIntento, const std::string& calificacion, const std::string& puntosObtenidos, const std::string& puntosTotales) {
    std::string query = "UPDATE intentos_examen SET calificacion = '" + calificacion + "', puntos_obtenidos = '" + puntosObtenidos + "', puntos_totales = '" + puntosTotales + "', completado = TRUE WHERE id = " + std::to_string(idIntento);

    if (mysql_query(conexionDB, query.c_str()) != 0) {
        std::cerr << "Error al finalizar intento de examen: " << mysql_error(conexionDB) << std::endl;
    }
}