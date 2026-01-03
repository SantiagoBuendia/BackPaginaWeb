#include "GestorCalificaciones.h"

GestorCalificaciones::GestorCalificaciones(MYSQL* conexion) : conexionDB(conexion) {}

void GestorCalificaciones::registrarCalificacionFinal(const std::string& idExamen, const std::string& idEstudiante, long long mejorIntentoId,
	const std::string& calificacionFinal, const std::string& puntosObtenidos, const std::string& puntosTotales,
	const std::string& porcentaje, const std::string& comentarios) {
	std::string query = "INSERT INTO calificaciones (examen_id, estudiante_id, mejor_intento_id, calificacion_final, puntos_obtenidos, puntos_totales, porcentaje, comentarios) VALUES ('" +
		idExamen + "', '" + idEstudiante + "', '" + std::to_string(mejorIntentoId) + "', '" + calificacionFinal + "', '" + puntosObtenidos + "', '" + puntosTotales + "', '" + porcentaje + "', '" + comentarios + "') "
		"ON DUPLICATE KEY UPDATE "
		"mejor_intento_id = VALUES(mejor_intento_id), "
		"calificacion_final = VALUES(calificacion_final), "
		"puntos_obtenidos = VALUES(puntos_obtenidos), "
		"puntos_totales = VALUES(puntos_totales), "
		"porcentaje = VALUES(porcentaje), "
		"fecha_calificacion = CURRENT_TIMESTAMP, "
		"comentarios = VALUES(comentarios)";

	if (mysql_query(conexionDB, query.c_str()) != 0) {
		std::cerr << "Error al registrar/actualizar calificación final: " << mysql_error(conexionDB) << std::endl;
	}
}
void GestorCalificaciones::obtenerSeguimientoProfesor(const std::string& idProfesor, const std::string& idGrupo) {
	std::cout << "Content-type: application/json\r\n\r\n";

	std::string filtroGrupo = (idGrupo == "todos" || idGrupo == "") ? "" : " AND g.id = " + idGrupo;

	std::string query =
		"SELECT u.id, u.usuario, g.nombre, "
		"COUNT(ie.id) AS total_por_alumno, "
		"AVG(ie.calificacion) AS promedio_alumno "
		"FROM usuarios u "
		"JOIN grupo_estudiantes ge ON u.id = ge.estudiante_id "
		"JOIN grupos g ON ge.grupo_id = g.id "
		"LEFT JOIN intentos_examen ie ON u.id = ie.estudiante_id "
		"WHERE g.profesor_id = " + idProfesor + filtroGrupo + " "
		"GROUP BY u.id, g.id";

	if (mysql_query(conexionDB, query.c_str()) == 0) {
		MYSQL_RES* res = mysql_store_result(conexionDB);
		MYSQL_ROW fila;

		std::cout << "{ \"detalles\": [";
		bool primero = true;
		double sumaPromedios = 0;
		int totalAlumnos = 0;
		int sumaTotalIntentos = 0;

		while ((fila = mysql_fetch_row(res))) {
			if (!primero) std::cout << ",";

			int intentosDeEsteAlumno = fila[3] ? atoi(fila[3]) : 0;
			double promDeEsteAlumno = fila[4] ? atof(fila[4]) : 0.0;

			sumaTotalIntentos += intentosDeEsteAlumno;
			sumaPromedios += promDeEsteAlumno;
			totalAlumnos++;

			std::cout << "{"
				<< "\"id\":" << fila[0] << ","
				<< "\"nombre\":\"" << fila[1] << "\","
				<< "\"grupo\":\"" << fila[2] << "\","
				<< "\"total_evals\":" << intentosDeEsteAlumno << ","
				<< "\"promedio\":" << (promDeEsteAlumno * 10.0)
				<< "}";
			primero = false;
		}

		double promedioGral = (totalAlumnos > 0) ? (sumaPromedios / totalAlumnos) * 10.0 : 0;

		std::cout << "], \"promedio_general\":" << promedioGral
			<< ", \"total_intentos\":" << sumaTotalIntentos << "}";

		mysql_free_result(res);
	}
	else {
		std::cout << "{\"error\":\"" << mysql_error(conexionDB) << "\"}";
	}
}
void GestorCalificaciones::obtenerHistorialAlumno(const std::string& idEstudiante) {
	std::cout << "Content-type: application/json\r\n\r\n";

	std::string nombreAlumno = "Estudiante";
	std::string qNombre = "SELECT usuario FROM usuarios WHERE id = " + idEstudiante;
	mysql_query(conexionDB, qNombre.c_str());
	MYSQL_RES* resN = mysql_store_result(conexionDB);
	if (resN) {
		MYSQL_ROW filaN = mysql_fetch_row(resN);
		if (filaN) nombreAlumno = filaN[0];
		mysql_free_result(resN);
	}

	std::string query =
		"SELECT e.titulo, ie.calificacion, ie.numero_intento, ie.id "
		"FROM intentos_examen ie "
		"JOIN examenes e ON ie.examen_id = e.id "
		"WHERE ie.estudiante_id = " + idEstudiante + " "
		"ORDER BY ie.id DESC";

	if (mysql_query(conexionDB, query.c_str()) != 0) {
		std::cout << "{\"error\": \"" << mysql_error(conexionDB) << "\"}";
		return;
	}

	MYSQL_RES* res = mysql_store_result(conexionDB);
	std::cout << "{ \"nombre\": \"" << nombreAlumno << "\", \"intentos\": [";

	if (res) {
		MYSQL_ROW fila;
		bool primero = true;
		while ((fila = mysql_fetch_row(res))) {
			if (!primero) std::cout << ",";
			std::cout << "{"
				<< "\"examen\":\"" << (fila[0] ? fila[0] : "Sin titulo") << "\","
				<< "\"nota\":" << (fila[1] ? fila[1] : "0") << ","
				<< "\"n_intento\":" << (fila[2] ? fila[2] : "0") << ","
				<< "\"id_intento\":" << (fila[3] ? fila[3] : "0")
				<< "}";
			primero = false;
		}
		mysql_free_result(res);
	}
	std::cout << "] }";
}