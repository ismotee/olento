#ifndef ODIRECTORY_H
#define ODIRECTORY_H

#include <vector>


#ifdef _WIN32
#include "direntwin.h"
#else
#include <dirent.h>
#endif


#include <string.h>
#include <stdio.h>
#include <iostream>

class oDirectory {
public:
    std::string path;
    std::vector<std::string> files;
    //std::vector<oDirectory> directories;
    
    oDirectory (std::string path);
    
    

};

#endif
