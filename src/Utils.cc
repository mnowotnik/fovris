#include <sstream>
#include <fstream>

#include "Utils.h"

#include "Exceptions.h"
namespace fovris {

std::string fileToString(const std::string &path) {
    std::stringstream buffer;
    std::ifstream fs(path, std::fstream::in | std::fstream::binary);

    if (!fs.is_open()) {
        std::string msg("Cannot access file at the given path: ");
        throw FileAccessException(msg.append(path));
    }

    buffer << fs.rdbuf();
    fs.close();

    return buffer.str();
}

std::string fileExtension(const std::string &path){
    size_t idx = path.rfind('.');
    if(idx != std::string::npos)
    {
        return path.substr(idx+1);
    }

    return "";
}

} // fovris
