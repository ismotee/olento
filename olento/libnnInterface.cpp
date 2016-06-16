#include "libnnInterface.h"
#include <iostream>
#include "dClock.h"
#include "random.h"
#include "archiver.h"

namespace nnInterface {
    const char* TILANTEET_PATH = "resources/tilanteet.net";
    
    std::vector<NNet> nn_nets;
    
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

    int neighbours_n = 8;

    std::mutex mtx;
    
    dClock timer;

    int nets = 8;
    int in = 102;
    int hid = 0;
    int out = 1;

    void setNeighbours_n(int value) {
    	if(value > 0)
    		neighbours_n = value;
    }

    
    void Init()
    {
        //std::vector<tilanne>* tilanneLoad = new std::vector<tilanne>();
        //Archiver::load((char*)tilanneLoad, sizeof(tilanne), "/Users/ismotorvinen/Documents/3d/olento/tilanteet.net");

        nn_nets.resize(nets,NNet());

        std::string loaded = Archiver::loadString(TILANTEET_PATH);
        std::cout << "tilanteet ladattu tiedostosta. Rimpsun mitta: " << loaded.length() << "\n";
       
        if(!loaded.empty()) {
            std::vector<std::string> ladatutTilanteet = parsiTilanteet(loaded);
            tilanteet.clear();

            for(int i=0; i<ladatutTilanteet.size(); i++) {
                tilanteet.push_back(tilanne());
                tilanteet.back().fromString(ladatutTilanteet[i]);
            }

            std::cout << "tilanteita: " << tilanteet.size() << "\n";

        }
        
        // count num_inputs && num_hidden_neurons dynamically
        nn_desired_out = std::vector<float>(nets*out, 0.5f);
        nn_input = std::vector<float>(in, 0.5f);
        nn_output = std::vector<float>(nets*out, 0.5f);
        result = std::vector<float>(nets*out, 0.5f);


                
    for (int i = in; i > 0; i--)
        hid += i;

    for(int k = 0; k < nn_nets.size(); k++) {    

        nn_nets[k].init(in, 1, hid, out);
        
        //tee linkitykset
        	int reduction = 0;
        	for (int i = 0; i < in; i++) {

            	reduction += i;
            	for (int j = 0; j < in; j++) {
					nn_nets[k].link(0, i, 1, i*in + j - reduction);	
	                nn_nets[k].link(0, j, 1, i*in + j - reduction);
          		}
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
            	for(int i = 0; i < nn_nets.size();i++) {
            		std::vector<float> dOut;
            		dOut.push_back(nn_desired_out[i]);
                	nn_nets[i].back(dOut);
                }
                //voisi kirjoittaa tiedostoon t‰ss‰ v‰liss‰
                desiredWritten = false;
            }
            mtx.unlock();
            
            std::this_thread::yield();
            
            mtx.lock();
            // feed forward
            if (inWritten == true) {
            	for(int i = 0; i < nn_nets.size(); i++)
            		nn_output[i] = nn_nets[i].forward(nn_input)[0];
                if (nn_output.size() > out*nets) {
                    nn_output.resize(out*nets);
                    std::cout << "liian iso output\n";
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
        Archiver::saveString(tilanteet.back().toString(), TILANTEET_PATH);
        std::cout << "tilanne tehty!\n";
    }
    
    void LaskeDesiredOut (std::vector<float> nykyinenPaikka)
    {
        std::vector<float> erot(tilanteet.size());
        
        if(nn_input.size() == in) {
            for(int i = 0; i < tilanteet.size(); i++) {
                erot[i] = vektorienEro(nn_input, tilanteet[i].inputData);
                if(erot[i] == -1)
                	std::cout << "nnInterface, LaskeDesiredOut: V‰‰r‰ vektorikoko: "<< nn_input.size() << " " << tilanteet[i].inputData.size() << "\n";
            }

            std::vector <int> jarjestetytIdt = jarjestaTaulukkoPienimmastaSuurimpaan(erot);

            std::vector <std::vector<float> > laskettuSuunta;

            for(int i = 0; i < tilanteet.size() && i < neighbours_n; i++) //muuta t‰m‰ 8 muuttujaksi
            	laskettuSuunta.push_back(tilanteet[jarjestetytIdt[i]].desiredOutData);

           	std::vector<float> summattu(laskettuSuunta[0].size());

           	int additiveId = 1;

           	for(int i = 1; i < tilanteet.size() && i < neighbours_n; i++) { //ja t‰m‰
           		additiveId+=i;
           		for(int j = 0; j < nn_desired_out.size(); j++) {

           			//summattu[j] += (laskettuSuunta[i][j]- summattu[j]) / (additiveId *2 + erot[i]) ;
           			summattu[j] += (laskettuSuunta[i][j]- summattu[j]) / (additiveId * (2 + erot[i]) ) ;
           		}
           	}

          	for(int i = 0; i<summattu.size();i++) {
           		summattu[i] /= tilanteet.size();
           		summattu[i] += laskettuSuunta[0][i]; 
           	}

            for(int i = 0; i < nykyinenPaikka.size(); i++) {
                nn_desired_out[i] =  (summattu[i] - nykyinenPaikka[i]);
            }
            
            float desiredPituus = VectorLength(nn_desired_out);
            
            for(int i = 0; i < nn_desired_out.size(); i++) {
                nn_desired_out[i] *= desiredPituus;
                bound(nn_desired_out[i],-0.9,0.9);
                nn_desired_out[i] = (nn_desired_out[i] + 1) * 0.5f;
            }
            //std::cout << "\n";
            
            desiredWritten = true;
        }
        else
            std::cout << "vaaran kokoinen nn_input: " << nn_input.size();
        
        //std::cout << "\n";
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

            tilanneStr = "\n";

            while(line.compare("start") !=0 && inss.good()) {
                tilanneStr += line + "\n";
                std::getline(inss, line);
            }
            
            result.push_back(tilanneStr);
        }
        return result;
    }
}