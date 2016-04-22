//
//  oModificators.h
//  opengltest
//
//  Created by Ismo Torvinen on 8.4.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#ifndef opengltest_oModificators_h
#define opengltest_oModificators_h

#include <vector>

#include "oMeshBundle.h"
#include "olento.h"
#include "oDirectory.h"
#include <iostream>

struct oModificators {
    oMeshBundle* archtypes;
    std::vector<oMeshBundle> mods;
    
    oModificators(std::string path, std::string _archtype, std::vector<std::string> modificators);
    
    std::vector<glm::vec3> getShape(std::vector<float>& values);
    
    //making easier and more intuitive to get data from mods vector
    oMeshBundle& operator[](std::size_t idx)       { return mods[idx]; };
    const oMeshBundle& operator[](std::size_t idx) const { return mods[idx]; };
    
};

oModificators::oModificators (std::string path, std::string _archtype, std::vector<std::string> modificators)
{
    std::string archtypePath = path + _archtype + "/";
    
    oDirectory archtypeDir(archtypePath);
    archtypes = new oMeshBundle(archtypeDir);
    
    for(int i = 0; i < modificators.size(); i++) {
        std::string modPath = path + modificators[i] + "/";
        //std::cout << modPath << "\n";
        oDirectory dir(modPath);
        mods.push_back(oMeshBundle(dir,*archtypes));
    }
    
}

std::vector<glm::vec3> oModificators::getShape(std::vector<float>& values) {

    //if(values.size() != 5) {return std::vector<glm::vec3>();}
    
    
    
    std::vector<glm::vec3> baseShape = archtypes->getShape(values[0]);
    std::vector<glm::vec3> result(baseShape.size());
    
    //std::cout << "result vector size: " << result.size() << "\n";
    
    for(int i = 0; i < values.size() -1; i++) {
        
        std::vector<glm::vec3> plus = mods[i].getShape(values[0], values[i+1]);
        
        //std::cout << "adding mod.. ";
        for(int j = 0; j < plus.size();j++) {
            result[j] += plus[j];
        }
        //std::cout << "done!\n";
    }
/*
    for(int i = 0; i < result.size();i++) {
        result[i] /=  values.size() - 1;
    }
  */ 
    for(int i = 0; i < result.size();i++) {
        result[i] += baseShape[i];
    }
    
    //std::cout << "done!";
    return result;
}

#endif
