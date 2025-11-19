#include <iostream>
#include <cgicc/Cgicc.h>
#include <cgicc/FormEntry.h>
#include <cgicc/CgiEnvironment.h>
#include <string>
#include <sstream>

// Incluir las cabeceras de tus clases con los nuevos nombres
#include "GestorBaseDeDatos.h"
#include "GestorAuditoria.h"
#include "GestorUsuarios.h"
#include "GestorRecursos.h"
#include "GestorGrupos.h"
#include "GestorExamenes.h"
#include "GestorIntentosExamen.h" // Nueva
#include "GestorCalificaciones.h" // Nueva

// Las funciones de Utilidades se usan a través de su namespace o por las clases que las incluyen.

int main() {
    cgicc::Cgicc formulario; // Cambiado a 'formulario'
    std::string accion = formulario("accion");

    GestorBaseDeDatos gestorDB("localhost", "root", "123456", "laboratorioquimica", 3306);

    if (!gestorDB.conectar()) {
        std::cout << "Content-type: text/html\r\n\r\n";
        std::cout << "<html><body><h2>Error en la conexión a la base de datos.</h2></body></html>";
        return 1;
    }

    MYSQL* conexion = gestorDB.getConexion();

    GestorAuditoria gestorAuditoria(conexion);
    GestorUsuarios gestorUsuarios(conexion, gestorAuditoria);
    GestorRecursos gestorRecursos(conexion);
    GestorGrupos gestorGrupos(conexion);
    GestorExamenes gestorExamenes(conexion);
    GestorIntentosExamen gestorIntentosExamen(conexion); // Instancia de la nueva clase
    GestorCalificaciones gestorCalificaciones(conexion); // Instancia de la nueva clase

    std::string idUsuarioAuditor;
    auto idEntry = formulario.getElement("usuario_id");
    if (idEntry != formulario.getElements().end() && !idEntry->isEmpty()) {
        idUsuarioAuditor = **idEntry;
    }
    else {
        idUsuarioAuditor = "1"; // Valor por defecto o para depuración
    }

    if (accion == "verificar") {
        std::string correo = formulario("correo");
        std::string contrasena = formulario("contrasena");
        gestorUsuarios.verificarUsuario(correo, contrasena);
    }
    else if (accion == "registrar") {
        std::string correo = formulario("correo");
        std::string contrasena = formulario("contrasena");
        std::string nombre = formulario("nombre");
        std::string rol = formulario("rol");
        gestorUsuarios.registrarUsuario(correo, contrasena, nombre, rol, idUsuarioAuditor);
    }
    else if (accion == "listaru") { // listaru -> listarUsuarios
        gestorUsuarios.listarUsuarios();
    }
    else if (accion == "actualizaru") { // actualizaru -> actualizarUsuario
        std::string id = formulario("id");
        std::string nuevoNombre = formulario("nombren");
        std::string nuevaContrasena = formulario("contrasenan");
        gestorUsuarios.actualizarUsuario(id, nuevoNombre, nuevaContrasena, idUsuarioAuditor);
    }
    else if (accion == "eliminaru") { // eliminaru -> eliminarUsuario
        std::string id = formulario("id");
        gestorUsuarios.eliminarUsuario(id, idUsuarioAuditor);
    }
    else if (accion == "listare") { // listare -> listarRecursos
        gestorRecursos.listarRecursos();
    }
    else if (accion == "eliminarr") { // eliminarr -> eliminarRecurso
        std::string id = formulario("id");
        gestorRecursos.eliminarRecurso(id);
    }
    else if (accion == "registrarr") { // registrarr -> registrarRecurso
        std::string titulo = formulario("titulo");
        std::string descripcion = formulario("descripcion");
        std::string categoria = formulario("categoria");
        std::string tipo = formulario("tipo");
        std::string autor = formulario("autor");
        std::string palabras_clave = formulario("palabras_clave");
        std::string fecha = formulario("fecha");
        std::string enlace = formulario("enlace");

        gestorRecursos.registrarRecurso(titulo, descripcion, categoria, tipo, autor, palabras_clave, fecha, enlace, formulario);
    }
    else if (accion == "listarAu") { // listarAu -> listarAuditorias
        gestorAuditoria.listarAuditorias();
    }
    else if (accion == "agregarGrupo") {
        std::string nombreGrupo = formulario("nombre_grupo");
        std::string descripcion = formulario("descripcion_grupo");
        std::string idProfesor = formulario("id");
        gestorGrupos.registrarGrupo(nombreGrupo, descripcion, idProfesor);
    }
    else if (accion == "listarGrupos") {
        gestorGrupos.listarGrupos();
        return 0;
    }
    else if (accion == "asignarEstudianteGrupo") {
        std::string idGrupo = formulario("grupo_id");
        std::string idEstudiante = formulario("estudiante_id");
        gestorGrupos.registrarEstudianteEnGrupo(idGrupo, idEstudiante);
    }
    else if (accion == "listarGruposProfesor") {
        std::string idProfesor = formulario("id");
        gestorGrupos.listarGruposProfesor(idProfesor);
        return 0;
    }
    else if (accion == "crearExamen") {
        std::string titulo = formulario("titulo");
        std::string descripcion = formulario("descripcion");
        std::string idGrupo = formulario("grupo_id");
        std::string instrucciones = formulario("instrucciones");
        std::string intentosPermitidos = formulario("intentos_permitidos");
        std::string idProfesor = formulario("profesor_id");
        gestorExamenes.registrarExamen(titulo, descripcion, idGrupo, instrucciones, intentosPermitidos, idProfesor);
		return 0;
    }
    else if (accion == "agregarPregunta") {
        std::string idExamen = formulario("examen_id");
        std::string pregunta = formulario("pregunta");
        std::string puntos = formulario("puntos");
        std::string explicacion = formulario("explicacion");
        gestorExamenes.registrarPregunta(idExamen, pregunta, puntos, explicacion);
        return 0;
    }
    else if (accion == "agregarOpciones") {
        gestorExamenes.registrarOpciones(formulario);
        return 0;
    }
    else if (accion == "iniciarIntentoExamen") {
        std::string idExamen = formulario("examen_id");
        std::string idEstudiante = formulario("estudiante_id");
        long long idIntento = gestorIntentosExamen.iniciarIntento(idExamen, idEstudiante);
        if (idIntento != -1) {
            std::cout << "Content-type: application/json\r\n\r\n";
            std::cout << "{ \"id_intento\": " << idIntento << " }";
        }
        else {
            std::cout << "Content-type: application/json\r\n\r\n";
            std::cout << "{ \"error\": \"No se pudo iniciar el intento.\" }";
        }
    }
    else if (accion == "registrarRespuestaEstudiante") {
        long long idIntento = std::stoll(formulario("intento_id"));
        std::string idPregunta = formulario("pregunta_id");
        std::string idOpcionSeleccionada = formulario("opcion_seleccionada_id");
        bool esCorrecta = (formulario("es_correcta") == "true"); // Asegúrate de enviar "true" o "false" desde el cliente
        std::string puntosObtenidos = formulario("puntos_obtenidos");
        gestorIntentosExamen.registrarRespuesta(idIntento, idPregunta, idOpcionSeleccionada, esCorrecta, puntosObtenidos);
        std::cout << "Content-type: text/plain\r\n\r\nRespuesta registrada.";
    }
    else if (accion == "finalizarIntentoExamen") {
        long long idIntento = std::stoll(formulario("intento_id"));
        std::string calificacion = formulario("calificacion_final");
        std::string puntosObtenidos = formulario("puntos_obtenidos_intento");
        std::string puntosTotales = formulario("puntos_totales_intento");
        gestorIntentosExamen.finalizarIntento(idIntento, calificacion, puntosObtenidos, puntosTotales);
        std::cout << "Content-type: text/plain\r\n\r\nIntento finalizado y calificado.";
    }
    else if (accion == "registrarCalificacionFinal") {
        std::string idExamen = formulario("examen_id");
        std::string idEstudiante = formulario("estudiante_id");
        long long mejorIntentoId = std::stoll(formulario("mejor_intento_id"));
        std::string calificacionFinal = formulario("calificacion_final");
        std::string puntosObtenidos = formulario("puntos_obtenidos_final");
        std::string puntosTotales = formulario("puntos_totales_final");
        std::string porcentaje = formulario("porcentaje_final");
        std::string comentarios = formulario("comentarios_calificacion");
        gestorCalificaciones.registrarCalificacionFinal(idExamen, idEstudiante, mejorIntentoId, calificacionFinal, puntosObtenidos, puntosTotales, porcentaje, comentarios);
        std::cout << "Content-type: text/plain\r\n\r\nCalificación final registrada.";
    }else if(accion== "listarExamenesPorProfesor"){
        std::string idProfesor = formulario("idProfesor");
        gestorExamenes.listarExamenesPorProfesor(idProfesor);
	} else if (accion == "eliminarExamen") {
        std::string idExamen = formulario("examen_id");
        gestorExamenes.eliminarExamen(idExamen);
	} else if (accion == "mostrarExamen") {
        std::string idExamen = formulario("examen_id");
        gestorExamenes.mostrarExamen(idExamen);
	} else if (accion == "editarExamen") {
        // Obtenemos el ID directamente del objeto 'formulario'
        std::string id = formulario("id");
        gestorExamenes.formularioEditarExamen(id);
    } else if (accion == "guardarEdicion") {
        // Pasamos el objeto 'formulario' (que es de tipo Cgicc)
        gestorExamenes.guardarCambiosExamen(formulario);
    }
    else {
        std::cout << "Content-type: text/html\r\n\r\n";
        std::cout << "<html><body><h2>Acción no reconocida.</h2></body></html>";
        return 0;
    }

    return 0;
}