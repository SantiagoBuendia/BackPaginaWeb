#ifndef GESTOR_USUARIOS_H
#define GESTOR_USUARIOS_H

#include <mysql.h>
#include <string>
#include <cgicc/Cgicc.h>
#include "GestorAuditoria.h" // Se usa GestorAuditoria
#include "Utilidades.h"      // Se usa Utilidades::generarToken

class GestorUsuarios {
private:
    MYSQL* conexionDB;
    GestorAuditoria& gestorAuditoria; // Referencia para registrar auditorías

public:
    GestorUsuarios(MYSQL* conexion, GestorAuditoria& gestorAud);

    void verificarUsuario(const std::string& correo, const std::string& contrasena);
    void registrarUsuario(const std::string& correo, const std::string& contrasena, const std::string& nombre, const std::string& rol, const std::string& idUsuarioAuditor);
    void listarUsuarios();
    void actualizarUsuario(const std::string& id, const std::string& nuevoNombre, const std::string& nuevaContrasena, const std::string& idUsuarioAuditor);
    void eliminarUsuario(const std::string& id, const std::string& idUsuarioAuditor);
};

#endif // GESTOR_USUARIOS_H