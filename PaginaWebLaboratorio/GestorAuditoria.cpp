#include "GestorAuditoria.h"

GestorAuditoria::GestorAuditoria(MYSQL* conexion) : conexionDB(conexion) {}

void GestorAuditoria::registrarAuditoria(const std::string& idUsuario, const std::string& tabla, const std::string& accion, const std::string& descripcion) {
    std::string query = "INSERT INTO laboratorioquimica.auditoria (usuario_id, tabla, accion, descripcion) VALUES ('" +
        idUsuario + "', '" + tabla + "', '" + accion + "', '" + descripcion + "')";

    if (mysql_query(conexionDB, query.c_str()) != 0) {
        std::cout << "<script>alert('Error en auditoría: " + std::string(mysql_error(conexionDB)) + "');</script>";
    }
}

void GestorAuditoria::listarAuditorias() {
    std::string query = "SELECT a.id, u.usuario, a.tabla, a.accion, a.descripcion, a.fecha_hora "
        "FROM laboratorioquimica.auditoria a "
        "INNER JOIN laboratorioquimica.usuarios u "
        "ON a.usuario_id = u.id ORDER BY a.fecha_hora DESC";

    MYSQL_RES* resultado;
    MYSQL_ROW fila;
    std::cout << "Content-type: text/html\r\n\r\n";

    if (mysql_query(conexionDB, query.c_str()) == 0) {
        resultado = mysql_store_result(conexionDB);

        std::cout << "<h2 style='text-align:center;'>Registro de Auditoría</h2>";
        std::cout << "<table border='1' style='width:100%; border-collapse: collapse; margin-top: 20px;'>";
        std::cout << "<thead style='background-color:#003366; color:white;'>";
        std::cout << "<tr><th>ID</th><th>Usuario</th><th>Tabla Afectada</th><th>Acción</th><th>Descripción</th><th>Fecha y Hora</th></tr>";
        std::cout << "</thead><tbody>";

        while ((fila = mysql_fetch_row(resultado)) != nullptr) {
            std::cout << "<tr>";
            std::cout << "<td>" << (fila[0] ? fila[0] : "N/A") << "</td>";
            std::cout << "<td>" << (fila[1] ? fila[1] : "N/A") << "</td>";
            std::cout << "<td>" << (fila[2] ? fila[2] : "N/A") << "</td>";
            std::cout << "<td>" << (fila[3] ? fila[3] : "N/A") << "</td>";
            std::cout << "<td>" << (fila[4] ? fila[4] : "N/A") << "</td>";
            std::cout << "<td>" << (fila[5] ? fila[5] : "N/A") << "</td>";
            std::cout << "</tr>";
        }

        std::cout << "</tbody></table>";
        mysql_free_result(resultado);
    }
    else {
        std::cout << "<p>Error al ejecutar la consulta de auditoría: " << mysql_error(conexionDB) << "</p>";
    }
}