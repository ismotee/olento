#ifndef opengltest_oDirectory_h
#define opengltest_oDirectory_h

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

oDirectory::oDirectory (std::string _path): path(_path)
{
    
    /* print contents of directories listed in command line */
    DIR *dir;
    struct dirent *ent;
    /* open directory stream */
    dir = opendir(path.c_str());
    if (dir != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_name[0] != '.' && ent->d_name[strlen(ent->d_name)-1] != '~') {
/*                switch (ent->d_type) {
                    case DT_REG:
                            files.push_back(path + ent->d_name);
                        break;
                    case DT_DIR:
                        // Directory
                        //directories.push_back(oDirectory(path + ent->d_name));
                        break;
                }
*/                
				files.push_back(path + ent->d_name);
            }
        }
            
        closedir (dir);
    } else {
        /* could not open directory */
        //std::cout << "could not open directory " << path << "\n";
    }
    
}


#endif
