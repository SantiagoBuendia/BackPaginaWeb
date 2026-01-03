#include "GestorBaseDeDatos.h"

GestorBaseDeDatos::GestorBaseDeDatos(const char* servidor, const char* usuario_db, const char* contrasena_db, const char* base_de_datos, unsigned int puerto)
	: servidor(servidor), usuario_db(usuario_db), contrasena_db(contrasena_db), base_de_datos(base_de_datos), puerto(puerto), conexion(nullptr) {
}

GestorBaseDeDatos::~GestorBaseDeDatos() {
	desconectar();
}

bool GestorBaseDeDatos::conectar() {
	conexion = mysql_init(0);
	if (!conexion) {
		std::cerr << "mysql_init falló\n";
		return false;
	}
	conexion = mysql_real_connect(conexion, servidor, usuario_db, contrasena_db, base_de_datos, puerto, NULL, 0);
	if (!conexion) {
		std::cerr << "Fallo al conectar a la base de datos: " << mysql_error(conexion) << "\n";
		return false;
	}
	return true;
}

MYSQL* GestorBaseDeDatos::getConexion() const {
	return conexion;
}

void GestorBaseDeDatos::desconectar() {
	if (conexion) {
		mysql_close(conexion);
		conexion = nullptr;
	}
}