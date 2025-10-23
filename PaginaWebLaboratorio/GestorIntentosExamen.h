#ifndef GESTOR_INTENTOS_EXAMEN_H
#define GESTOR_INTENTOS_EXAMEN_H

#include <mysql.h>
#include <string>
#include <vector> // Para futuras funciones de recuperación de datos

class GestorIntentosExamen {
private:
    MYSQL* conexionDB;

public:
    GestorIntentosExamen(MYSQL* conexion);

    // Método para iniciar un intento de examen
    long long iniciarIntento(const std::string& idExamen, const std::string& idEstudiante);

    // Método para registrar una respuesta de estudiante
    void registrarRespuesta(long long idIntento, const std::string& idPregunta, const std::string& idOpcionSeleccionada, bool esCorrecta, const std::string& puntosObtenidos);

    // Método para finalizar y calificar un intento
    void finalizarIntento(long long idIntento, const std::string& calificacion, const std::string& puntosObtenidos, const std::string& puntosTotales);

    // Puedes añadir más métodos como:
    // int obtenerNumeroDeIntentosRealizados(const std::string& idExamen, const std::string& idEstudiante);
    // std::vector<PreguntaConOpciones> obtenerPreguntasParaExamen(const std::string& idExamen);
};

#endif // GESTOR_INTENTOS_EXAMEN_H