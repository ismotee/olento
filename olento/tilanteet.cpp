#include "tilanteet.h"

void tilanteet::lisaa(tilanne uusi) {
    vec_tilanteet.push_back(uusi);
}

void tilanteet::lisaa(std::vector<float> input, std::vector<float> output) {
    vec_tilanteet.push_back(tilanne(input, output));
}

void tilanteet::etsiInputArvojenMukaan(std::vector<float> input) {

    float pieninEro = 100000;
    int indeksi = -1;

    for (int i = 0; i < (int) vec_tilanteet.size(); i++) {
        float vertailu = vec_tilanteet[i].compare(input);

        if (vertailu < pieninEro) {
            pieninEro = vertailu;
            indeksi = i;
        }
    }
    lahinTilanneid = indeksi;
}

std::vector<int> tilanteet::teeLahimpienTilanteidenIndeksiTaulukko(std::vector<float> input) {

    std::vector<int> indeksiTaulukko;
    indeksiTaulukko.resize(input.size(), 0);

    try {
        if (input.size() != 102)
            throw 1;

        for (int j = 0; j < input.size(); j++) {

            float pieninEro = 100000;
            int indeksi = -1;

            for (int i = 0; i < vec_tilanteet.size(); i++) {
                float vertailu = vec_tilanteet[i].compare_one(j, input[j]);

                if (vertailu < pieninEro) {
                    pieninEro = vertailu;
                    indeksiTaulukko[j] = i;
                }
            }

        }

    } catch (int exp) {
        std::cout << "huono taulukko.\n";
    }

    return indeksiTaulukko;
}

std::vector<float> tilanteet::annaVertailuArvot(std::vector<float> vertailtavat) {
    std::vector<float> tulos;

    tulos.resize(vec_tilanteet.size(), 0);

    for (int i = 0; i < vec_tilanteet.size(); i++) {
        tulos[i] = 1 - vec_tilanteet[i].compare(vertailtavat);

        if (tulos[i] < 0)
            tulos[i] = 0;
    }
    return tulos;

}

tilanne tilanteet::annaLahinTilanne() {
    if (lahinTilanneid == -1) {
        std::cout << "tilanteet: ei tilanne dataa\n";
        return tilanne();
    }

    return vec_tilanteet[lahinTilanneid];
}

tilanne tilanteet::annaViimeisinTilanne() {
    return vec_tilanteet.back();
}

tilanne tilanteet::annaTilanne(int i) {
    if (i < koko())
        return vec_tilanteet[i];

    std::cout << "tilanteet: annaTilanne: indeksi liian iso\n";
    return tilanne();
}

void tilanteet::lataa(std::string tiedosto) {

    std::ifstream is(tiedosto);

    if (!is.is_open()) {
        std::cout << "tilanteet: lataa: tiedostoa ei loydy\n";
        return;
    }

    while (is.good()) {
        std::string temp;
        std::string data;

        std::getline(is, data);
        std::getline(is, temp);

        data = data + "\n" + temp + "\n";

        if (data != "\n\n")
            vec_tilanteet.push_back(tilanne(data));

    }

}

void tilanteet::tallenna(std::string tiedosto) {

    std::ofstream os(tiedosto);

    if (!os.is_open()) {
        std::cout << "tilanteet: tallenna: tiedostoa ei voi avata\n";
        return;
    }

    for (int i = 0; i < (int) vec_tilanteet.size(); i++) {
        os << vec_tilanteet[i].toString();
    }

    os.close();

}

bool tilanteet::tyhja() {
    return vec_tilanteet.empty();
}

void tilanteet::laskeGeenit() {

    for (int i = 0; i < vec_tilanteet.size(); i++) {
        std::cout << "tilanne " << i << "\n";
        vec_tilanteet[i].epoch();
    }
}

int tilanteet::annaLahinTilanneId() {
    return lahinTilanneid;
}

void tilanteet::jarjestaLahinTilanne(std::vector<float> input) {
    vec_tilanteet[lahinTilanneid].jarjestaGeenit(input);
}

std::vector<int> tilanteet::lahimmatTilanteetOutputinMukaan(int maara, std::vector<float> out) {
    std::vector<int> lista;

    std::vector<float> vertaillut;
    vertaillut.resize(vec_tilanteet.size(), -1);
    
    for(int i = 0; i < vec_tilanteet.size() ; i++)
        vertaillut[i] = vec_tilanteet[i].compare_out(out);
    
    while (lista.size() < maara) {
        int id = -1;
        float vertailu = 100000;
        
        for (int i = 0; i < vertaillut.size(); i++) {
            
            if(vertaillut[i] < vertailu) {
                bool listassa = false;
                
                for(int j = 0; j < lista.size();j++) {
                    if(lista[j] == i) 
                        listassa = true;
                }
                
                if(!listassa) {
                    vertailu = vertaillut[i];
                    id = i;
                }
            }
        }
        std::cout << "lista indeksi: " << id << "\n";
        lista.push_back(id);
    }
    return lista;
}

tilanne tilanteet::lahinTilanneListasta(std::vector<int> lista, std::vector<float> input) {
    
    float pieninEro = 100000;
    int indeksi = -1;

    for (int i = 0; i < lista.size(); i++) {
        float vertailu = vec_tilanteet[lista[i]].compare(input);

        if (vertailu < pieninEro && lista[i] != -1 && vertailu != -1) {
            pieninEro = vertailu;
            indeksi = i;
        }
    }
    lahinTilanneid = indeksi;
    return vec_tilanteet[indeksi];
}