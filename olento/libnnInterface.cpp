
#include "libnnInterface.h"
#include <iostream>
#include "dClock.h"
#include "random.h"
#include "archiver.h"

namespace nnInterface {
    
    NNet nn_net;
    
    std::vector<tilanne> tilanteet;
    
    std::vector<float> nn_output;
    std::vector<float> result;
    
    std::vector<float> nn_desired_out;
    std::vector<float> nn_input;
    
    std::atomic<bool> outRead;
    std::atomic<bool> inWritten;
    std::atomic<bool> desiredWritten;
    std::atomic<bool> nn_stop;
    std::atomic<bool> update;
    
    std::mutex mtx;
    
    dClock timer;
    
    int in = 102;
    int hid = 0;
    int out = 8;
    
    void Init()
    {
        //std::vector<tilanne>* tilanneLoad = new std::vector<tilanne>();
        //Archiver::load((char*)tilanneLoad, sizeof(tilanne), "/Users/ismotorvinen/Documents/3d/olento/tilanteet.net");
        
        const char* TILANTEET_PATH = "/Users/ismotorvinen/Documents/3d/olento/tilanteet.net";
        
        std::string loaded = Archiver::loadString(TILANTEET_PATH);
        std::cout << "tilanteet ladattu tiedostosta. Rimpsun mitta: " << loaded.length() << "\n";
       
        if(!loaded.empty()) {
            std::vector<std::string> ladatutTilanteet = parsiTilanteet(loaded);
            std::cout << "tilanteita: " << ladatutTilanteet.size() << "\n";
            
            tilanteet.resize(ladatutTilanteet.size());
            
            for(int i=0; i<ladatutTilanteet.size(); i++) {
                tilanteet[i].fromString(ladatutTilanteet[i]);
            }
        }
        
        // count num_inputs && num_hidden_neurons dynamically
        nn_desired_out = std::vector<float>(out, 0.5f);
        nn_input = std::vector<float>(in, 0.5f);
        nn_output = std::vector<float>(out, 0.5f);
        result = std::vector<float>(out, 0.5f);
        
        for (int i = in; i > 0; i--)
            hid += i;
        
        nn_net.init(in, 1, hid, out);
        
        //tee linkitykset
        int reduction = 0;
        for (int i = 0; i < in; i++) {
            reduction += i;
            for (int j = 0; j < in; j++) {
                nn_net.link(0, i, 1, i*in + j - reduction);
                nn_net.link(0, j, 1, i*in + j - reduction);
            }
        }
        
    }
    
    void StartRoutine() {
        
        // aloita looppi
        nn_stop = false;
        
        while (!nn_stop) {
            // back propagation
            mtx.lock();
            if (desiredWritten == true) {
                nn_net.back(nn_desired_out);
                //voisi kirjoittaa tiedostoon t‰ss‰ v‰liss‰
                desiredWritten = false;
            }
            mtx.unlock();
            
            std::this_thread::yield();
            
            mtx.lock();
            // feed forward
            if (inWritten == true) {
                nn_output = nn_net.forward(nn_input);
                if (nn_output.size() > out) {
                    nn_output.resize(out);
                    std::cerr << "liian iso output\n";
                }
                outRead = false;
            }
            mtx.unlock();
            std::this_thread::yield();
            
        }
    }
    
    
    void SetInput(std::vector<float> input_)
    {
        nn_input = input_;
        inWritten = true;
    }
    
    
    std::vector<float> GetOutput()
    {
        if (outRead) {
            result.clear();
            return result;
        }
        
        //std::cout << "teach\n";
        
        outRead = true;
        result = nn_output;
        return result;
        
    }
    
    void TeeTilanne (std::vector<float> input, std::vector<float> output)
    {
        tilanteet.push_back(tilanne(input,output));
        Archiver::saveString(tilanteet.back().toString(), "/Users/ismotorvinen/Documents/3d/olento/tilanteet.net");
    }
    
    void LaskeDesiredOut (std::vector<float> nykyinenPaikka)
    {
        std::vector<float> erot(tilanteet.size());
        
        if(nn_input.size() == in) {
            for(int i = 0; i < tilanteet.size(); i++) {
                erot[i] = vektorienEro(nn_input, tilanteet[i].inputData);
            }
            
            float pieninEro = 10000000;
            int lahinTilanneId = 0;
            
            for(int i = 0; i < erot.size(); i++)
                if(erot[i] < pieninEro) {
                    pieninEro = erot[i];
                    lahinTilanneId = i;
                }
            std::cout << "\n";
            
            std::cout << "pieninEro: " << pieninEro << "\n";
            std::cout << "Lahin tilanne: " << lahinTilanneId << "\n";
            std::cout << "desired: " ;
            
            
            
            for(int i = 0; i < nykyinenPaikka.size(); i++) {
                nn_desired_out[i] =  (tilanteet[lahinTilanneId].desiredOutData[i] - nykyinenPaikka[i]);
            }
            
            float desiredPituus = VectorLength(nn_desired_out);
            
            for(int i = 0; i < nn_desired_out.size(); i++) {
                nn_desired_out[i] *= desiredPituus;
                bound(nn_desired_out[i],-1,1);
                nn_desired_out[i] = (nn_desired_out[i] + 1) * 0.5f;
                std::cout << nn_desired_out[i] << " ";
            }
            std::cout << "\n";
            
            desiredWritten = true;
        }
        else
            std::cout << "vaaran kokoinen nn_input: " << nn_input.size();
        
        std::cout << "\n";
    }
    
    void Close() {
        nn_stop = true;
    }
    
    std::vector<std::string> parsiTilanteet(std::string inputString) {
        //Tilanne alkaa start-rivistä ja päättyy seuraavaan start-riviin tai EOF
        //Tungetaan vektoriin tilanne kerrallaan
        
        std::vector<std::string> result;
        std::stringstream inss;
        inss.str(inputString);
        
        //ruvetaan käymään läpi
        while(inss.good() ) {
            std::string line;
            std::string tilanneStr;
            std::getline(inss, line);
            std::cout << "eka while\n";
            while(line.compare("start") !=0 && inss.good()) {
                tilanneStr += line;
                std::getline(inss, line);
                std::cout << "toka while\n";
            }
            
            result.push_back(tilanneStr);
        }
        return result;
    }
    
}