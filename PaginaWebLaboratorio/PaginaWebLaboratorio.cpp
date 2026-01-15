#include <iostream>
#include <cgicc/Cgicc.h>
#include <cgicc/FormEntry.h>
#include <cgicc/CgiEnvironment.h>
#include <string>
#include <sstream>
#include <windows.h>
#include <shellapi.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>

#include "GestorBaseDeDatos.h"
#include "GestorAuditoria.h"
#include "GestorUsuarios.h"
#include "GestorRecursos.h"
#include "GestorGrupos.h"
#include "GestorExamenes.h"
#include "GestorIntentosExamen.h"
#include "GestorCalificaciones.h"
#include "GestorSimulacion.h"

void abrirSimuladorAsync(const std::string& idUsuario, const std::string& tipoExp);

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
	GestorSimulacion gestorSimulacion(conexion);

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

		const std::vector<cgicc::FormFile>& files = formulario.getFiles();
		auto file = formulario.getFile("foto");

		if (file != formulario.getFiles().end() && !file->getFilename().empty()) {
			std::string nombreOriginal = file->getFilename();
			std::string extension = "";
			size_t pos = nombreOriginal.find_last_of('.');
			if (pos != std::string::npos) {
				extension = nombreOriginal.substr(pos);
			}
			nombreArchivoFinal = id + extension;
			std::string rutaDestino = "C:\\Apache24\\htdocs\\PaginaWebLaboratorio\\img\\" + nombreArchivoFinal;
			std::ofstream archivoDisco(rutaDestino, std::ios::binary);
			if (archivoDisco.is_open()) {
				archivoDisco.write(file->getData().data(), file->getData().size());
				archivoDisco.close();
			}
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
	else if (accion == "abrirSimulador") {
		std::string idUsuario = formulario("usuario_id");
		std::string tipoExp = formulario("experimento");
		idUsuario.erase(std::remove(idUsuario.begin(), idUsuario.end(), '\r'), idUsuario.end());
		idUsuario.erase(std::remove(idUsuario.begin(), idUsuario.end(), '\n'), idUsuario.end());
		tipoExp.erase(std::remove(tipoExp.begin(), tipoExp.end(), '\r'), tipoExp.end());
		tipoExp.erase(std::remove(tipoExp.begin(), tipoExp.end(), '\n'), tipoExp.end());
		abrirSimuladorAsync(idUsuario, tipoExp);
		std::cout << "Content-type: application/json\r\n\r\n";
		std::cout << "{\"ok\": true, \"proceso\": \"" << tipoExp << "\"}";
	}
	else if (accion == "iniciarSimulacion") {
		std::string usuarioId = formulario("usuario_id");
		std::string nombre = formulario("nombre");
		std::string descripcion = formulario("descripcion");
		std::string dispositivo = formulario("dispositivo");
		int idSimulacion = gestorSimulacion.iniciarSimulacion(usuarioId, nombre, descripcion, dispositivo);
		std::cout << "Content-type: text/plain\r\n\r\n";
		std::cout << idSimulacion;
	}
	else if (accion == "registrarResultado") {
		gestorSimulacion.registrarResultado(formulario("simulacion_id"), formulario("variable"), formulario("valor"), formulario("unidad"));
		std::cout << "Content-type: text/plain\r\n\r\nOK";
	}
	else if (accion == "registrarEvento") {
		gestorSimulacion.registrarEvento(formulario("simulacion_id"), formulario("evento"), formulario("detalle"), formulario("tiempo"));
		std::cout << "Content-type: text/plain\r\n\r\nOK";
	}
	else if (accion == "finalizarSimulacion") {
		gestorSimulacion.finalizarSimulacion(formulario("simulacion_id"), formulario("duracion"));
		std::cout << "Content-type: text/plain\r\n\r\nOK";
	}
	else if (accion == "obtenerSeguimientoCompleto") {
		std::string filtroNombre = formulario("filtro_nombre");
		std::string filtroExp = formulario("filtro_exp");
		filtroNombre.erase(std::remove(filtroNombre.begin(), filtroNombre.end(), '\r'), filtroNombre.end());
		filtroNombre.erase(std::remove(filtroNombre.begin(), filtroNombre.end(), '\n'), filtroNombre.end());
		filtroExp.erase(std::remove(filtroExp.begin(), filtroExp.end(), '\r'), filtroExp.end());
		filtroExp.erase(std::remove(filtroExp.begin(), filtroExp.end(), '\n'), filtroExp.end());
		gestorSimulacion.obtenerSeguimientoCompleto(filtroNombre, filtroExp);
	}
	else if (accion == "obtenerDetallesSimulacion") {
		std::string idSim = formulario("simulacion_id");
		gestorSimulacion.obtenerDetallesSimulacion(idSim);
	}
	else if (accion == "obtenerSeguimientoEstudiante") {
		std::string idEstudiante = formulario("estudiante_id");
		std::string filtroExp = formulario("filtro_exp");
		idEstudiante.erase(std::remove(idEstudiante.begin(), idEstudiante.end(), '\r'), idEstudiante.end());
		idEstudiante.erase(std::remove(idEstudiante.begin(), idEstudiante.end(), '\n'), idEstudiante.end());
		filtroExp.erase(std::remove(filtroExp.begin(), filtroExp.end(), '\r'), filtroExp.end());
		filtroExp.erase(std::remove(filtroExp.begin(), filtroExp.end(), '\n'), filtroExp.end());
		gestorSimulacion.obtenerSeguimientoEstudiante(idEstudiante, filtroExp);
	}
	else {
		std::cout << "Content-type: text/html\r\n\r\n";
		std::cout << "<html><body><h2>Acción no reconocida.</h2></body></html>";
		return 0;
	}

	return 0;
}

void abrirSimuladorAsync(const std::string& idUsuario, const std::string& tipoExp) {
	std::string tempFile = "C:\\Temp\\unity_userid.txt";
	std::ofstream f(tempFile, std::ios::out | std::ios::trunc);
	if (f.is_open()) { f << idUsuario; f.close(); }

	std::string base = "C:\\LaboratorioVR\\";
	std::string exe;

	if (tipoExp == "fusion")          exe = base + "Fusion_VR\\escena_fusion.exe";
	else if (tipoExp == "evaporizacion")   exe = base + "Evaporacion\\Laboratorio_Evaporizacion.exe";
	else if (tipoExp == "solidificacion")  exe = base + "Solidificacion\\Laboratorio_solidificacion.exe";
	else if (tipoExp == "condensacion")    exe = base + "Condensacion\\Laboratorio_Condensacion.exe";

	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	if (CreateProcessA(exe.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}