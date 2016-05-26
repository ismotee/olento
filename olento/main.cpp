//main.cpp
#include "olento.h"
#include "random.h"
#include "dClock.h"
#include "server.h"
#include "libnn.h"

#include <thread>
#include <vector>
#include <iostream>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
  
  std::vector<float> muodonArvot;
  muodonArvot.resize(8);
  
  olentoServer::aloita(); //käynnistyy omaan threadiin
  
  std::thread olentogl(run);
  
  dClock t;
  int frame = 0;
  
  NNet theNet;
  
  //linkitä input neuronit
  
  int input_n = 144;
  int hiddenNeuron_n = 0;
  
  for(int i=input_n; i>0; i--) {
    hiddenNeuron_n += i;
  }
  
  theNet.init(input_n, 1, hiddenNeuron_n, 8);
  int reduction =0;
  
  for(int i=0; i<input_n; i++) {
    reduction += i;
    for(int j=0; j<input_n; j++) {
      theNet.link(0,i,1,i*input_n + j - reduction);
      theNet.link(0,j,1,i*input_n + j - reduction);
    }
  }
  
  std::vector<float> inputs(input_n, 0.1f);
   
  do {
    
    t.reset(); //ota aikaa
    
    //hae paketti
    olentoServer::pakettiDataT p = olentoServer::haePaketti();
    //if (!p.empty()) {
    {
      /*std::cout << "Paketti:\n";
       *     tulostaPaketti(p);*/
      
      /*inputs = olentoServer::annaPisteet(p);
       *     if(inputs.size() != input_n) {
       * std::cerr << "Väärän kokoinen paketti (" << inputs.size() << ")! Muutetaan\n";
       * inputs.resize(input_n);
    }
    */   
      //luo paketista arvot neuralnetin avulla...
		if (!katsellaanko() ) {
			//jos ei katsella niin muokataan muotoa ja voidaan kouluttaa -- luodaan muoto koulutusarvoista
			muodonArvot = haeKoulutusarvot();
			if (koulutetaanko) {
				theNet.forward(inputs);
				theNet.back(haeKoulutusarvot());
			}
		}
		else {
			//katsellaan NNetin tuottamaa muotoa -- luodaan muoto NNetin antamista arvoista
			muodonArvot = theNet.forward(inputs);
		}

      //muuta muotoa gl:ään
      asetaMuoto(muodonArvot);
      olentoServer::asetaVastausviesti(muodonArvot);
    }
    
    //ajasta luuppi
    t.delay(20);
    //std::cout << frame++ << "\n";  
    
    for(int i=0; i<8; i++) {
      std::cout << muodonArvot[i] << ", ";
    }    
    std::cout << "\n";
    
  } while(olentoPyorii() );
  
  std::cerr << "olentogl suljettiin\n";
  
  olentoServer::lopeta();
  suljeOlento();
  olentogl.join();
  
  return 0;
}