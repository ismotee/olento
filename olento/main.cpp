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
  std::vector<float> desiredOutput(8, 0.5f);
  
  const float koulutus_up = 0.99;
  const float koulutus_down = 0.01;
  bool koulutetaan = true;
  
  SDL_Event e;
  bool running = true;
  
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
      muodonArvot = theNet.forward(inputs);
      if(koulutetaan) theNet.back(desiredOutput);
      
      //skaalaa luvut olennon käyttöön!
      muodonArvot[0] *= 3;
      for (int i = 1; i < 5; i++) muodonArvot[i] = muodonArvot[i] * 2 - 1;
      muodonArvot[5] *= 5;
      
      //muuta muotoa gl:ään
      asetaMuoto(muodonArvot);
      olentoServer::asetaVastausviesti(muodonArvot);
    }
    
    //ajasta luuppi
    t.delay(2);
    //std::cout << frame++ << "\n";
    
    while(SDL_PollEvent(&e)) {
      if(e.type == SDL_QUIT) running = false;
      
      if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
	switch(e.key.keysym.sym) {
	  case SDLK_q:
	    if(e.type == SDL_KEYDOWN) desiredOutput[0] = koulutus_up;
	    else desiredOutput[0] = 0.5;
	    break;
	  case SDLK_a:
	    if(e.type == SDL_KEYDOWN) desiredOutput[0] = koulutus_down;
	    else desiredOutput[0] = 0.5;
	    break;  
	  case SDLK_w:
	    if(e.type == SDL_KEYDOWN) desiredOutput[1] = koulutus_up;
	    else desiredOutput[0] = 0.5;
	    break;
	  case SDLK_s:
	    if(e.type == SDL_KEYDOWN) desiredOutput[1] = koulutus_down;
	    else desiredOutput[0] = 0.5;
	    break;  
	  case SDLK_e:
	    if(e.type == SDL_KEYDOWN) desiredOutput[2] = koulutus_up;
	    else desiredOutput[0] = 0.5;
	    break;
	  case SDLK_d:
	    if(e.type == SDL_KEYDOWN) desiredOutput[2] = koulutus_down;
	    else desiredOutput[0] = 0.5;
	    break;  
	  case SDLK_r:
	    if(e.type == SDL_KEYDOWN) desiredOutput[3] = koulutus_up;
	    else desiredOutput[0] = 0.5;
	    break;
	  case SDLK_f:
	    if(e.type == SDL_KEYDOWN) desiredOutput[3] = koulutus_down;
	    else desiredOutput[0] = 0.5;
	    break;  
	  case SDLK_t:
	    if(e.type == SDL_KEYDOWN) desiredOutput[4] = koulutus_up;
	    else desiredOutput[0] = 0.5;
	    break;
	  case SDLK_g:
	    if(e.type == SDL_KEYDOWN) desiredOutput[4] = koulutus_down;
	    else desiredOutput[0] = 0.5;
	    break;  
	  case SDLK_y:
	    if(e.type == SDL_KEYDOWN) desiredOutput[5] = koulutus_up;
	    else desiredOutput[0] = 0.5;
	    break;
	  case SDLK_h:
	    if(e.type == SDL_KEYDOWN) desiredOutput[5] = koulutus_down;
	    else desiredOutput[0] = 0.5;
	    break;  
	  case SDLK_u:
	    if(e.type == SDL_KEYDOWN) desiredOutput[6] = koulutus_up;
	    else desiredOutput[0] = 0.5;
	    break;
	  case SDLK_j:
	    if(e.type == SDL_KEYDOWN) desiredOutput[6] = koulutus_down;
	    else desiredOutput[0] = 0.5;
	    break;  
	  case SDLK_i:
	    if(e.type == SDL_KEYDOWN) desiredOutput[7] = koulutus_up;
	    else desiredOutput[0] = 0.5;
	    break;
	  case SDLK_k:
	    if(e.type == SDL_KEYDOWN) desiredOutput[7] = koulutus_down;
	    else desiredOutput[0] = 0.5;
	    break;  
	  case SDLK_RETURN:
	    if(e.type == SDL_KEYDOWN) koulutetaan = false;
	    break;
	}	
      }
    }    
    
    for(int i=0; i<8; i++) {
      std::cout << muodonArvot[i] << ", ";
    }    
    std::cout << "\n";
    
  } while(running);
  
  std::cerr << "olentogl suljettiin\n";
  
  olentoServer::lopeta();
  suljeOlento();
  olentogl.join();
  
  return 0;
}