#ifndef GESTOR_SIMULACION_H
#define GESTOR_SIMULACION_H

#include <mysql.h>
#include <string>
#include <iostream>

class GestorSimulacion {
private:
	MYSQL* conexionDB;
	std::string escaparSQL(const std::string& input);
	std::string escaparJSON(const std::string& input);

public:
	GestorSimulacion(MYSQL* conexion);

	int iniciarSimulacion(
		const std::string& usuarioId,
		const std::string& nombre,
		const std::string& descripcion,
		const std::string& dispositivo
	);

	void registrarResultado(
		const std::string& simulacionId,
		const std::string& variable,
		const std::string& valor,
		const std::string& unidad
	);

	void registrarEvento(
		const std::string& simulacionId,
		const std::string& evento,
		const std::string& detalle,
		const std::string& tiempo
	);

	void finalizarSimulacion(
		const std::string& simulacionId,
		const std::string& duracion
	);

	void obtenerSeguimientoCompleto(
		const std::string& filtroUsuario,
		const std::string& filtroExp
	);

	void obtenerSeguimientoEstudiante(
		const std::string& idEstudiante,
		const std::string& filtroExp
	);

	void obtenerDetallesSimulacion(const std::string& idSim);
};

#endif