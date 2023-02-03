#include "file.h"
#include <fstream>
#include "../screen.h"

void loadJson(const char *filename, Json::Value *data) 
{
    std::ifstream file(filename);
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    parseFromStream(builder, file, data, &errs);

    if (!*data)
    {
        ErrMSG( "FAILED TO FIND JSON: %s", filename);
        return;
    }

    file.close();
}