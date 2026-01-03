#ifndef GESTOR_AUDITORIA_H
#define GESTOR_AUDITORIA_H

#include <mysql.h>
#include <string>
#include <iostream>

class GestorAuditoria {
private:
	MYSQL* conexionDB;

public:
	GestorAuditoria(MYSQL* conexion);
	void registrarAuditoria(const std::string& idUsuario, const std::string& tabla, const std::string& accion, const std::string& descripcion);
	void listarAuditorias();
};

#endif 