#ifndef GESTOR_BASE_DE_DATOS_H
#define GESTOR_BASE_DE_DATOS_H

#include <mysql.h>
#include <string>
#include <iostream>

class GestorBaseDeDatos {
private:
	MYSQL* conexion;
	const char* servidor;
	const char* usuario_db;
	const char* contrasena_db;
	const char* base_de_datos;
	unsigned int puerto;

public:
	GestorBaseDeDatos(const char* servidor, const char* usuario_db, const char* contrasena_db, const char* base_de_datos, unsigned int puerto = 3306);
	~GestorBaseDeDatos();

	bool conectar();
	MYSQL* getConexion() const;
	void desconectar();
};

#endif 