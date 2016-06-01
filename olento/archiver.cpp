//
//  archiver.cpp
//  libnn
//
//  Created by Ismo Torvinen on 24.5.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#include "archiver.h"
#include <iostream>
#include <fstream>

namespace Archiver {

    void load(char* start,size_t size, std::string filename)
    {
        std::streampos actual_size;
        
        
        std::ifstream file (filename,  std::ios::in|std::ios::binary|std::ios::ate);
        if (file.is_open())
        {
            actual_size = file.tellg();
            file.seekg (0, std::ios::beg);
            file.read (start, actual_size);
            file.close();
            
        } else
            std::cout << "no load\n";
    }
    
    
    void save(char* start, size_t size, std::string filename)
    {
    
        char buffer[size];
        
        
        for(int i = 0; i < size; i++)
            buffer[i] = *(start + i);
        
        
        std::ofstream file(filename, std::ios::out|std::ios::binary);
        
        if (file.is_open())
        {
            file.write (buffer, size);
            file.close();
        }
        else
            std::cout << "file no open\n";
        
    }

    void saveString(std::string data, std::string filename)
    {
        
        std::ofstream file(filename, std::ios::app);
        
        if (file.is_open())
        {
            file << data;
            file.close();
        }
        else
            std::cout << "file no open\n";
        
    }

    
}