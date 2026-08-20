#include "RetroEngine.hpp"

// Librerías específicas de Nintendo DS/DSi (libnds)
#include <nds.h>
#include <fat.h>
#include <unistd.h>

#if !RETRO_USE_ORIGINAL_CODE

void parseArguments(int argc, char *argv[])
{
    for (int a = 0; a < argc; ++a) {
        const char *find = "";

        find = strstr(argv[a], "stage=");
        if (find) {
            int b = 0;
            int c = 6;
            while (find[c] && find[c] != ';') Engine.startSceneFolder[b++] = find[c++];
            Engine.startSceneFolder[b] = 0;
        }

        find = strstr(argv[a], "scene=");
        if (find) {
            int b = 0;
            int c = 6;
            while (find[c] && find[c] != ';') Engine.startSceneID[b++] = find[c++];
            Engine.startSceneID[b] = 0;
        }

        find = strstr(argv[a], "usingCWD=true");
        if (find) {
            usingCWD = true;
        }
    }
}
#endif

int main(int argc, char *argv[])
{
    // 1. Inicializar los sistemas de la Nintendo DS / DSi
    defaultExceptionHandler();

    // 2. Inicializar NitroFS para leer los archivos empaquetados en la ROM (.nds) o desde la SD
    if (!nitroFSInit(NULL)) {
        // Si NitroFS falla, intentamos respaldo con FAT normal
        fatInitDefault();
    }

    // 3. Cambiar al directorio raíz de NitroFS para que el motor encuentre el Data.rsdk
    chdir("nitro:/");

#if !RETRO_USE_ORIGINAL_CODE
    parseArguments(argc, argv);
#endif

    // 4. Arrancar el motor Retro Engine de Sonic CD
    Engine.Init();
    Engine.Run();

    return 0;
}

#if RETRO_PLATFORM == RETRO_WIN && _MSC_VER
        FreeConsole();
#endif
    }
#endif

    return 0;
}
