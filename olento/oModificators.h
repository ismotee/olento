#ifndef OMODIFICATORS_H
#define OMODIFICATORS_H

#include <vector>

#include "oMeshBundle.h"
#include "oDirectory.h"
#include <iostream>

struct oModificators {
    oMeshBundle* archtypes;
    std::vector<oMeshBundle> mods;
    
    oModificators();
    oModificators(std::string path, std::string _archtype, std::vector<std::string> modificators);
    void load(std::string path, std::string _archtype, std::vector<std::string> modificators);
    std::vector<glm::vec3> getShape(std::vector<float>& values);
    
    //making easier and more intuitive to get data from mods vector
    oMeshBundle& operator[](std::size_t idx);
    
};
#endif