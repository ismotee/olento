//
//  archiver.h
//  libnn
//
//  Created by Ismo Torvinen on 24.5.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#ifndef __libnn__archiver__
#define __libnn__archiver__

#include <stdio.h>
#include <string>

namespace Archiver {
    void load(char* start, size_t size, std::string filename);
    void save(char* start, size_t size, std::string filename);
    void saveString(std::string data, std::string filename);
}


#endif /* defined(__libnn__archiver__) */
