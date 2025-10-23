#include "GestorRecursos.h"
#include <iostream>

GestorRecursos::GestorRecursos(MYSQL* conexion) : conexionDB(conexion) {}

void GestorRecursos::listarRecursos() {
    std::string query = "SELECT * FROM laboratorioquimica.recursos";
    MYSQL_RES* resultado;
    MYSQL_ROW fila;

    std::cout << "Content-type: text/html\r\n\r\n";

    if (mysql_query(conexionDB, query.c_str()) == 0) {
        resultado = mysql_store_result(conexionDB);

        std::cout << "<h2 style='text-align:center;'>Biblioteca de Recursos Educativos</h2>";
        std::cout << "<table id='tabla-recursos-real' border='1' style='width:100%; border-collapse: collapse;'>";

        std::cout << "<thead style='background-color:#003366; color:white;'>";
        std::cout << "<tr><th>ID</th><th>Título</th><th>Descripción</th><th>Categoría</th><th>Tipo</th><th>Autor</th><th>Palabras clave</th><th>Fecha</th><th>Enlace</th><th>Acciones</th></tr>";
        std::cout << "</thead><tbody>";

        while ((fila = mysql_fetch_row(resultado)) != nullptr) {
            std::string id = fila[0];

            std::cout << "<tr>";
            std::cout << "<td>" << (fila[0] ? fila[0] : "N/A") << "</td>"; // id
            std::cout << "<td>" << (fila[1] ? fila[1] : "N/A") << "</td>"; // titulo
            std::cout << "<td>" << (fila[2] ? fila[2] : "N/A") << "</td>"; // descripcion
            std::cout << "<td>" << (fila[3] ? fila[3] : "N/A") << "</td>"; // categoria
            std::cout << "<td>" << (fila[4] ? fila[4] : "N/A") << "</td>"; // tipo
            std::cout << "<td>" << (fila[5] ? fila[5] : "N/A") << "</td>"; // autor
            std::cout << "<td>" << (fila[6] ? fila[6] : "N/A") << "</td>"; // palabras_clave
            std::cout << "<td>" << (fila[7] ? fila[7] : "N/A") << "</td>"; // fecha
            std::cout << "<td><a href='" << (fila[8] ? fila[8] : "#") << "' target='_blank'>Ver recurso</a></td>"; // enlace

            std::cout << "<td>"
                << "<a href='/cgi-bin/PaginaWebLaboratorio.exe?accion=eliminarr&id=" << id
                << "' onclick=\"return confirm('¿Estás seguro de eliminar este recurso?');\">"
                << "<button style='background-color:#dc3545; color:white; border:none; padding:6px 12px; border-radius:4px; cursor:pointer;'>Eliminar</button></a>"
                << "</td>";
            std::cout << "</tr>";
        }
        std::cout << "</tbody></table>";
        mysql_free_result(resultado);
    }
    else {
        std::cout << "<p>Error al ejecutar la consulta SQL: " << mysql_error(conexionDB) << "</p>";
    }
}

void GestorRecursos::eliminarRecurso(const std::string& id) {
    std::string consulta = "DELETE FROM laboratorioquimica.recursos WHERE id = " + id;

    if (mysql_query(conexionDB, consulta.c_str()) == 0) {
        std::cout << "Status: 302 Found\r\n";
        std::cout << "Location: http://localhost/PaginaWebLaboratorio/contenidoEducativo.html\r\n\r\n";
    }
    else {
        std::cout << "Content-type: text/html\r\n\r\n";
        std::cout << "<html><body><h2>Error al eliminar el recurso: " << mysql_error(conexionDB) << "</h2></body></html>";
    }
}

void GestorRecursos::registrarRecurso(const std::string& titulo, const std::string& descripcion, const std::string& categoria,
    const std::string& tipo, const std::string& autor, const std::string& palabrasClave,
    const std::string& fecha, const std::string& enlace, cgicc::Cgicc& formulario) {

    std::string enlaceFinal = enlace; // Por defecto es el link del formulario

    if (tipo == "guias") {
        cgicc::const_file_iterator archivo = formulario.getFile("enlace"); // 'enlace' es el nombre del campo file en el HTML
        if (archivo != formulario.getFiles().end()) {
            std::string nombreArchivo = archivo->getFilename();
            nombreArchivo = Utilidades::sanitizarNombreArchivo(nombreArchivo); // Usar la función de utilidad

            std::string rutaGuardado = "C:\\Apache24\\htdocs\\PaginaWebLaboratorio\\Guias\\" + nombreArchivo; // Ajustar ruta según tu servidor

            // Guardar archivo en servidor
            std::ofstream out(rutaGuardado, std::ios::binary);
            if (out.is_open()) {
                archivo->writeToStream(out);
                out.close();
                // URL relativa para guardar en BD
                enlaceFinal = "/PaginaWebLaboratorio/Guias/" + nombreArchivo;
            }
            else {
                std::cout << "Content-type: text/html\r\n\r\n";
                std::cout << "<html><body><h2>Error: No se pudo abrir el archivo para guardar en el servidor. Revise permisos y ruta.</h2></body></html>";
                return;
            }
        }
        else {
            std::cout << "Content-type: text/html\r\n\r\n";
            std::cout << "<html><body><h2>No se recibió archivo para una guía.</h2></body></html>";
            return;
        }
    }

    std::string query = "INSERT INTO laboratorioquimica.recursos (titulo, descripcion, categoria, tipo, autor, palabras_clave, fecha, enlace) VALUES ('" +
        titulo + "', '" +
        descripcion + "', '" +
        categoria + "', '" +
        tipo + "', '" +
        autor + "', '" +
        palabrasClave + "', '" +
        fecha + "', '" +
        enlaceFinal + "')";

    std::cout << "Content-type: text/html\r\n\r\n";

    if (mysql_query(conexionDB, query.c_str()) == 0) {
        std::cout << "<html><head><script>";
        std::cout << "alert('✅ Registro exitoso');";
        std::cout << "window.location.href = 'http://localhost/PaginaWebLaboratorio/contenidoEducativo.html';";
        std::cout << "</script></head><body></body></html>";
    }
    else {
        std::cout << "<html><head><script>";
        std::cout << "alert('❌ Error al registrar: " << mysql_error(conexionDB) << "');";
        std::cout << "window.history.back();";
        std::cout << "</script></head><body></body></html>";
    }
}