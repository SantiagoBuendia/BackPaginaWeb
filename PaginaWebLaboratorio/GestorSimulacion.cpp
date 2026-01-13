#include "GestorSimulacion.h"
#include <sstream>
#include <cctype>
#include <cstdio>

GestorSimulacion::GestorSimulacion(MYSQL* conexion) {
	conexionDB = conexion;
}

std::string GestorSimulacion::escaparSQL(const std::string& input) {
	if (conexionDB == nullptr || input.empty()) return input;

	char* buffer = new char[input.length() * 2 + 1];
	mysql_real_escape_string(conexionDB, buffer, input.c_str(), input.length());
	std::string resultado(buffer);
	delete[] buffer;
	return resultado;
}

std::string GestorSimulacion::escaparJSON(const std::string& input) {
	std::string output;

	for (size_t i = 0; i < input.length(); i++) {
		unsigned char c = input[i];

		switch (c) {
		case '"':  output += "\\\""; break;
		case '\\': output += "\\\\"; break;
		case '\b': output += "\\b"; break;
		case '\f': output += "\\f"; break;
		case '\n': output += "\\n"; break;
		case '\r': output += "\\r"; break;
		case '\t': output += "\\t"; break;
		default:
			if (c < 32) {
				char buf[7];
				snprintf(buf, sizeof(buf), "\\u%04x", c);
				output += buf;
			}
			else {
				output += c;
			}
		}
	}

	return output;
}

int GestorSimulacion::iniciarSimulacion(
	const std::string& usuarioId,
	const std::string& nombre,
	const std::string& descripcion,
	const std::string& dispositivo
) {
	int idNumerico = 0;
	try {
		idNumerico = std::stoi(usuarioId);
	}
	catch (...) {
		idNumerico = 0;
	}
	std::string uidLimpio = std::to_string(idNumerico);

	std::string query =
		"INSERT INTO simulaciones "
		"(usuario_id, nombre, descripcion, fecha_inicio, dispositivo) "
		"VALUES (" +
		uidLimpio + ", '" + nombre + "', '" + descripcion + "', NOW(), '" + dispositivo + "')";

	std::cerr << "SQL QUERY: " << query << std::endl;

	if (mysql_query(conexionDB, query.c_str())) {
		std::cerr << "Error iniciarSimulacion: " << mysql_error(conexionDB) << std::endl;
		return -1;
	}

	return mysql_insert_id(conexionDB);
}

void GestorSimulacion::registrarResultado(
	const std::string& simulacionId,
	const std::string& variable,
	const std::string& valor,
	const std::string& unidad
) {
	std::string query =
		"INSERT INTO resultados_simulacion "
		"(simulacion_id, variable, valor, unidad) VALUES (" +
		simulacionId + ", '" + variable + "', '" + valor + "', '" + unidad + "')";

	if (mysql_query(conexionDB, query.c_str())) {
		std::cerr << "Error registrarResultado: " << mysql_error(conexionDB) << std::endl;
	}
}

void GestorSimulacion::registrarEvento(
	const std::string& simulacionId,
	const std::string& evento,
	const std::string& detalle,
	const std::string& tiempo
) {
	std::string query =
		"INSERT INTO eventos_simulacion "
		"(simulacion_id, evento, detalle, tiempo_segundos) VALUES (" +
		simulacionId + ", '" + evento + "', '" + detalle + "', " + tiempo + ")";

	if (mysql_query(conexionDB, query.c_str())) {
		std::cerr << "Error registrarEvento: " << mysql_error(conexionDB) << std::endl;
	}
}

void GestorSimulacion::finalizarSimulacion(
	const std::string& simulacionId,
	const std::string& duracion
) {
	std::string query =
		"UPDATE simulaciones SET "
		"fecha_fin = NOW(), "
		"duracion_segundos = " + duracion +
		" WHERE id = " + simulacionId;

	if (mysql_query(conexionDB, query.c_str())) {
		std::cerr << "Error finalizarSimulacion: " << mysql_error(conexionDB) << std::endl;
	}
}
void GestorSimulacion::obtenerSeguimientoCompleto(
	const std::string& filtroUsuario,
	const std::string& filtroExp
) {
	std::cout << "Content-type: application/json; charset=utf-8\r\n\r\n";

	if (conexionDB == nullptr) {
		std::cout << "{\"error\": \"Sin conexión a la base de datos\"}";
		return;
	}

	std::string query = "SELECT s.id, u.correo, s.nombre, s.fecha_inicio, s.duracion_segundos, s.dispositivo "
		"FROM simulaciones s "
		"JOIN usuarios u ON s.usuario_id = u.id";

	bool tieneFiltro = false;

	if (!filtroUsuario.empty() || !filtroExp.empty()) {
		query += " WHERE ";

		if (!filtroUsuario.empty()) {
			std::string filtroEscapado = escaparSQL(filtroUsuario);
			query += "u.correo LIKE '%" + filtroEscapado + "%'";
			tieneFiltro = true;
		}

		if (!filtroExp.empty()) {
			if (tieneFiltro) query += " AND ";
			std::string filtroEscapado = escaparSQL(filtroExp);
			query += "s.nombre LIKE '%" + filtroEscapado + "%'";
		}
	}

	query += " ORDER BY s.fecha_inicio DESC LIMIT 1000";

	if (mysql_query(conexionDB, query.c_str())) {
		std::string error = escaparJSON(mysql_error(conexionDB));
		std::cout << "{\"error\": \"Error SQL: " << error << "\"}";
		return;
	}

	MYSQL_RES* res = mysql_store_result(conexionDB);
	if (res == nullptr) {
		std::cout << "{\"error\": \"Error obteniendo resultados\"}";
		return;
	}

	std::cout << "[";

	MYSQL_ROW row;
	bool primero = true;

	while ((row = mysql_fetch_row(res))) {
		if (!primero) std::cout << ",";

		std::cout << "{"
			<< "\"id\":\"" << escaparJSON(row[0] ? row[0] : "") << "\","
			<< "\"estudiante\":\"" << escaparJSON(row[1] ? row[1] : "Anónimo") << "\","
			<< "\"experimento\":\"" << escaparJSON(row[2] ? row[2] : "") << "\","
			<< "\"fecha\":\"" << escaparJSON(row[3] ? row[3] : "") << "\","
			<< "\"duracion\":\"" << escaparJSON(row[4] ? row[4] : "0") << "\","
			<< "\"dispositivo\":\"" << escaparJSON(row[5] ? row[5] : "PC") << "\""
			<< "}";

		primero = false;
	}

	std::cout << "]";

	mysql_free_result(res);
}

void GestorSimulacion::obtenerSeguimientoEstudiante(
	const std::string& idEstudiante,
	const std::string& filtroExp
) {
	std::cout << "Content-type: application/json; charset=utf-8\r\n\r\n";

	if (conexionDB == nullptr) {
		std::cout << "{\"error\": \"Sin conexión a la base de datos\"}";
		return;
	}

	if (idEstudiante.empty()) {
		std::cout << "{\"error\": \"ID de estudiante vacío\"}";
		return;
	}

	for (char c : idEstudiante) {
		if (!isdigit(c)) {
			std::cout << "{\"error\": \"ID de estudiante inválido\"}";
			return;
		}
	}

	std::string query = "SELECT s.id, u.correo, s.nombre, s.fecha_inicio, s.duracion_segundos, s.dispositivo "
		"FROM simulaciones s "
		"JOIN usuarios u ON s.usuario_id = u.id "
		"WHERE s.usuario_id = " + idEstudiante;

	if (!filtroExp.empty()) {
		std::string filtroEscapado = escaparSQL(filtroExp);
		query += " AND s.nombre LIKE '%" + filtroEscapado + "%'";
	}

	query += " ORDER BY s.fecha_inicio DESC LIMIT 100";

	if (mysql_query(conexionDB, query.c_str())) {
		std::string error = escaparJSON(mysql_error(conexionDB));
		std::cout << "{\"error\": \"Error SQL: " << error << "\"}";
		return;
	}

	MYSQL_RES* res = mysql_store_result(conexionDB);
	if (res == nullptr) {
		std::cout << "{\"error\": \"Error obteniendo resultados\"}";
		return;
	}

	std::cout << "[";

	MYSQL_ROW row;
	bool primero = true;

	while ((row = mysql_fetch_row(res))) {
		if (!primero) std::cout << ",";

		std::cout << "{"
			<< "\"id\":\"" << escaparJSON(row[0] ? row[0] : "") << "\","
			<< "\"estudiante\":\"" << escaparJSON(row[1] ? row[1] : "Anónimo") << "\","
			<< "\"experimento\":\"" << escaparJSON(row[2] ? row[2] : "") << "\","
			<< "\"fecha\":\"" << escaparJSON(row[3] ? row[3] : "") << "\","
			<< "\"duracion\":\"" << escaparJSON(row[4] ? row[4] : "0") << "\","
			<< "\"dispositivo\":\"" << escaparJSON(row[5] ? row[5] : "PC") << "\""
			<< "}";

		primero = false;
	}

	std::cout << "]";

	mysql_free_result(res);
}

void GestorSimulacion::obtenerDetallesSimulacion(const std::string& idSim) {
	std::cout << "Content-type: application/json; charset=utf-8\r\n\r\n";

	if (conexionDB == nullptr) {
		std::cout << "{\"error\": \"Sin conexión a la base de datos\"}";
		return;
	}

	if (idSim.empty()) {
		std::cout << "{\"error\": \"ID vacío\"}";
		return;
	}

	for (char c : idSim) {
		if (!isdigit(c)) {
			std::cout << "{\"error\": \"ID inválido\"}";
			return;
		}
	}

	std::string idEscapado = escaparSQL(idSim);

	std::string qRes = "SELECT variable, valor, unidad FROM resultados_simulacion WHERE simulacion_id = " + idEscapado;

	if (mysql_query(conexionDB, qRes.c_str())) {
		std::cout << "{\"error\": \"Error SQL en resultados\"}";
		return;
	}

	MYSQL_RES* resRes = mysql_store_result(conexionDB);
	if (!resRes) {
		std::cout << "{\"error\": \"Error obteniendo resultados\"}";
		return;
	}

	std::cout << "{\"resultados\": [";

	MYSQL_ROW row;
	bool primero = true;

	while ((row = mysql_fetch_row(resRes))) {
		if (!primero) std::cout << ",";

		std::cout << "{"
			<< "\"var\":\"" << escaparJSON(row[0] ? row[0] : "") << "\","
			<< "\"val\":\"" << escaparJSON(row[1] ? row[1] : "") << "\","
			<< "\"uni\":\"" << escaparJSON(row[2] ? row[2] : "") << "\""
			<< "}";

		primero = false;
	}

	mysql_free_result(resRes);

	std::cout << "],";

	std::string qEve = "SELECT evento, detalle, tiempo_segundos FROM eventos_simulacion WHERE simulacion_id = "
		+ idEscapado + " ORDER BY tiempo_segundos ASC";

	if (mysql_query(conexionDB, qEve.c_str())) {
		std::cout << "\"eventos\": []}";
		return;
	}

	MYSQL_RES* resEve = mysql_store_result(conexionDB);
	if (!resEve) {
		std::cout << "\"eventos\": []}";
		return;
	}

	std::cout << "\"eventos\": [";

	primero = true;

	while ((row = mysql_fetch_row(resEve))) {
		if (!primero) std::cout << ",";

		std::cout << "{"
			<< "\"nom\":\"" << escaparJSON(row[0] ? row[0] : "") << "\","
			<< "\"det\":\"" << escaparJSON(row[1] ? row[1] : "") << "\","
			<< "\"t\":\"" << escaparJSON(row[2] ? row[2] : "0") << "\""
			<< "}";

		primero = false;
	}

	mysql_free_result(resEve);

	std::cout << "]}";
}