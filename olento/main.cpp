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
	KOULUTETAAN, MUOKATAAN, KATSELLAAN
};

std::mutex mtx;

struct userInput{
	std::vector<float> arvot;
	bool run;
	moodiT moodi;
	userInput() : arvot(8, 0.5), run(true), moodi(MUOKATAAN) {}
};


void print(userInput ui) {
	std::cout << "moodi: ";
	if (ui.moodi == KOULUTETAAN) std::cout << "Koulutetaan";
	else if (ui.moodi == MUOKATAAN) std::cout << "Muokataan";
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

	static userInput ui;

	const float MUUTOS = 0.02;

	if (e.type == SDL_QUIT)
		ui.run = false;

	if (e.type == SDL_KEYDOWN) {

		//std::cerr << SDL_GetKeyName(e.key.keysym.sym) << " pressed\n";

		switch (e.key.keysym.sym) {

			//Moodin vaihto
		case SDLK_RETURN:
			if (ui.moodi == MUOKATAAN) ui.moodi = KOULUTETAAN;
			else if (ui.moodi == KOULUTETAAN) ui.moodi = MUOKATAAN;
			break;
		case SDLK_TAB:
			if (ui.moodi == KATSELLAAN) ui.moodi = MUOKATAAN;
			else ui.moodi = KATSELLAAN;
			break;

			//Reset
		case SDLK_SPACE:
			for (int i = 0; i < ui.arvot.size(); i++)
				ui.arvot[i] = 0.5;
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

	userInput ui;
	std::vector<float> muodonArvot;


	do {
		//ota aikaa
		t.reset();

		//hae käyttäjän syöte
		while (SDL_PollEvent(&e))
			ui = handleEvent(e);

		//hae paketti
		//paketti = olentoServer::haePaketti(); //palauttaa tyhjän paketin jos paketteja ei ole tullut. Tarkista p.empty()
		paketti = testInputs;

        
        nnInterface::mtx.lock();
		if (ui.moodi == MUOKATAAN) {
			muodonArvot = ui.arvot;
		}

		else if (ui.moodi == KOULUTETAAN) {
			muodonArvot = ui.arvot;

			if (!paketti.empty()) {
                std::cout << "setinput\n";
				nnInterface::SetInput(paketti);
				nnInterface::SetDesiredOut(ui.arvot);
			}
		}

		else if (ui.moodi == KATSELLAAN) {
            mtx.lock();
			if (!paketti.empty()) {
                std::cout << "setinput\n";
				nnInterface::SetInput(paketti);
				std::vector<float> outputs;

				while (outputs.empty())
					outputs = nnInterface::GetOutput();
                std::cout << outputs[0];
				muodonArvot = outputs;
                std::cout << " " << outputs[0] << "\n";
			}
            mtx.unlock();
		}

		nnInterface::mtx.unlock();       
		asetaMuoto(muodonArvot);

		olentoServer::asetaVastausviesti(muodonArvot);
		updateGL();

		//print(ui);

		//ajasta luuppi
		std::cout << "Kesti " << t.get() << " s\n";
		t.delay(30);

	} while (ui.run);

	nnInterface::Close();

	net.join();
	olentoServer::lopeta();

	return 0;

}
  
