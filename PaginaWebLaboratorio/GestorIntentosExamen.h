#ifndef GESTOR_INTENTOS_EXAMEN_H
#define GESTOR_INTENTOS_EXAMEN_H

#include <mysql.h>
#include <string>
#include <vector>

class GestorIntentosExamen {
private:
	MYSQL* conexionDB;

public:
	GestorIntentosExamen(MYSQL* conexion);

	long long iniciarIntento(const std::string& idExamen, const std::string& idEstudiante);

	void registrarRespuesta(long long idIntento, const std::string& idPregunta, const std::string& idOpcionSeleccionada, bool esCorrecta, const std::string& puntosObtenidos);

	void finalizarIntento(long long idIntento, const std::string& calificacion, const std::string& puntosObtenidos, const std::string& puntosTotales);
};

#endif 