#include <iostream>
#include <cgicc/Cgicc.h>
#include <cgicc/FormEntry.h>
#include <cgicc/CgiEnvironment.h>
#include <string>
#include <sstream>

#include "GestorBaseDeDatos.h"
#include "GestorAuditoria.h"
#include "GestorUsuarios.h"
#include "GestorRecursos.h"
#include "GestorGrupos.h"
#include "GestorExamenes.h"
#include "GestorIntentosExamen.h"
#include "GestorCalificaciones.h"

int main() {
	cgicc::Cgicc formulario;
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
	GestorIntentosExamen gestorIntentosExamen(conexion);
	GestorCalificaciones gestorCalificaciones(conexion);

	std::string idUsuarioAuditor;
	auto idEntry = formulario.getElement("usuario_id");
	if (idEntry != formulario.getElements().end() && !idEntry->isEmpty()) {
		idUsuarioAuditor = **idEntry;
	}
	else {
		idUsuarioAuditor = "1";
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
	else if (accion == "listaru") {
		gestorUsuarios.listarUsuarios();
	}
	else if (accion == "actualizaru") {
		std::string id = formulario("id");
		std::string nuevoNombre = formulario("nombren");
		std::string nuevaContrasena = formulario("contrasenan");
		std::string rol = formulario("rol");
		std::string idUsuarioAuditor = formulario("usuario_id");

		std::string nombreArchivoFinal = "";

		std::cerr << "ID recibido: [" << id << "]" << std::endl;

		const std::vector<cgicc::FormFile>& files = formulario.getFiles();
		std::cerr << "Cantidad de archivos: " << files.size() << std::endl;

		for (const auto& f : files) {
			std::cerr << "Archivo encontrado -> campo: " << f.getName()
				<< " | nombre real: " << f.getFilename()
				<< " | tamaño: " << f.getData().size()
				<< std::endl;
		}

		auto file = formulario.getFile("foto");

		if (file != formulario.getFiles().end() && !file->getFilename().empty()) {
			std::cerr << "Archivo recibido: " << file->getFilename() << std::endl;
			std::cerr << "Tamaño archivo: " << file->getData().size() << std::endl;

			std::string nombreOriginal = file->getFilename();
			std::string extension = "";

			size_t pos = nombreOriginal.find_last_of('.');
			if (pos != std::string::npos) {
				extension = nombreOriginal.substr(pos);
			}

			nombreArchivoFinal = id + extension;

			std::string rutaDestino =
				"C:\\Apache24\\htdocs\\PaginaWebLaboratorio\\img\\" + nombreArchivoFinal;

			std::ofstream archivoDisco(rutaDestino, std::ios::binary);

			if (!archivoDisco.is_open()) {
				std::cerr << "ERROR: No se pudo abrir el archivo destino" << std::endl;
			}
			else {
				archivoDisco.write(file->getData().data(), file->getData().size());
				archivoDisco.close();
				std::cerr << "Imagen guardada correctamente en: " << rutaDestino << std::endl;
			}
		}
		else {
			std::cerr << "NO se recibió archivo 'foto'" << std::endl;
		}

		gestorUsuarios.actualizarUsuario(id, nuevoNombre, nuevaContrasena, idUsuarioAuditor, rol);
	}
	else if (accion == "eliminaru") {
		std::string id = formulario("id");
		gestorUsuarios.eliminarUsuario(id, idUsuarioAuditor);
	}
	else if (accion == "listare") {
		gestorRecursos.listarRecursos();
	}
	else if (accion == "eliminarr") {
		std::string id = formulario("id");
		gestorRecursos.eliminarRecurso(id);
	}
	else if (accion == "registrarr") {
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
	else if (accion == "listarAu") {
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
		bool esCorrecta = (formulario("es_correcta") == "true");
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
	}
	else if (accion == "listarExamenesPorProfesor") {
		std::string idProfesor = formulario("idProfesor");
		gestorExamenes.listarExamenesPorProfesor(idProfesor);
	}
	else if (accion == "eliminarExamen") {
		std::string idExamen = formulario("examen_id");
		gestorExamenes.eliminarExamen(idExamen);
	}
	else if (accion == "mostrarExamen") {
		std::string idExamen = formulario("examen_id");
		gestorExamenes.mostrarExamen(idExamen);
	}
	else if (accion == "editarExamen") {
		std::string id = formulario("id");
		gestorExamenes.formularioEditarExamen(id);
	}
	else if (accion == "guardarEdicion") {
		gestorExamenes.guardarCambiosExamen(formulario);
	}
	else if (accion == "listarExamenesEstudiante") {
		std::string idEstudiante = formulario("estudiante_id");
		gestorExamenes.listarExamenesEstudiante(idEstudiante);
	}
	else if (accion == "prepararExamenEstudiante") {
		std::string idExamen = formulario("examen_id");
		std::string idEstudiante = formulario("estudiante_id");
		gestorExamenes.prepararExamenEstudiante(idExamen, idEstudiante);
	}
	else if (accion == "calificarExamen") {
		gestorExamenes.calificarExamen(formulario, gestorIntentosExamen);
	}
	else if (accion == "mostrarResultadoIntento") {
		std::string idIntento = formulario("intento_id");
		gestorExamenes.mostrarResultadoIntento(idIntento);
	}
	else if (accion == "listarGruposProfesorJSON") {
		std::string idProfesor = formulario("id");
		gestorGrupos.listarGruposProfesorJSON(idProfesor);
	}

	else if (accion == "obtenerSeguimientoProfesor") {
		std::string idProfesor = formulario("id_profesor");
		std::string idGrupo = formulario("id_grupo");

		gestorCalificaciones.obtenerSeguimientoProfesor(idProfesor, idGrupo);
	}
	else if (accion == "obtenerHistorialAlumno") {
		std::string idEst = formulario("estudiante_id");
		gestorCalificaciones.obtenerHistorialAlumno(idEst);
	}
	else {
		std::cout << "Content-type: text/html\r\n\r\n";
		std::cout << "<html><body><h2>Acción no reconocida.</h2></body></html>";
		return 0;
	}

	return 0;
}