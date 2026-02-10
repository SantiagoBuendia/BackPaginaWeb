#include "GestorRecursos.h"
#include <iostream>

GestorRecursos::GestorRecursos(MYSQL* conexion) : conexionDB(conexion) {}

void GestorRecursos::listarRecursos() {
	mysql_set_character_set(conexionDB, "utf8");

	std::string query = "SELECT * FROM laboratorioquimica.recursos";
	MYSQL_RES* resultado;
	MYSQL_ROW fila;

	std::cout << "Content-type: text/html; charset=UTF-8\r\n\r\n";

	if (mysql_query(conexionDB, query.c_str()) == 0) {
		resultado = mysql_store_result(conexionDB);

		std::cout << "<div id='tabla-recursos-container'>";
		std::cout << "<table id='tabla-recursos-real'>";
		std::cout << "<thead>";
		std::cout << "<tr><th>ID</th><th>Titulo</th><th>Descripcion</th><th>Categoria</th><th>Tipo</th><th>Autor</th><th>Palabras clave</th><th>Fecha</th><th>Enlace</th><th>Acciones</th></tr>";
		std::cout << "</thead><tbody>";

		while ((fila = mysql_fetch_row(resultado)) != nullptr) {
			std::string id = fila[0] ? fila[0] : "0";

			std::cout << "<tr>";
			for (int i = 0; i < 8; ++i) {
				std::cout << "<td>" << (fila[i] ? fila[i] : "") << "</td>";
			}
			std::cout << "<td>"
				<< "<a href='" << (fila[8] ? fila[8] : "#") << "' target='_blank' class='btn-ver' title='Ver recurso'>"
				<< "<i class='fas fa-eye'></i>"
				<< "</a></td>";
			std::cout << "<td>"
				<< "<a href='/cgi-bin/PaginaWebLaboratorio.exe?accion=eliminarr&id=" << id
				<< "' onclick=\"return confirm('¿Estás seguro de eliminar este recurso?');\" class='link-no-style'>"
				<< "<button class='btn-icon-eliminar' title='Eliminar'>"
				<< "<i class='fas fa-trash-alt'></i>"
				<< "</button></a>"
				<< "</td>";
			std::cout << "</tr>";
		}
		std::cout << "</tbody></table></div>";
		mysql_free_result(resultado);
	}
}

void GestorRecursos::eliminarRecurso(const std::string& id) {
	std::string consulta = "DELETE FROM laboratorioquimica.recursos WHERE id = " + id;

	if (mysql_query(conexionDB, consulta.c_str()) == 0) {
		std::cout << "Status: 302 Found\r\n";
		std::cout << "Location: /PaginaWebLaboratorio/contenidoEducativo.html\r\n\r\n";
	}
	else {
		std::cout << "Content-type: text/html\r\n\r\n";
		std::cout << "<html><body><h2>Error al eliminar el recurso: " << mysql_error(conexionDB) << "</h2></body></html>";
	}
}

void GestorRecursos::registrarRecurso(const std::string& titulo, const std::string& descripcion, const std::string& categoria,
	const std::string& tipo, const std::string& autor, const std::string& palabrasClave,
	const std::string& fecha, const std::string& enlace, cgicc::Cgicc& formulario) {
	std::string enlaceFinal = enlace;

	if (tipo == "guias") {
		cgicc::const_file_iterator archivo = formulario.getFile("enlace");
		if (archivo != formulario.getFiles().end()) {
			std::string nombreArchivo = archivo->getFilename();
			nombreArchivo = Utilidades::sanitizarNombreArchivo(nombreArchivo);

			std::string rutaGuardado = "C:\\Apache24\\htdocs\\PaginaWebLaboratorio\\Guias\\" + nombreArchivo;

			std::ofstream out(rutaGuardado, std::ios::binary);
			if (out.is_open()) {
				archivo->writeToStream(out);
				out.close();

				enlaceFinal = "/PaginaWebLaboratorio/Guias/" + nombreArchivo;
			}
			else {
				std::cout << "Content-type: text/html\r\n\r\n";
				std::cout << "<html><body><h2>Error: No se pudo abrir el archivo para guardar en el servidor. Revise permisos y ruta.</h2></body></html>";
				return;
			}
		}
		else {
			std::cout << "Content-type: text/html\r\n\r\n";
			std::cout << "<html><body><h2>No se recibió archivo para una guía.</h2></body></html>";
			return;
		}
	}

	std::string query = "INSERT INTO laboratorioquimica.recursos (titulo, descripcion, categoria, tipo, autor, palabras_clave, fecha, enlace) VALUES ('" +
		titulo + "', '" +
		descripcion + "', '" +
		categoria + "', '" +
		tipo + "', '" +
		autor + "', '" +
		palabrasClave + "', '" +
		fecha + "', '" +
		enlaceFinal + "')";

	std::cout << "Content-type: text/html\r\n\r\n";

	if (mysql_query(conexionDB, query.c_str()) == 0) {
		std::cout << "<html><head><script>";
		std::cout << "alert('✅ Registro exitoso');";
		std::cout << "window.location.href = '/PaginaWebLaboratorio/contenidoEducativo.html';";
		std::cout << "</script></head><body></body></html>";
	}
	else {
		std::cout << "<html><head><script>";
		std::cout << "alert('❌ Error al registrar: " << mysql_error(conexionDB) << "');";
		std::cout << "window.history.back();";
		std::cout << "</script></head><body></body></html>";
	}
}