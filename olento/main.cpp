//main.cpp
#include "olento.h"
#include "random.h"
#include "dClock.h"
#include "server.h"
#include "libnnInterface.h"

#include <thread>
#include <vector>
#include <iostream>
#include <SDL2/SDL.h>
#include <mutex>

enum moodiT {
	KOULUTETAAN, ASETA_TOIVE, MUOKATAAN, KATSELLAAN
};


struct userInput{
	std::vector<float> arvot;
	bool run;
	moodiT moodi;
	//userInput() : arvot(8, 0.5), run(true), moodi(MUOKATAAN) {}
    float mouse_x;
    float mouse_y;
    userInput() : arvot(8, 0.5), run(true), moodi(MUOKATAAN), mouse_x(0), mouse_y(0) {}
};

std::mutex mtx;
userInput ui;

void print(userInput ui) {
	std::cout << "moodi: ";
	if (ui.moodi == KOULUTETAAN) std::cout << "Koulutetaan";
	else if (ui.moodi == MUOKATAAN) std::cout << "Muokataan";
    else if (ui.moodi == ASETA_TOIVE) std::cout << "aseta toive - ei pitais tulla";
	else 
		std::cout << "Katsellaan";

	std::cout << "\n";
	
	for (int i = 0; i < ui.arvot.size(); i++) {
		std::cout << ui.arvot[i];
		if (i < ui.arvot.size() - 1)
			std::cout << ", ";
		else
			std::cout << "\n";
	}
	
	

}


userInput handleEvent(SDL_Event e) {


	const float MUUTOS = 0.02;

	if (e.type == SDL_QUIT)
		ui.run = false;
   
    else if (e.type == SDL_MOUSEMOTION) {
        
        int x, y;
        const int w = 1000;
        const int h = 800;
        
        SDL_GetMouseState(&x, &y);
        ui.mouse_x = (float)x / w;
        ui.mouse_y = (float)y / h;
        
    }

	else if (e.type == SDL_KEYDOWN) {

		//std::cerr << SDL_GetKeyName(e.key.keysym.sym) << " pressed\n";

		switch (e.key.keysym.sym) {

			//Moodin vaihto
		case SDLK_RETURN:
			ui.moodi = ASETA_TOIVE;
			break;
		
		case SDLK_SPACE:
			if (ui.moodi == MUOKATAAN) ui.moodi = KOULUTETAAN;
			else if (ui.moodi == KOULUTETAAN) ui.moodi = MUOKATAAN;
			break;
		case SDLK_TAB:
			if (ui.moodi == KATSELLAAN) ui.moodi = MUOKATAAN;
			else ui.moodi = KATSELLAAN;
			break;

			//Reset
		case SDLK_ESCAPE:
			for (int i = 0; i < ui.arvot.size(); i++)
				ui.arvot[i] = 0.5;
			break;
            case SDLK_z:
                for (int i = 0; i < ui.arvot.size(); i++)
                    ui.arvot[i] = randf(0, 1);
                break;
			//Muodon säätö

		case SDLK_q:
			ui.arvot[0] += MUUTOS;
			break;
		case SDLK_a:
			ui.arvot[0] -= MUUTOS;
			break;
		case SDLK_w:
			ui.arvot[1] += MUUTOS;
			break;
		case SDLK_s:
			ui.arvot[1] -= MUUTOS;
			break;
		case SDLK_e:
			ui.arvot[2] += MUUTOS;
			break;
		case SDLK_d:
			ui.arvot[2] -= MUUTOS;
			break;
		case SDLK_r:
			ui.arvot[3] += MUUTOS;
			break;
		case SDLK_f:
			ui.arvot[3] -= MUUTOS;
			break;
		case SDLK_t:
			ui.arvot[4] += MUUTOS;
			break;
		case SDLK_g:
			ui.arvot[4] -= MUUTOS;
			break;
		case SDLK_y:
			ui.arvot[5] += MUUTOS;
			break;
		case SDLK_h:
			ui.arvot[5] -= MUUTOS;
			break;
		case SDLK_u:
			ui.arvot[6] += MUUTOS;
			break;
		case SDLK_j:
			ui.arvot[6] -= MUUTOS;
			break;
		case SDLK_i:
			ui.arvot[7] += MUUTOS;
			break;
		case SDLK_k:
			ui.arvot[7] -= MUUTOS;
			break;

		}

		for (int i = 0; i < ui.arvot.size(); i++)
			bound(ui.arvot[i], 0, 1);

	}
	return ui;
}




int main(int argc, char* argv[]) {

	olentoServer::aloita(); //käynnistyy omaan threadiin

	nnInterface::Init(); //initialize nnet

	std::thread net(nnInterface::StartRoutine);

	initialize();

	SDL_Event e;
	std::vector<float> paketti;
	std::vector<float> testInputs(4, 0.5);
	dClock t;

	std::vector<float> muodonArvot;


	do {
		//ota aikaa
		t.reset();

		//hae käyttäjän syöte
		while (SDL_PollEvent(&e))
			handleEvent(e);

		//hae paketti
		paketti = olentoServer::haePaketti(); //palauttaa tyhjän paketin jos paketteja ei ole tullut. Tarkista p.empty()
		//paketti = testInputs;
       /* paketti.resize(2);
        paketti[0] = ui.mouse_x;
        paketti[1] = ui.mouse_y;
        */
		nnInterface::mtx.lock();
		
		if (ui.moodi == KATSELLAAN) {
            std::vector<float> outputs;
			//annetaan inputit nnetille
			nnInterface::SetInput(paketti);

			//haetaan vaste nnetiltä
			while (outputs.empty()) {
				outputs = nnInterface::GetOutput();
				//std::cout << "outputs oli tyhjä\n";
			}
			//std::cout << "tuli output\n";

            std::cout << "outputs: " << outputs[0] << " " << outputs[1] << " " << outputs[2] << "\n";
			
            //skaalaa
            for (int i = 0; i < outputs.size(); i++) {
                muodonArvot[i] = (outputs[i] * 2 - 1) * 0.1f;
                bound(muodonArvot[i],0.0001f,0.9999f);
            }
            
            nnInterface::LaskeDesiredOut(muodonArvot);
            
		}
		
		if (ui.moodi == ASETA_TOIVE) {
            nnInterface::SetInput(paketti);
            nnInterface::TeeTilanne(paketti, ui.arvot);
            ui.moodi = MUOKATAAN;
		}

		if (ui.moodi == MUOKATAAN) {//ks handle event
		}

		nnInterface::mtx.unlock();       
		asetaMuoto(muodonArvot);

		olentoServer::asetaVastausviesti(muodonArvot);
		updateGL();

		print(ui);

		//ajasta luuppi
		std::cout << "Kesti " << t.get() << " s\n";
		t.delay(30);

	} while (ui.run);

	nnInterface::Close();

	net.join();
	olentoServer::lopeta();

	return 0;

}
  
