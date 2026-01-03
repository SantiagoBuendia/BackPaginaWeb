#include "GestorUsuarios.h"
#include <iostream>
#include <sstream>
#include <fstream>

GestorUsuarios::GestorUsuarios(MYSQL* conexion, GestorAuditoria& gestorAud)
	: conexionDB(conexion), gestorAuditoria(gestorAud) {
}

void GestorUsuarios::verificarUsuario(const std::string& correo, const std::string& contrasena) {
	std::string consulta = "SELECT rol, id, usuario FROM usuarios WHERE correo='" + correo + "' AND contrasena='" + contrasena + "'";
	int estado = mysql_query(conexionDB, consulta.c_str());

	if (!estado) {
		MYSQL_RES* resultado = mysql_store_result(conexionDB);
		MYSQL_ROW fila = mysql_fetch_row(resultado);

		if (fila) {
			std::string rol = fila[0];
			std::string id = fila[1];
			std::string usuario = fila[2];
			std::string token = Utilidades::generarToken();

			std::cout << "Content-type: text/html\r\n";
			std::cout << "Set-Cookie: token=" << token << "; Path=/\r\n";
			std::cout << "Set-Cookie: usuario=" << usuario << "; Path=/\r\n";
			std::cout << "Set-Cookie: rol=" << rol << "; Path=/\r\n";
			std::cout << "Set-Cookie: id=" << id << "; Path=/\r\n";
			std::cout << "Set-Cookie: correo=" << correo << "; Path=/\r\n";
			std::cout << "Set-Cookie: contrasena=" << contrasena << "; path=/\r\n";
			std::cout << "\r\n";

			std::string destino;
			if (rol == "administrador")
				destino = "administrador.html";
			else if (rol == "estudiante")
				destino = "estudiante.html";
			else if (rol == "profesor")
				destino = "profesor.html";
			else
				destino = "";

			if (!destino.empty()) {
				std::cout << "<html><head>";
				std::cout << "<meta http-equiv='refresh' content='0;URL=http://localhost/PaginaWebLaboratorio/" << destino << "'>";
				std::cout << "</head><body></body></html>";
			}
			else {
				std::cout << "<html><body><h2>Rol no reconocido</h2></body></html>";
			}
		}
		else {
			std::cout << "Content-type: text/html\r\n\r\n";
			std::cout << "<html><head><meta charset='UTF-8'></head><body>";
			std::cout << "<script type='text/javascript'>";
			std::cout << "alert('Usuario o contraseña incorrectos. Por favor, intentelo de nuevo.');";
			std::cout << "window.location.href = 'http://localhost/PaginaWebLaboratorio/index.html';";
			std::cout << "</script>";
			std::cout << "</body></html>";
		}
		mysql_free_result(resultado);
	}
	else {
		std::cout << "Content-type: text/html\r\n\r\n";
		std::cout << "<html><body><h2>Error en la consulta: " << mysql_error(conexionDB) << "</h2></body></html>";
	}
}

void GestorUsuarios::registrarUsuario(const std::string& correo, const std::string& contrasena, const std::string& nombre, const std::string& rol, const std::string& idUsuarioAuditor) {
	std::string query = "INSERT INTO usuarios (usuario, contrasena, correo, rol) VALUES ('" +
		nombre + "', '" + contrasena + "', '" + correo + "', '" + rol + "')";

	std::cout << "Content-type: text/html\r\n\r\n";

	if (mysql_query(conexionDB, query.c_str()) == 0) {
		gestorAuditoria.registrarAuditoria(idUsuarioAuditor, "usuarios", "INSERT", "Registro el usuario " + nombre + " con rol " + rol);
		std::cout << "<html><head><script>";
		std::cout << "alert('✅ Registro exitoso');";
		std::cout << "window.location.href = 'http://localhost/PaginaWebLaboratorio/gestionUsuarios.html';";
		std::cout << "</script></head><body></body></html>";
	}
	else {
		unsigned int errorNum = mysql_errno(conexionDB);

		std::cout << "<html><head><script>";

		if (errorNum == 1062) {
			std::cout << "alert('⚠️ Error: El correo \"" << correo << "\" ya se encuentra registrado. Intente con otro.');";
		}
		else {
			std::cout << "alert('❌ Error técnico al registrar: " << mysql_error(conexionDB) << "');";
		}

		std::cout << "window.history.back();";
		std::cout << "</script></head><body></body></html>";
	}
}

void GestorUsuarios::listarUsuarios() {
	std::string query = "SELECT id, usuario, correo, rol FROM laboratorioquimica.usuarios";
	MYSQL_RES* resultado;
	MYSQL_ROW fila;
	std::cout << "Content-type: text/html\r\n\r\n";

	if (mysql_query(conexionDB, query.c_str()) == 0) {
		resultado = mysql_store_result(conexionDB);

		std::cout << "<table border='1' style='width:100%; border-collapse: collapse;'>";
		std::cout << "<thead><tr><th>ID</th><th>Usuario</th><th>Correo</th><th>Rol</th><th>Acciones</th><th>Grupos</th></tr></thead><tbody>";

		while ((fila = mysql_fetch_row(resultado)) != nullptr) {
			std::string id = fila[0];
			std::string usuario = fila[1];
			std::string correo = fila[2];
			std::string rol = fila[3];

			std::cout << "<tr>";
			std::cout << "<td>" << id << "</td>";
			std::cout << "<td>" << usuario << "</td>";
			std::cout << "<td>" << correo << "</td>";
			std::cout << "<td>" << rol << "</td>";

			std::cout << "<td>"
				<< "<a href='http://localhost/PaginaWebLaboratorio/actualizarUsuario.html?id=" << id
				<< "&nombre=" << usuario
				<< "&correo=" << correo
				<< "&rol=" << rol
				<< "'>"
				<< "<button>Actualizar</button></a> ";

			std::cout << "<a href='#' onclick=\"return confirmarEliminar(" << id << ")\">"
				<< "<button>Eliminar</button></a>"
				<< "</td>";
			if (rol == "profesor") {
				std::cout << "<td>"
					<< "<a href='http://localhost/PaginaWebLaboratorio/asignarCursoP.html?id=" << id
					<< "&nombre=" << usuario
					<< "&rol=" << rol
					<< "'>"
					<< "<button>Asignar grupo</button></a>"
					<< "</td>";
			}
			else if (rol == "estudiante") {
				std::cout << "<td>"
					<< "<a href='http://localhost/PaginaWebLaboratorio/asignarCursoE.html?id=" << id
					<< "&nombre=" << usuario
					<< "&rol=" << rol
					<< "'>"
					<< "<button>Asignar grupo</button></a>"
					<< "</td>";
			}
			else {
				std::cout << "<td>No aplica</td>";
			}
			std::cout << "</tr>";
		}
		std::cout << "</tbody></table>";
		mysql_free_result(resultado);
	}
	else {
		std::cout << "<p>Error al ejecutar la consulta SQL: " << mysql_error(conexionDB) << "</p>";
	}
}

void GestorUsuarios::actualizarUsuario(const std::string& id, const std::string& nuevoNombre, const std::string& nuevaContrasena, const std::string& idUsuarioAuditor, const std::string& rol) {
	std::string consulta = "UPDATE usuarios SET usuario='" + nuevoNombre + "', rol='" + rol + "'";

	if (!nuevaContrasena.empty()) {
		consulta += ", contrasena='" + nuevaContrasena + "'";
	}

	consulta += " WHERE id='" + id + "'";

	std::cout << "Content-type: text/html\r\n\r\n";

	if (mysql_query(conexionDB, consulta.c_str()) == 0) {
		gestorAuditoria.registrarAuditoria(idUsuarioAuditor, "usuarios", "UPDATE", "Actualizo el usuario " + nuevoNombre + " (ID: " + id + ")");
		std::cout << "<html><head><script>";

		if (id == idUsuarioAuditor) {
			std::cout << "document.cookie = 'usuario=" << nuevoNombre << "; path=/';";
		}

		std::cout << "alert('✅ Actualización exitosa');";

		if (id != idUsuarioAuditor) {
			std::cout << "window.location.href = 'http://localhost/PaginaWebLaboratorio/gestionUsuarios.html';";
		}
		else {
			std::cout << "const rol = '" << rol << "';";
			std::cout << "if (rol === 'administrador') {";
			std::cout << "    window.location.href = 'http://localhost/PaginaWebLaboratorio/administrador.html';";
			std::cout << "} else if (rol === 'profesor') {";
			std::cout << "    window.location.href = 'http://localhost/PaginaWebLaboratorio/profesor.html';";
			std::cout << "} else if (rol === 'estudiante') {";
			std::cout << "    window.location.href = 'http://localhost/PaginaWebLaboratorio/estudiante.html';";
			std::cout << "} else {";
			std::cout << "    window.location.href = 'http://localhost/PaginaWebLaboratorio/index.html';";
			std::cout << "}";
		}

		std::cout << "</script></head><body></body></html>";
	}
	else {
		std::string errorMsg = mysql_error(conexionDB);

		for (size_t i = 0; i < errorMsg.length(); i++) {
			if (errorMsg[i] == '\'') errorMsg.replace(i, 1, "\\'");
		}
		std::cout << "<html><head><script>";
		std::cout << "alert('❌ Error al actualizar: " << errorMsg << "');";
		std::cout << "window.history.back();";
		std::cout << "</script></head><body></body></html>";
	}
}

void GestorUsuarios::eliminarUsuario(const std::string& id, const std::string& idUsuarioAuditor) {
	std::string nombreUsuarioEliminado = "Desconocido";
	std::string queryNombre = "SELECT usuario FROM usuarios WHERE id = " + id;
	MYSQL_RES* resNombre;
	if (mysql_query(conexionDB, queryNombre.c_str()) == 0) {
		resNombre = mysql_store_result(conexionDB);
		MYSQL_ROW rowNombre = mysql_fetch_row(resNombre);
		if (rowNombre && rowNombre[0]) {
			nombreUsuarioEliminado = rowNombre[0];
		}
		mysql_free_result(resNombre);
	}

	std::string consulta = "DELETE FROM laboratorioquimica.usuarios WHERE id = " + id;

	if (mysql_query(conexionDB, consulta.c_str()) == 0) {
		std::string descripcion = "Elimino el usuario " + nombreUsuarioEliminado + " con ID " + id;
		gestorAuditoria.registrarAuditoria(idUsuarioAuditor, "usuarios", "DELETE", descripcion);
		std::cout << "Status: 302 Found\r\n";
		std::cout << "Location: http://localhost/PaginaWebLaboratorio/gestionUsuarios.html\r\n\r\n";
	}
	else {
		std::cout << "Content-type: text/html\r\n\r\n";
		std::cout << "<html><body><h2>Error al eliminar el usuario: " << mysql_error(conexionDB) << "</h2></body></html>";
	}
}