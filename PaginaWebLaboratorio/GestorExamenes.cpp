#include "GestorExamenes.h"
#include <cstdlib> // Para atoi

GestorExamenes::GestorExamenes(MYSQL* conexion) : conexionDB(conexion) {}

void GestorExamenes::registrarExamen(const std::string& titulo, const std::string& descripcion, const std::string& idGrupo,
    const std::string& instrucciones, const std::string& intentosPermitidos, const std::string& idProfesor) {
    std::string query = "INSERT INTO examenes (titulo, descripcion, grupo_id, profesor_id, instrucciones, intentos_permitidos) VALUES ('" +
        titulo + "', '" +
        descripcion + "', '" +
        idGrupo + "', '" +
        idProfesor + "', '" +
        instrucciones + "', '" +
        intentosPermitidos + "')";
    std::cout << "Content-type: application/json\r\n\r\n";
    if (mysql_query(conexionDB, query.c_str()) == 0) {
        my_ulonglong id_examen_generado = mysql_insert_id(conexionDB);
        std::cout << "{ \"examen_id\": " << id_examen_generado << " }";  // <--- aquí el cambio
    }
    else {
        std::string errorMsg = mysql_error(conexionDB);
        std::cout << "{ \"error\": \"" << errorMsg << "\" }";
    }
}

void GestorExamenes::registrarPregunta(const std::string& idExamen, const std::string& pregunta, const std::string& puntos,
    const std::string& explicacion) {
    // Calcular el siguiente orden_pregunta automáticamente
    int orden = 1;
    std::string consultaConteo = "SELECT COUNT(*) FROM preguntas WHERE examen_id = " + idExamen;
    if (mysql_query(conexionDB, consultaConteo.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conexionDB);
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row && row[0]) orden = atoi(row[0]) + 1;
        mysql_free_result(res);
    }

    std::string query = "INSERT INTO preguntas (examen_id, pregunta, puntos, orden_pregunta, explicacion) VALUES ('" +
        idExamen + "', '" + pregunta + "', '" + puntos + "', '" + std::to_string(orden) + "', '" + explicacion + "')";
    std::cout << "Content-type: application/json\r\n\r\n";
    if (mysql_query(conexionDB, query.c_str()) == 0) {
        my_ulonglong id_pregunta_generada = mysql_insert_id(conexionDB);
        std::cout << "{ \"pregunta_id\": " << id_pregunta_generada << " }";
    }
    else {
        std::string errorMsg = mysql_error(conexionDB);
        std::cout << "{ \"error\": \"" << errorMsg << "\" }";
    }
}

void GestorExamenes::registrarOpciones(const cgicc::Cgicc& formulario) {
    std::string id_pregunta = formulario("pregunta_id");
    int indice = 0;
    bool algunaOpcionGuardada = false;

    // Detectar dinámicamente las opciones enviadas como arreglos (opciones[0][texto], opciones[0][es_correcta])
    while (true) {
        std::string nombreTexto = "opciones[" + std::to_string(indice) + "][texto]";
        std::string nombreCorrecta = "opciones[" + std::to_string(indice) + "][es_correcta]";

        auto itTexto = formulario.getElement(nombreTexto);
        if (itTexto == formulario.getElements().end()) break;

        std::string textoOpcion = **itTexto;
        std::string esCorrectaStr = formulario(nombreCorrecta);
        bool esCorrecta = (esCorrectaStr == "1");

        std::string query = "INSERT INTO opciones_respuesta (pregunta_id, opcion, es_correcta, orden_opcion) VALUES ('" +
            id_pregunta + "', '" + textoOpcion + "', " + (esCorrecta ? "1" : "0") + ", " + std::to_string(indice + 1) + ")";

        if (mysql_query(conexionDB, query.c_str()) == 0) {
            algunaOpcionGuardada = true;
        }
        else {
            std::cerr << "Error al insertar opción: " << mysql_error(conexionDB) << std::endl;
        }

        indice++;
    }

    std::cout << "Content-Type: application/json\r\n\r\n";
    if (algunaOpcionGuardada)
        std::cout << "{ \"success\": true }";
    else
        std::cout << "{ \"success\": false, \"error\": \"No se recibieron opciones o hubo errores al guardar.\" }";
}


void GestorExamenes::listarExamenesPorProfesor(const std::string& idProfesor) {
    std::string query = "SELECT id, titulo, descripcion, grupo_id, instrucciones, intentos_permitidos FROM examenes WHERE profesor_id = " + idProfesor;
    MYSQL_RES* resultado;
    MYSQL_ROW fila;

    std::cout << "Content-type: text/html\r\n\r\n";

    if (mysql_query(conexionDB, query.c_str()) == 0) {
        resultado = mysql_store_result(conexionDB);

        if (resultado && mysql_num_rows(resultado) > 0) {
            // Cambio: Usamos la clase 'tabla-moderna' en lugar de border='1'
            std::cout << "<table class='tabla-moderna'>";
            std::cout << "<thead><tr>"
                "<th>ID</th>"
                "<th>Título</th>"
                "<th>Descripción</th>"
                "<th>Grupo ID</th>"
                "<th>Intentos</th>" // Abrevié el título para espacio
                "<th>Acciones</th>"
                "</tr></thead><tbody>";

            while ((fila = mysql_fetch_row(resultado)) != nullptr) {
                std::string id = fila[0] ? fila[0] : "";
                std::string titulo = fila[1] ? fila[1] : "";
                std::string descripcion = fila[2] ? fila[2] : "";
                std::string grupo_id = fila[3] ? fila[3] : "";
                std::string instrucciones = fila[4] ? fila[4] : ""; // No la mostramos en la tabla para ahorrar espacio, o puedes dejarla
                std::string intentos_permitidos = fila[5] ? fila[5] : "";

                std::cout << "<tr>";
                std::cout << "<td>" << id << "</td>";
                std::cout << "<td><strong>" << titulo << "</strong></td>"; // Negrita al título
                std::cout << "<td>" << descripcion << "</td>";
                std::cout << "<td>" << grupo_id << "</td>";
                std::cout << "<td>" << intentos_permitidos << "</td>";
                std::cout << "<td style='white-space: nowrap;'>"; // Evita que los botones se rompan feo

                // 1. Botón EDITAR (Amarillo + Icono Lápiz)
                std::cout << "<a href='/cgi-bin/PaginaWebLaboratorio.exe?accion=editarExamen&id=" << id
                    << "' class='link-no-style'>"
                    << "<button class='btn-accion btn-editar' title='Editar Examen'>"
                    << "<i class='fas fa-edit'></i>" // Icono solo (o agrega texto si cabe)
                    << "</button></a> ";

                // 2. Botón AGREGAR PREGUNTAS (Verde + Icono Plus)
                std::cout << "<a href='http://localhost/PaginaWebLaboratorio/agregarPregunta.html?id_examen=" << id
                    << "' class='link-no-style'>"
                    << "<button class='btn-accion btn-agregar' title='Agregar Preguntas'>"
                    << "<i class='fas fa-plus-circle'></i>"
                    << "</button></a> ";

                // 3. Botón VER PREGUNTAS (Azul + Icono Ojo)
                std::cout << "<a href='http://localhost/PaginaWebLaboratorio/verExamen.html?id=" << id
                    << "' class='link-no-style'>"
                    << "<button class='btn-accion btn-ver' title='Ver Examen'>"
                    << "<i class='fas fa-eye'></i>"
                    << "</button></a> ";

                // 4. Botón ELIMINAR (Rojo + Icono Basura)
                std::cout << "<a href='/cgi-bin/PaginaWebLaboratorio.exe?accion=eliminarExamen&examen_id=" << id
                    << "' onclick=\"return confirm('¿Estás seguro de eliminar el examen ID " << id << "?');\" class='link-no-style'>"
                    << "<button class='btn-accion btn-eliminar' title='Eliminar Examen'>"
                    << "<i class='fas fa-trash-alt'></i>"
                    << "</button></a>";

                std::cout << "</td>";
                std::cout << "</tr>";
            }
            std::cout << "</tbody></table>";
        }
        else {
            // Mensaje bonito si no hay datos
            std::cout << "<div style='text-align:center; padding:40px; background:white; border-radius:8px; box-shadow:0 2px 5px rgba(0,0,0,0.1);'>";
            std::cout << "<i class='fas fa-folder-open' style='font-size:3em; color:#ccc;'></i>";
            std::cout << "<p style='color:#666; margin-top:10px;'>No hay exámenes registrados para este profesor.</p>";
            std::cout << "</div>";
        }
        mysql_free_result(resultado);
    }
    else {
        std::cout << "<p style='color:red'>Error al ejecutar la consulta SQL: " << mysql_error(conexionDB) << "</p>";
    }
}
void GestorExamenes::eliminarExamen(const std::string& idExamen) {
    std::cout << "Content-type: text/html\r\n\r\n";

    if (idExamen.empty()) {
        std::cout << "<html><body><h3>Error: el parámetro examen_id está vacío.</h3></body></html>";
        return;
    }

    int id = atoi(idExamen.c_str());
    if (id <= 0) {
        std::cout << "<html><body><h3>Error: el ID de examen no es válido.</h3></body></html>";
        return;
    }

    std::string query = "DELETE FROM examenes WHERE id = " + std::to_string(id);

    if (mysql_query(conexionDB, query.c_str()) == 0) {
        if (mysql_affected_rows(conexionDB) > 0) {
            // ✅ Redirige automáticamente a la misma página para actualizar la lista
            std::cout << "<meta http-equiv='refresh' content='0;url=http://localhost/PaginaWebLaboratorio/gestionEvaluaciones.html'>";
        }
        else {
            std::cout << "<html><body><h3>No se encontró un examen con ese ID.</h3></body></html>";
        }
    }
    else {
        std::string errorMsg = mysql_error(conexionDB);
        std::cout << "<html><body><h3>Error al eliminar examen: " << errorMsg << "</h3></body></html>";
    }
}
void GestorExamenes::mostrarExamen(const std::string& idExamen) {
    std::string query =
        "SELECT "
        "e.id AS examen_id, "
        "e.titulo AS titulo_examen, "
        "e.descripcion AS descripcion_examen, "
        "e.instrucciones, "
        "e.intentos_permitidos, "
        "p.id AS pregunta_id, "
        "p.pregunta, "
        "p.puntos, "
        "p.orden_pregunta, "
        "p.explicacion AS explicacion_pregunta, "
        "op.id AS opcion_id, "
        "op.opcion AS texto_opcion, "
        "op.es_correcta, "
        "op.orden_opcion "
        "FROM examenes e "
        "JOIN preguntas p ON e.id = p.examen_id "
        "JOIN opciones_respuesta op ON p.id = op.pregunta_id "
        "WHERE e.id = " + idExamen + " "
        "ORDER BY p.orden_pregunta ASC, op.orden_opcion ASC";

    if (mysql_query(conexionDB, query.c_str()) != 0) {
        std::cout << "Content-type: text/html\r\n\r\n";
        std::cout << "<div class='card-view'><h3 style='color:red;'>Error SQL al cargar examen.</h3></div>";
        return;
    }

    MYSQL_RES* resultado = mysql_store_result(conexionDB);
    if (!resultado) {
        std::cout << "Content-type: text/html\r\n\r\n";
        std::cout << "<div class='card-view'><h3>No se pudo obtener el resultado.</h3></div>";
        return;
    }

    MYSQL_ROW fila;
    std::string preguntaActual = "";
    int preguntaIDActual = -1;

    // IMPORTANTE: Solo imprimimos HTML limpio. El CSS ya está en tu archivo .css
    std::cout << "Content-type: text/html\r\n\r\n";

    bool primerafila = true;

    // Contenedor específico que ya tiene estilos en tu CSS
    std::cout << "<div id='contenedor-preguntas'>";

    while ((fila = mysql_fetch_row(resultado)) != nullptr) {
        std::string examenID = fila[0] ? fila[0] : "";
        std::string tituloExamen = fila[1] ? fila[1] : "";
        std::string descripcionExamen = fila[2] ? fila[2] : "";
        std::string instrucciones = fila[3] ? fila[3] : "";
        std::string intentosPermitidos = fila[4] ? fila[4] : "";

        // 1. TARJETA DE ENCABEZADO
        if (primerafila) {
            std::cout << "<div class='card-view card-header-exam'>";
            std::cout << "<h2 class='exam-title'>" << tituloExamen << "</h2>";

            if (!descripcionExamen.empty())
                std::cout << "<p class='exam-desc'>" << descripcionExamen << "</p>";

            if (!instrucciones.empty()) {
                std::cout << "<div class='exam-instr'>";
                std::cout << "<strong><i class='fas fa-info-circle'></i> Instrucciones:</strong> " << instrucciones;
                std::cout << "</div>";
            }
            std::cout << "<p style='margin-top:10px; font-size:0.9em; color:#666;'>Intentos permitidos: <strong>" << intentosPermitidos << "</strong></p>";
            std::cout << "</div>"; // Fin header
            primerafila = false;
        }

        std::string preguntaId = fila[5] ? fila[5] : "";
        int preguntaID = preguntaId.empty() ? -1 : atoi(preguntaId.c_str());
        std::string preguntaTexto = fila[6] ? fila[6] : "";
        std::string puntos = fila[7] ? fila[7] : "";
        std::string ordenPregunta = fila[8] ? fila[8] : "";

        std::string opcionID = fila[10] ? fila[10] : "";
        std::string textoOpcion = fila[11] ? fila[11] : "";
        std::string esCorrecta = fila[12] ? fila[12] : "0";

        // 2. TARJETA DE PREGUNTA (Nueva pregunta detectada)
        if (preguntaID != preguntaIDActual) {
            if (preguntaIDActual != -1) {
                std::cout << "</div>"; // Cerrar tarjeta anterior
            }

            std::cout << "<div class='card-view card-question'>";

            // Header de la pregunta (Texto + Badge de puntos)
            std::cout << "<div class='question-header'>";
            std::cout << "<span class='question-text'>" << ordenPregunta << ". " << preguntaTexto << "</span>";
            std::cout << "<span class='badge-puntos'>" << puntos << " pts</span>";
            std::cout << "</div>";

            preguntaIDActual = preguntaID;
        }

        // 3. OPCIÓN DE RESPUESTA
        // Usamos <label> para que sea clickeable toda la fila
        std::cout << "<label class='opcion-item'>";
        std::cout << "<input type='radio' name='pregunta_" << preguntaId << "' value='" << opcionID << "'> ";
        std::cout << "<span style='flex-grow:1;'>" << textoOpcion << "</span>";

        // Mostrar etiqueta "Correcta" (Solo visualización profesor)
        if (esCorrecta == "1") {
            std::cout << "<span class='feedback correcta'><i class='fas fa-check'></i> Correcta</span>";
        }

        std::cout << "</label>";
    }

    if (!primerafila) {
        std::cout << "</div>"; // Cerrar última tarjeta
    }
    else {
        std::cout << "<div class='card-view'><p style='text-align:center; color:#777;'>No hay preguntas registradas.</p></div>";
    }

    std::cout << "</div>"; // Cerrar #contenedor-preguntas

    mysql_free_result(resultado);
}

void GestorExamenes::formularioEditarExamen(const std::string& idExamen) {
    std::string query =
        "SELECT "
        "e.id, e.titulo, e.descripcion, e.instrucciones, e.intentos_permitidos, "
        "p.id, p.pregunta, p.puntos, p.orden_pregunta, "
        "op.id, op.opcion, op.es_correcta, op.orden_opcion "
        "FROM examenes e "
        "JOIN preguntas p ON e.id = p.examen_id "
        "JOIN opciones_respuesta op ON p.id = op.pregunta_id "
        "WHERE e.id = " + idExamen + " "
        "ORDER BY p.orden_pregunta ASC, op.orden_opcion ASC";

    if (mysql_query(conexionDB, query.c_str()) != 0) {
        std::cout << "Content-type: text/html\r\n\r\n<h3>Error SQL: " << mysql_error(conexionDB) << "</h3>";
        return;
    }

    MYSQL_RES* resultado = mysql_store_result(conexionDB);
    MYSQL_ROW fila;

    std::cout << "Content-type: text/html\r\n\r\n";

    // INICIO DEL HTML + CSS INCORPORADO
    std::cout << "<!DOCTYPE html><html lang='es'><head><meta charset='UTF-8'><title>Editar Examen</title>";

    // --- AQUÍ ESTÁ EL CSS PARA QUE NO SE VEA FEO ---
    std::cout << "<style>"
        "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f8f9fa; color: #333; margin: 0; padding: 20px; }"
        ".main-container { max-width: 900px; margin: 0 auto; }"

        /* Estilo de Tarjetas (Como las tablas de tu diseño) */
        ".card { background: white; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); padding: 25px; margin-bottom: 20px; border-top: 5px solid #003366; }"
        ".card-pregunta { border-top: 5px solid #007bff; background-color: #ffffff; }"

        /* Títulos */
        "h2 { color: #003366; border-bottom: 1px solid #eee; padding-bottom: 10px; margin-top: 0; }"
        "h4 { color: #444; margin-top: 0; display: flex; justify-content: space-between; align-items: center; }"

        /* Inputs y Textareas bonitos */
        "label { font-weight: 600; color: #555; display: block; margin-bottom: 5px; margin-top: 15px; }"
        "input[type='text'], input[type='number'], textarea { width: 100%; padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px; box-sizing: border-box; transition: border 0.3s; }"
        "input[type='text']:focus, textarea:focus { border-color: #007bff; outline: none; box-shadow: 0 0 5px rgba(0,123,255,0.2); }"

        /* Estilo específico para las opciones (Radio + Texto) */
        ".fila-opcion { display: flex; align-items: center; margin-bottom: 8px; padding: 5px; border-radius: 4px; transition: background 0.2s; }"
        ".fila-opcion:hover { background-color: #f1f3f5; }"
        "input[type='radio'] { transform: scale(1.5); margin-right: 15px; cursor: pointer; }"

        /* Botones */
        ".btn-container { text-align: right; margin-top: 20px; }"
        ".btn { padding: 12px 24px; border: none; border-radius: 5px; font-size: 16px; cursor: pointer; transition: background 0.3s; text-decoration: none; display: inline-block; }"
        ".btn-guardar { background-color: #28a745; color: white; }"
        ".btn-guardar:hover { background-color: #218838; }"
        ".btn-cancelar { background-color: #6c757d; color: white; margin-left: 10px; }"
        ".btn-cancelar:hover { background-color: #5a6268; }"
        "</style>";

    // VINCULO A TU CSS EXTERNO (Por si tienes estilos globales)
    std::cout << "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css'>";
    std::cout << "</head><body>";

    std::cout << "<div class='main-container'>";
    std::cout << "<form method='POST' action='/cgi-bin/PaginaWebLaboratorio.exe'>";
    std::cout << "<input type='hidden' name='accion' value='guardarEdicion'>";
    std::cout << "<input type='hidden' name='examen_id' value='" << idExamen << "'>";

    bool primeraFila = true;
    int preguntaIDActual = -1;

    while ((fila = mysql_fetch_row(resultado)) != nullptr) {
        std::string titulo = fila[1] ? fila[1] : "";
        std::string descripcion = fila[2] ? fila[2] : "";
        std::string instrucciones = fila[3] ? fila[3] : "";
        std::string intentos = fila[4] ? fila[4] : "";

        std::string pID = fila[5] ? fila[5] : "";
        std::string pTexto = fila[6] ? fila[6] : "";
        std::string pPuntos = fila[7] ? fila[7] : "";

        std::string oID = fila[9] ? fila[9] : "";
        std::string oTexto = fila[10] ? fila[10] : "";
        std::string oEsCorrecta = fila[11] ? fila[11] : "0";

        // 1. TARJETA DE INFORMACIÓN GENERAL
        if (primeraFila) {
            std::cout << "<div class='card'>";
            std::cout << "<h2><i class='fas fa-edit'></i> Editar Información del Examen</h2>";

            std::cout << "<label>Título:</label>";
            std::cout << "<input type='text' name='titulo_examen' value='" << titulo << "' required>";

            std::cout << "<div style='display:flex; gap:20px;'>"; // Flexbox para columnas
            std::cout << "<div style='flex:1;'><label>Descripción:</label><textarea name='descripcion_examen' rows='3'>" << descripcion << "</textarea></div>";
            std::cout << "<div style='flex:1;'><label>Instrucciones:</label><textarea name='instrucciones_examen' rows='3'>" << instrucciones << "</textarea></div>";
            std::cout << "</div>";

            std::cout << "<label>Intentos Permitidos:</label>";
            std::cout << "<input type='number' name='intentos_examen' value='" << intentos << "' style='width:100px;'>";
            std::cout << "</div>"; // Fin card examen
            primeraFila = false;
        }

        int pID_int = atoi(pID.c_str());

        // 2. TARJETA DE PREGUNTA
        if (pID_int != preguntaIDActual) {
            if (preguntaIDActual != -1) std::cout << "</div>"; // Cerrar pregunta anterior

            std::cout << "<div class='card card-pregunta'>";
            std::cout << "<h4>";
            std::cout << "<span><i class='fas fa-question-circle'></i> Pregunta #" << fila[8] << "</span>"; // fila[8] es orden
            std::cout << "<span style='font-size:0.9em;'>Puntos: <input type='number' step='0.01' name='puntos_" << pID << "' value='" << pPuntos << "' style='width:70px; display:inline-block;'></span>";
            std::cout << "</h4>";

            std::cout << "<label>Texto de la Pregunta:</label>";
            std::cout << "<textarea name='texto_pregunta_" << pID << "' rows='2'>" << pTexto << "</textarea>";
            std::cout << "<label style='margin-top:20px; border-bottom:1px solid #eee; padding-bottom:5px;'>Opciones de Respuesta:</label>";

            preguntaIDActual = pID_int;
        }

        // 3. OPCIONES (Filas limpias)
        std::cout << "<div class='fila-opcion'>";
        std::cout << "<input type='radio' name='correcta_p_" << pID << "' value='" << oID << "' "
            << (oEsCorrecta == "1" ? "checked" : "") << " title='Marcar como respuesta correcta'> ";
        std::cout << "<input type='text' name='texto_opcion_" << oID << "' value='" << oTexto << "' placeholder='Texto de la opción'>";
        std::cout << "</div>";
    }

    if (!primeraFila) std::cout << "</div>"; // Cerrar última tarjeta de pregunta

    // BOTONES FLOTANTES O AL FINAL
    std::cout << "<div class='btn-container'>";
    std::cout << "<button type='submit' class='btn btn-guardar'><i class='fas fa-save'></i> Guardar Cambios</button>";
    std::cout << "<a href='http://localhost/PaginaWebLaboratorio/gestionEvaluaciones.html' class='btn btn-cancelar'><i class='fas fa-times'></i> Cancelar</a>";
    std::cout << "</div>";

    std::cout << "</form></div><br><br></body></html>";
    mysql_free_result(resultado);
}

void GestorExamenes::guardarCambiosExamen(const cgicc::Cgicc& formData) {
    std::cout << "Content-type: text/html\r\n\r\n";

    // 1. Actualizar Datos Generales del Examen
    std::string idExamen = formData("examen_id");
    std::string titulo = formData("titulo_examen");
    std::string desc = formData("descripcion_examen");
    std::string instr = formData("instrucciones_examen");
    std::string intentos = formData("intentos_examen");

    std::string queryExamen = "UPDATE examenes SET titulo='" + titulo + "', descripcion='" + desc +
        "', instrucciones='" + instr + "', intentos_permitidos=" + intentos +
        " WHERE id=" + idExamen;

    mysql_query(conexionDB, queryExamen.c_str());

    // 2. Recorrer todos los elementos enviados para encontrar preguntas y opciones
    const std::vector<cgicc::FormEntry>& elementos = formData.getElements();

    for (const auto& entrada : elementos) {
        std::string nombre = entrada.getName();
        std::string valor = entrada.getValue();

        // A. Actualizar Texto de Pregunta (name="texto_pregunta_123")
        if (nombre.find("texto_pregunta_") == 0) {
            std::string idPregunta = nombre.substr(15); // Cortar el prefijo
            std::string q = "UPDATE preguntas SET pregunta='" + valor + "' WHERE id=" + idPregunta;
            mysql_query(conexionDB, q.c_str());
        }

        // B. Actualizar Puntos de Pregunta (name="puntos_123")
        else if (nombre.find("puntos_") == 0) {
            std::string idPregunta = nombre.substr(7);
            std::string q = "UPDATE preguntas SET puntos=" + valor + " WHERE id=" + idPregunta;
            mysql_query(conexionDB, q.c_str());
        }

        // C. Actualizar Texto de Opción (name="texto_opcion_456")
        else if (nombre.find("texto_opcion_") == 0) {
            std::string idOpcion = nombre.substr(13);
            std::string q = "UPDATE opciones_respuesta SET opcion='" + valor + "' WHERE id=" + idOpcion;
            mysql_query(conexionDB, q.c_str());
        }

        // D. Actualizar Cuál es la Correcta (name="correcta_p_123", value="ID_OPCION_CORRECTA")
        else if (nombre.find("correcta_p_") == 0) {
            std::string idPregunta = nombre.substr(11);
            std::string idOpcionCorrecta = valor;

            // Primero ponemos todas las de esa pregunta en FALSE
            std::string qReset = "UPDATE opciones_respuesta SET es_correcta=0 WHERE pregunta_id=" + idPregunta;
            mysql_query(conexionDB, qReset.c_str());

            // Luego ponemos la seleccionada en TRUE
            std::string qSet = "UPDATE opciones_respuesta SET es_correcta=1 WHERE id=" + idOpcionCorrecta;
            mysql_query(conexionDB, qSet.c_str());
        }
    }

    // Redirigir al usuario al listado
    std::cout << "<script>alert('Examen actualizado exitosamente.'); window.location.href='http://localhost/PaginaWebLaboratorio/gestionEvaluaciones.html';</script>";
}