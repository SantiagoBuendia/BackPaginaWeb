#include "Utilidades.h"

namespace Utilidades {
    std::string sanitizarNombreArchivo(const std::string& nombreArchivo) {
        std::string resultado = nombreArchivo;

        // Mapa para reemplazar tildes
        std::map<char, char> reemplazos = {
            {'á', 'a'}, {'Á', 'A'}, {'é', 'e'}, {'É', 'E'},
            {'í', 'i'}, {'Í', 'I'}, {'ó', 'o'}, {'Ó', 'O'},
            {'ú', 'u'}, {'Ú', 'U'}, {'ñ', 'n'}, {'Ñ', 'N'}
        };

        // Reemplazar tildes
        for (auto& c : resultado) {
            auto it = reemplazos.find(c);
            if (it != reemplazos.end()) {
                c = it->second;
            }
        }

        // Reemplazar espacios por guiones medios
        std::replace(resultado.begin(), resultado.end(), ' ', '-');

        // Eliminar caracteres especiales no permitidos en URL
        resultado.erase(std::remove_if(resultado.begin(), resultado.end(), [](char c) {
            return !(std::isalnum(static_cast<unsigned char>(c)) || c == '.' || c == '-' || c == '_');
            }), resultado.end());

        return resultado;
    }

    std::string generarToken() {
        static bool inicializado = false;
        if (!inicializado) {
            srand(static_cast<unsigned int>(time(0)));
            inicializado = true;
        }

        std::string caracteres = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::string token;
        for (int i = 0; i < 32; ++i) {
            token += caracteres[rand() % caracteres.length()];
        }
        return token;
    }
}