#ifndef GESTOR_USUARIOS_H
#define GESTOR_USUARIOS_H

#include <mysql.h>
#include <string>
#include <cgicc/Cgicc.h>
#include "GestorAuditoria.h"
#include "Utilidades.h"

class GestorUsuarios {
private:
	MYSQL* conexionDB;
	GestorAuditoria& gestorAuditoria;

public:
	GestorUsuarios(MYSQL* conexion, GestorAuditoria& gestorAud);

	void verificarUsuario(const std::string& correo, const std::string& contrasena);
	void registrarUsuario(const std::string& correo, const std::string& contrasena, const std::string& nombre, const std::string& rol, const std::string& idUsuarioAuditor);
	void listarUsuarios();
	void actualizarUsuario(const std::string& id, const std::string& nuevoNombre, const std::string& nuevaContrasena, const std::string& idUsuarioAuditor, const std::string& rol);
	void eliminarUsuario(const std::string& id, const std::string& idUsuarioAuditor);
	void verificarUsuarioVR(const std::string& correo, const std::string& contrasena);
};

#endif 