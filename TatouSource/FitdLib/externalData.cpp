// Local Linux build: use the user's original PAK/ITD files and release HDA
// archives instead of compiling the distribution's embedded game-data arrays.
#include "embedded/embeddedData.h"

bool getEmbeddedFile(const char*, const unsigned char** data, size_t* size)
{
    if (data) *data = nullptr;
    if (size) *size = 0;
    return false;
}

bool getEmbeddedAitd1File(const char* name, const unsigned char** data, size_t* size)
{
    return getEmbeddedFile(name, data, size);
}

bool getEmbeddedJackFile(const char* name, const unsigned char** data, size_t* size)
{
    return getEmbeddedFile(name, data, size);
}
