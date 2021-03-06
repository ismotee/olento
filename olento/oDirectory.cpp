#include "oDirectory.h"
#include <algorithm>
#include <sys/types.h>

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
        std::sort(files.begin(),files.end());
    } else {
        /* could not open directory */
        std::cout << "could not open directory " << path << "\n";
    }
    
}

