#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <string>
#include <algorithm>
#include <map>
#include <ctime>
#include <cstdlib> // Para rand y srand
#include <cctype>  // Para isalnum

namespace Utilidades {
    std::string sanitizarNombreArchivo(const std::string& nombreArchivo);
    std::string generarToken();
}

#endif // UTILIDADES_H