#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include "tilanne.h"

class tilanteet {
private:
    std::vector<tilanne> vec_tilanteet;
    int lahinTilanneid;
    
public:
    
    tilanteet(): lahinTilanneid(-1) {}
    
    void lisaa(tilanne uusi);
    void lisaa(std::vector<float> input, std::vector<float> output);
    bool tyhja();
    int koko() {return (int)vec_tilanteet.size();}

    void etsiInputArvojenMukaan(std::vector<float> input);
    std::vector<int> teeLahimpienTilanteidenIndeksiTaulukko(std::vector<float> input);
    tilanne annaLahinTilanne();
    int annaLahinTilanneId();
    tilanne annaViimeisinTilanne();
    tilanne annaTilanne(int i);
    std::vector<float> annaVertailuArvot(std::vector<float> vertailtavat);
    void jarjestaLahinTilanne (std::vector<float> input);
    std::vector<int> lahimmatTilanteetOutputinMukaan(int maara, std::vector<float> out);
    tilanne lahinTilanneListasta(std::vector<int> lista, std::vector<float> input);
    
    
    void laskeGeenit();
    
    void lataa(std::string tiedosto);
    void tallenna(std::string tiedosto);
    
};