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
    
    std::vector<oMeshBundle> mods;
    
    oModificators(std::string path, std::vector<std::string> modificators);
    
    
    //making easier and more intuitive to get data from mods vector
    oMeshBundle& operator[](std::size_t idx)       { return mods[idx]; };
    const oMeshBundle& operator[](std::size_t idx) const { return mods[idx]; };
    
};

oModificators::oModificators (std::string path, std::vector<std::string> modificators)
{
    for(int i = 0; i < modificators.size(); i++) {
        std::string modPath = path + modificators[i] + "/";
        std::cout << modPath << "\n";
        oDirectory dir(modPath);
        mods.push_back(oMeshBundle(dir));
    }
    
}


#endif
