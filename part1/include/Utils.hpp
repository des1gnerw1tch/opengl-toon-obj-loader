#pragma once

#include <string>
#include <iostream>

class Utils {
public:
    std::string inline static GetDirectoryOfFile(std::string file)    {
        size_t locationOfLastSlash = file.find_last_of("/");
        if (locationOfLastSlash == std::string::npos)  {
            return "./";
        }
        return file.substr(0, locationOfLastSlash + 1);
    }
};