#ifndef OLENTO_LIBNNINTERFACE_H
#define OLENTO_LIBNNINTERFACE_H

#include <stdio.h>
#include "libnn.h"
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>
#include <iostream>

/*
 
 HiiriNNet:
 
 hiirenXY
 |
 v
 hiiriNNet
 |
 v
 liikeXY ja koonMuutos
 |
 v
 skaalaa (-1,1)
 |
 v
 muuta paikka & bound
 |
 v
 laske toivottu paikka
 |
 v
 NNet back()
 
 
 
 
 NNetin Katsellaan rutiini:
 
 inputit
 |
 v
 ryhmät? (vector<NNet>(groups_n,NNet(ValikoituInputJoukko, 1, kaikki_inputtien_yhdistelmä, ))  )
 |
 v
 tunneNNet (NNet(groups_n, 1, kaikki_groups_yhdistelmät, 8))
 |
 |
 v
 muodonLiike (vector<float>(8))
 |
 v
 skaalaa (-1,1)
 |
 v
 lisää MuodonArvoihin
 |
 v
 laske Desired output
 |
 v
 NNet back()
 
 Kuinka lasketaan DesireOut:
 
 - vertaa Inputtia Tilanteiden Inputtiin
 - valitse Inputteja lähin tilanteen output
 
 - laske outputin ja nykyisen muodon välinen eroavuus
 - skaalaaLuvut välille(0,1) ja boundaa
 
 
 
 */

namespace nnInterface {
    extern std::mutex mtx;
    
    struct tilanne{
        std::vector<float> inputData;
        std::vector<float> desiredOutData;
        
        tilanne(){}
        tilanne(std::vector<float> input, std::vector<float> desired) : inputData(input), desiredOutData(desired) {}

        std::string toString() {
            std::string vastaus = "start\nI\n";
            for (int i = 0; i < inputData.size(); i++) {
                vastaus += " " + std::to_string(inputData[i]);
            }
            vastaus += "\nD\n";
            for (int i = 0; i < desiredOutData.size(); i++) {
                vastaus += " " + std::to_string(desiredOutData[i]);
            }
            vastaus += "\n";
            return vastaus;
        }
        
        void fromString(std::string str)
        {
            
            std::stringstream ss;
            
            inputData.clear();
            desiredOutData.clear();
            
            ss.str(str);
            std::string the_line;
            
            
            while(!ss.eof() ){
                std::getline(ss, the_line);

                if(the_line.compare("I") == 0) {
                    //seuraavassa rivissä on arvot. Kopioidaan ne omaan streamiin
                    std::getline(ss, the_line);
                    std::stringstream values;
                    values.str(the_line);

                    float value;
                    //Kopioidaan arvot streamista vektoriin
                    while(values >> value) {
                        inputData.push_back(value);
                        std::cout << inputData.back() << " ";
                    }
               
                }

                std::cout << "\n";
                  
                if(the_line.compare("D") == 0) {
                    std::getline(ss, the_line);
                    std::stringstream values;
                    values.str(the_line);         

                    float value;
                    while(values >> value) {
                        desiredOutData.push_back(value);
                        std::cout << desiredOutData.back() << " ";
                    }
                }
                    
            }
            std::cout << "input vektori: " << inputData.size() << "\n";
            std::cout << "desired vektori: " << desiredOutData.size() << "\n";
        }
        
    };

    std::vector<std::string> parsiTilanteet(std::string inputString);

    void setNeighbours_n(int value);

    void Init();
    
    void StartRoutine();
    
    void SetInput(std::vector<float> input_);
    
    std::vector<float> GetOutput();
    
    void TeeTilanne(std::vector<float> input, std::vector<float> output);
    
    void LaskeDesiredOut(std::vector<float> nykyinenPaikka);
    
    void Close();
    
    void SaveNN(std::string savefile);
    
    void LoadNN(std::string filename);
}
#endif