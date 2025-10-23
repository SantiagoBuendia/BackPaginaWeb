#include "GestorExamenes.h"
#include <cstdlib> // Para atoi

GestorExamenes::GestorExamenes(MYSQL* conexion) : conexionDB(conexion) {}

void GestorExamenes::registrarExamen(const std::string& titulo, const std::string& descripcion, const std::string& idGrupo,
    const std::string& instrucciones, const std::string& intentosPermitidos, const std::string& idProfesor) {
    std::string query = "INSERT INTO examenes (titulo, descripcion, grupo_id, profesor_id, instrucciones, intentos_permitidos) VALUES ('" +
        titulo + "', '" +
        descripcion + "', '" +
        idGrupo + "', '" +
        idProfesor + "', '" +
        instrucciones + "', '" +
        intentosPermitidos + "')";
    std::cout << "Content-type: application/json\r\n\r\n";
    if (mysql_query(conexionDB, query.c_str()) == 0) {
        my_ulonglong id_examen_generado = mysql_insert_id(conexionDB);
        std::cout << "{ \"id_examen\": " << id_examen_generado << " }";
    }
    else {
        std::string errorMsg = mysql_error(conexionDB);
        std::cout << "{ \"error\": \"" << errorMsg << "\" }";
    }
}

void GestorExamenes::registrarPregunta(const std::string& idExamen, const std::string& pregunta, const std::string& puntos,
    const std::string& explicacion) {
    // Calcular el siguiente orden_pregunta automáticamente
    int orden = 1;
    std::string consultaConteo = "SELECT COUNT(*) FROM preguntas WHERE examen_id = " + idExamen;
    if (mysql_query(conexionDB, consultaConteo.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conexionDB);
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row && row[0]) orden = atoi(row[0]) + 1;
        mysql_free_result(res);
    }

    std::string query = "INSERT INTO preguntas (examen_id, pregunta, puntos, orden_pregunta, explicacion) VALUES ('" +
        idExamen + "', '" + pregunta + "', '" + puntos + "', '" + std::to_string(orden) + "', '" + explicacion + "')";
    std::cout << "Content-type: application/json\r\n\r\n";
    if (mysql_query(conexionDB, query.c_str()) == 0) {
        my_ulonglong id_pregunta_generada = mysql_insert_id(conexionDB);
        std::cout << "{ \"id_pregunta\": " << id_pregunta_generada << " }";
    }
    else {
        std::string errorMsg = mysql_error(conexionDB);
        std::cout << "{ \"error\": \"" << errorMsg << "\" }";
    }
}

void GestorExamenes::registrarOpciones(const cgicc::Cgicc& formulario) {
    std::string id_pregunta = formulario("pregunta_id"); // Asegúrate de que este campo exista en el formulario
    int indice = 1;
    bool algunaOpcionGuardada = false;
    while (true) {
        std::string nombreOpcion = "opcion_" + std::to_string(indice);
        std::string nombreEsCorrecta = "es_correcta_" + std::to_string(indice);

        auto it = formulario.getElement(nombreOpcion);
        if (it == formulario.getElements().end()) break; // Si no hay más opciones, salir del bucle
        std::string textoOpcion = **it;
        bool esCorrecta = formulario.queryCheckbox(nombreEsCorrecta);

        std::string orden = std::to_string(indice);

        std::string query = "INSERT INTO opciones_respuesta (pregunta_id, opcion, es_correcta, orden_opcion) VALUES ('" +
            id_pregunta + "', '" + textoOpcion + "', " + (esCorrecta ? "1" : "0") + ", " + orden + ")";
        if (mysql_query(conexionDB, query.c_str()) == 0) {
            algunaOpcionGuardada = true;
        }
        else {
            std::cerr << "Error al insertar opción: " << mysql_error(conexionDB) << std::endl;
        }
        ++indice;
    }
    std::cout << "Content-type: text/plain\r\n\r\n";
    if (algunaOpcionGuardada)
        std::cout << "Opciones guardadas correctamente.";
    else
        std::cout << "No se recibieron opciones o hubo errores al guardar.";
}