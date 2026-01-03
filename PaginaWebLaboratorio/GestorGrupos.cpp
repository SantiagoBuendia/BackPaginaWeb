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

static std::string escape_json(const std::string& s) {
	std::string out;
	out.reserve(s.size());
	for (unsigned char c : s) {
		switch (c) {
		case '"': out += "\\\""; break;
		case '\\': out += "\\\\"; break;
		case '\b': out += "\\b"; break;
		case '\f': out += "\\f"; break;
		case '\n': out += "\\n"; break;
		case '\r': out += "\\r"; break;
		case '\t': out += "\\t"; break;
		default:
			if (c < 0x20) {
				char buf[7];
				std::snprintf(buf, sizeof(buf), "\\u%04x", c);
				out += buf;
			}
			else {
				out += c;
			}
		}
	}
	return out;
}

void GestorGrupos::listarGrupos() {
	const std::string query = "SELECT id, nombre FROM grupos";
	MYSQL_RES* resultado = nullptr;
	MYSQL_ROW fila = nullptr;

	std::cout << "Content-Type: application/json\r\n\r\n";

	if (mysql_query(conexionDB, query.c_str()) != 0) {
		std::cerr << "Error en listarGrupos SQL: " << mysql_error(conexionDB) << std::endl;
		std::cout << "[]";
		return;
	}

	resultado = mysql_store_result(conexionDB);
	if (!resultado) {
		std::cout << "[]";
		return;
	}

	bool primero = true;
	std::cout << "[";
	while ((fila = mysql_fetch_row(resultado)) != nullptr) {
		if (!primero) std::cout << ",";
		const char* colId = fila[0];
		const char* colNombre = fila[1];
		std::string id = colId ? colId : "0";
		std::string nombre = colNombre ? colNombre : "";
		std::cout << "{\"id\":" << id << ",\"nombre\":\"" << escape_json(nombre) << "\"}";
		primero = false;
	}
	std::cout << "]";
	mysql_free_result(resultado);

	return;
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
void GestorGrupos::listarGruposProfesorJSON(const std::string& idProfesor) {
	std::cout << "Content-type: application/json\r\n\r\n";
	std::string query = "SELECT id, nombre FROM grupos WHERE profesor_id = " + idProfesor;

	if (mysql_query(conexionDB, query.c_str()) == 0) {
		MYSQL_RES* res = mysql_store_result(conexionDB);
		MYSQL_ROW fila;
		std::cout << "[";
		bool primero = true;
		while ((fila = mysql_fetch_row(res))) {
			if (!primero) std::cout << ",";
			std::cout << "{\"id\":\"" << fila[0] << "\", \"nombre\":\"" << fila[1] << "\"}";
			primero = false;
		}
		std::cout << "]";
		mysql_free_result(res);
	}
	else {
		std::cout << "[]";
	}
}