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

enum moodiT {
	KOULUTETAAN, MUOKATAAN, KATSELLAAN
};


struct userInput{
	std::vector<float> arvot;
	bool run;
	moodiT moodi;
	userInput() : arvot(8, 0), run(true), moodi(MUOKATAAN) {}
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

	const float MUUTOS = 0.1;
	static std::vector<int> keyboard(8, 0);

	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT)
			ui.run = false;

		else if (e.type == SDL_KEYUP || e.type == SDL_KEYDOWN) {
			std::cerr << "got key event\n";
			switch (e.key.keysym.sym) {
			
			//moodin vaihtaminen
			case SDLK_TAB:
				if (e.type == SDL_KEYDOWN) {
					if (ui.moodi == KATSELLAAN) ui.moodi = MUOKATAAN;
					else ui.moodi = KATSELLAAN;
				}
				break;
			case SDLK_RETURN:
				if (e.type == SDL_KEYDOWN) {
					if (ui.moodi == MUOKATAAN) ui.moodi = KOULUTETAAN;
					else if (ui.moodi == KOULUTETAAN) ui.moodi = MUOKATAAN;
				}
				break;

			//muodon säätö
			case SDLK_q:
				if (e.type == SDL_KEYDOWN) keyboard[0] = 1;
				else keyboard[0] = 0;
				break;
			case SDLK_a:
				if (e.type == SDL_KEYDOWN)keyboard[0] = -1;
				else keyboard[0] = 0;
				break;
			case SDLK_w:
				if (e.type == SDL_KEYDOWN) keyboard[1] = 1;
				else keyboard[1] = 0;
				break;
			case SDLK_s:
				if (e.type == SDL_KEYDOWN)keyboard[1] = -1;
				else keyboard[1] = 0;
				break;
			case SDLK_e:
				if (e.type == SDL_KEYDOWN) keyboard[2] = 1;
				else keyboard[2] = 0;
				break;
			case SDLK_d:
				if (e.type == SDL_KEYDOWN)keyboard[2] = -1;
				else keyboard[2] = 0;
				break;
			case SDLK_r:
				if (e.type == SDL_KEYDOWN) keyboard[3] = 1;
				else keyboard[3] = 0;
				break;
			case SDLK_f:
				if (e.type == SDL_KEYDOWN)keyboard[3] = -1;
				else keyboard[3] = 0;
				break;
			case SDLK_t:
				if (e.type == SDL_KEYDOWN) keyboard[4] = 1;
				else keyboard[4] = 0;
				break;
			case SDLK_g:
				if (e.type == SDL_KEYDOWN)keyboard[4] = -1;
				else keyboard[4] = 0;
				break;
			case SDLK_y:
				if (e.type == SDL_KEYDOWN) keyboard[5] = 1;
				else keyboard[5] = 0;
				break;
			case SDLK_h:
				if (e.type == SDL_KEYDOWN)keyboard[5] = -1;
				else keyboard[5] = 0;
				break;
			case SDLK_u:
				if (e.type == SDL_KEYDOWN) keyboard[6] = 1;
				else keyboard[6] = 0;
				break;
			case SDLK_j:
				if (e.type == SDL_KEYDOWN)keyboard[6] = -1;
				else keyboard[6] = 0;
				break;
			case SDLK_i:
				if (e.type == SDL_KEYDOWN) keyboard[7] = 1;
				else keyboard[7] = 0;
				break;
			case SDLK_k:
				if (e.type == SDL_KEYDOWN)keyboard[7] = -1;
				else keyboard[7] = 0;
				break;
			}
		}
	}

	//tee 'slider-arvot'
	for (int i = 0; i < ui.arvot.size(); i++) {
		if (keyboard[i] > 0) ui.arvot[i] += MUUTOS;
		else if (keyboard[i] < 0) ui.arvot[i] -= MUUTOS;
		bound(ui.arvot[i], 0, 1);
	}

	return ui;
}




int main(int argc, char* argv[]) {

	olentoServer::aloita(); //käynnistyy omaan threadiin
	std::thread(StartRoutine).detach();

	initialize();

	SDL_Event e;
	std::vector<float> paketti;
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
		paketti = olentoServer::haePaketti(); //palauttaa tyhjän paketin jos paketteja ei ole tullut. Tarkista p.empty()
		if (paketti.empty())
			std::cerr << "Ei pakettia!\n";

		if (ui.moodi == MUOKATAAN) {
			muodonArvot = ui.arvot;
		}

		else if (ui.moodi == KOULUTETAAN) {
			//hae paketti
			muodonArvot = ui.arvot;

			if (!paketti.empty()) {
				SetInput(paketti);
				SetDesiredOut(ui.arvot);
			}
		}

		else if (ui.moodi == KATSELLAAN) {
			//hae paketti
			paketti = olentoServer::haePaketti(); //palauttaa tyhjän paketin jos paketteja ei ole tullut. Tarkista p.empty()

			if (!paketti.empty()) {
				SetInput(paketti);
				muodonArvot = GetOutput();
			}
		}

		asetaMuoto(muodonArvot);
		olentoServer::asetaVastausviesti(muodonArvot);
		updateGL();

		print(ui);

		//ajasta luuppi
		std::cout << "Kesti " << t.get() << " s\n";
		t.delay(30);

	} while (ui.run);


	olentoServer::lopeta();

	return 0;

}
  
