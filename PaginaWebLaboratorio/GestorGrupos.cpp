#include "GestorGrupos.h"

GestorGrupos::GestorGrupos(MYSQL* conexion) : conexionDB(conexion) {}

void GestorGrupos::registrarGrupo(const std::string& nombre, const std::string& descripcion, const std::string& idProfesor) {
    std::string query = "INSERT INTO grupos (nombre, descripcion, profesor_id) VALUES ('" +
        nombre + "', '" + descripcion + "',  '" + idProfesor + "')";

    std::cout << "Content-type: text/html\r\n\r\n";

    if (mysql_query(conexionDB, query.c_str()) == 0) {
        std::cout << "<html><head><script>";
        std::cout << "alert('✅ Grupo registrado exitosamente');";
        std::cout << "window.location.href = 'http://localhost/PaginaWebLaboratorio/gestionUsuarios.html';";
        std::cout << "</script></head><body></body></html>";
    }
    else {
        std::cout << "<html><head><script>";
        std::cout << "alert('❌ Error al registrar el grupo: " << mysql_error(conexionDB) << "');";
        std::cout << "window.history.back();";
        std::cout << "</script></head><body></body></html>";
    }
}

void GestorGrupos::registrarEstudianteEnGrupo(const std::string& idGrupo, const std::string& idEstudiante) {
    std::string query = "INSERT INTO grupo_estudiantes (grupo_id, estudiante_id) VALUES ('" +
        idGrupo + "', '" + idEstudiante + "')";
    std::cout << "Content-type: text/html\r\n\r\n";

    if (mysql_query(conexionDB, query.c_str()) == 0) {
        std::cout << "<html><head><script>";
        std::cout << "alert('✅ Estudiante registrado exitosamente en el grupo');";
        std::cout << "window.location.href = 'http://localhost/PaginaWebLaboratorio/gestionUsuarios.html';";
        std::cout << "</script></head><body></body></html>";
    }
    else {
        std::string errorMsg = mysql_error(conexionDB);
        std::cout << "<html><head><script>";
        if (errorMsg.find("Duplicate entry") != std::string::npos) {
            std::cout << "alert('❌ El estudiante ya está asignado a este grupo.');";
        }
        else {
            std::cout << "alert('❌ Error al registrar al grupo: " << errorMsg << "');";
        }
        std::cout << "window.history.back();";
        std::cout << "</script></head><body></body></html>";
    }
}

void GestorGrupos::listarGrupos() {
    std::string query = "SELECT id, nombre FROM grupos";
    MYSQL_RES* resultado;
    MYSQL_ROW fila;

    std::cout << "Content-type: application/json\r\n\r\n";

    if (mysql_query(conexionDB, query.c_str()) == 0) {
        resultado = mysql_store_result(conexionDB);
        std::cout << "[";
        bool primero = true;
        while ((fila = mysql_fetch_row(resultado))) {
            if (!primero) std::cout << ",";
            std::cout << "{";
            std::cout << "\"id\":" << fila[0] << ",";
            std::cout << "\"nombre\":\"" << fila[1] << "\"";
            std::cout << "}";
            primero = false;
        }
        std::cout << "]";
        mysql_free_result(resultado);
    }
    else {
        std::cerr << "Error en la consulta listarGrupos: " << mysql_error(conexionDB) << std::endl;
        std::cout << "[]";
    }
}

void GestorGrupos::listarGruposProfesor(const std::string& idProfesor) {
    std::string query = "SELECT id, nombre FROM grupos WHERE profesor_id = " + idProfesor;
    MYSQL_RES* resultado;
    MYSQL_ROW fila;

    std::cout << "Content-type: application/json\r\n\r\n";

    if (mysql_query(conexionDB, query.c_str()) == 0) {
        resultado = mysql_store_result(conexionDB);
        std::cout << "[";
        bool primero = true;
        while ((fila = mysql_fetch_row(resultado))) {
            if (!primero) std::cout << ",";
            std::cout << "{";
            std::cout << "\"id\":" << fila[0] << ",";
            std::cout << "\"nombre\":\"" << fila[1] << "\"";
            std::cout << "}";
            primero = false;
        }
        std::cout << "]";
        mysql_free_result(resultado);
    }
    else {
        std::cerr << "Error en la consulta listarGruposProfesor: " << mysql_error(conexionDB) << std::endl;
        std::cout << "[]";
    }
}