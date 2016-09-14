#include "oModificators.h"


oModificators::oModificators()
{}

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

void oModificators::load(std::string path, std::string _archtype, std::vector<std::string> modificators)
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


oMeshBundle& oModificators::operator[] (std::size_t idx) { return mods[idx]; };



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
