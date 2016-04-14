//
//  oMeshBundle.h
//  opengltest
//
//  Created by Ismo Torvinen on 27.3.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#ifndef opengltest_oMeshBundle_h
#define opengltest_oMeshBundle_h

#include "olento.h"
#include <vector>
#include "oMesh.h"
#include "oDirectory.h"

class oMeshBundle {
public:
    std::vector<oMesh> meshes;
    
    oMeshBundle(oDirectory& dir);
    
    
    //making easier and more intuitive to get data from vector
    oMesh& operator[](std::size_t idx)       { return meshes[idx]; };
    const oMesh& operator[](std::size_t idx) const { return meshes[idx]; };

};

oMeshBundle::oMeshBundle (oDirectory& dir)
{
    for(int i = 0; i < dir.files.size(); i++)
        meshes.push_back(oMesh(dir.files[i]));

}


#endif
