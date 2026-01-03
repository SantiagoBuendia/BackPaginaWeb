#ifndef GESTOR_EXAMENES_H
#define GESTOR_EXAMENES_H

#include <mysql.h>
#include <string>
#include <cgicc/Cgicc.h>
#include <iostream>
#include "GestorIntentosExamen.h"

class GestorExamenes {
private:
	MYSQL* conexionDB;

public:
	GestorExamenes(MYSQL* conexion);

	void registrarExamen(const std::string& titulo, const std::string& descripcion, const std::string& idGrupo,
		const std::string& instrucciones, const std::string& intentosPermitidos, const std::string& idProfesor);
	void registrarPregunta(const std::string& idExamen, const std::string& pregunta, const std::string& puntos,
		const std::string& explicacion);
	void registrarOpciones(const cgicc::Cgicc& formulario);

	void listarExamenesPorProfesor(const std::string& idProfesor);

	void eliminarExamen(const std::string& idExamen);

	void mostrarExamen(const std::string& idExamen);

	void formularioEditarExamen(const std::string& idExamen);

	void guardarCambiosExamen(const cgicc::Cgicc& formData);

	void listarExamenesEstudiante(const std::string& idEstudiante);

	void prepararExamenEstudiante(const std::string& idExamen, const std::string& idEstudiante);

	void calificarExamen(cgicc::Cgicc& formulario, GestorIntentosExamen& gestorIntentos);

	void mostrarResultadoIntento(const std::string& idIntento);
};

#endif 