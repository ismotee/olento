//main.cpp
#include "olentogl.h"
#include "random.h"
#include "dClock.h"
#include "server.h"
#include "libnnInterface.h"

#include <thread>
#include <vector>
#include <iostream>
#include <SDL2/SDL.h>
#include <mutex>
#include <time.h>


enum moodiT {
    KOULUTETAAN, ASETA_TOIVE, MUOKATAAN, KATSELLAAN, PLAYBACK
};


struct userInput{
    std::vector<float> arvot;
    bool run;
    moodiT moodi;
    //userInput() : arvot(8, 0.5), run(true), moodi(MUOKATAAN) {}
    float mouse_x;
    float mouse_y;
    userInput() : arvot(8, 0.5), run(true), moodi(KATSELLAAN), mouse_x(0), mouse_y(0) {}
};

std::mutex mtx;
userInput ui;
std::vector<float> muodonArvot(8, 0.5);

std::vector< std::vector<float> > historia;

int aktiivinenLava = 1; //joko 1 tai 2
float naamoja = 0; //naamojen m‰‰r‰ viimeksi aktiivisella lavalla
float min_naamoja = 0.7; //naamojen v‰himm‰ism‰‰r‰


std::vector<float> haeHistoriasta() {
	static int i;

	if(historia.empty() ) {
		std::cerr << "Ei historiaa!\n";
		return std::vector<float>(0);
	}

	i++;

	if(i >= historia.size() || i < 0) 
		i=0;


	//std::cout << dClock::getTimeString() << " Playback [" << i << "]: " << olentoServer::floatsToString(historia[i]) << "\n";

	return historia[i];

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

        	case SDLK_F12:
        		ui.run = false;
        		break;

                //Moodin vaihto
            case SDLK_RETURN:
                ui.moodi = ASETA_TOIVE;
                break;
                
            case SDLK_SPACE:
                if (ui.moodi == MUOKATAAN) ui.moodi = KOULUTETAAN;
                else if (ui.moodi == KOULUTETAAN) ui.moodi = MUOKATAAN;
                break;
            case SDLK_TAB:
                if (ui.moodi == KATSELLAAN) {
                	ui.moodi = MUOKATAAN;
                	std::cerr << "muokataan\n";
                }
                else {
                	ui.moodi = KATSELLAAN;
            	    ui.arvot = muodonArvot;
            	    std::cerr << "katsellaan\n";
                }
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
                //Muodon s‰‰tˆ
                
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
    
    olentoServer::aloita(); //k‰ynnistyy omaan threadiin
    
    nnInterface::Init(); //initialize nnet
    
    std::thread net(nnInterface::StartRoutine);
    
    olentogl::initialize(1600, 900, false);
    
    SDL_Event e;
    std::vector<float> paketti;
    std::vector<float> testInputs(4, 0.5);
    dClock t;
    dClock paketti_t;

//    dClock runTime;
//    dClock kappaleenVaihdonOdotus;
    dClock playback_t;

    std::string PLAYBACK_FILE = "/media/olento/Uusi asema/ohjelmointi/c++/olento/olento/resources/playback.his";

   	historia = olentoServer::lataaHistoria(PLAYBACK_FILE);

    do {
        //ota aikaa
        t.reset();

        int hours = dClock::getTime_hours();

       	olentogl::vaihdaKuva();

        //hae k‰ytt‰j‰n syˆte
        while (SDL_PollEvent(&e))
            handleEvent(e);
        
        //hae paketti

        paketti = olentoServer::haePaketti(); //palauttaa tyhj‰n paketin jos paketteja ei ole tullut. Tarkista p.empty()
        if(!paketti.empty()) {
        	if(paketti.size() < 104) //paketin koko pit‰isi olla 104. Jos tuli v‰hemm‰n, hyl‰t‰‰n paketti.
        		paketti.clear();
        	else {
	        	//std::cout << dClock::getTimeString() << " Tuli paketti! (" << paketti.size() << ") V‰li: " << paketti_t.get() << "s\n";
	        	//std::cout << "lavalta " << paketti[0] << ", naamoja on " << paketti[1] << "\n";
	        	std::cout << dClock::getTimeString() << " Tuli paketti! Koko: " << paketti.size() << " Lava: " << paketti[0] << " Naamoja: " << paketti[1] << "\n";
				paketti_t.reset();
	            paketti.resize(104);
	        	//paketti[0] : lavan id (1/2)
	        	//paketti[1] : naamojen m‰‰r‰ (float)

	        	if(paketti[0] != aktiivinenLava) { //Paketti tuli toiselta lavalta
	        		if(paketti[1] > naamoja) {
	        			//Toisella lavalla on enemm‰n naamoja kuin viimeksi aktiivisella lavalla! Vaihdetaan aktiivista lavaa.
	        			aktiivinenLava = paketti[0];
	        			naamoja = paketti[1];
	        			std::cout << dClock::getTimeString() << " Vaihdetaan lavalle " << aktiivinenLava << "\n";
	        		}
	        		else { //Toisella lavalla on v‰hemm‰n naamoja. Hyl‰t‰‰n paketti.
	        			paketti.clear();
	        		}
	        	}

	        	else {//Paketti tuli aktiiviselta lavalta
	        		naamoja = paketti[1];	
	        	}

	        	//poistetaan n‰m‰
	        	if(paketti.size() >= 2) paketti.erase(paketti.begin(), paketti.begin()+2);

	        	//Jos naamoja ei ole miss‰‰n kovin paljoa, vaihdetaan playback p‰‰lle
	        	if(naamoja < min_naamoja) {
	        		if(ui.moodi != PLAYBACK) {
		        		ui.moodi = PLAYBACK;
//		        		historia = olentoServer::lataaHistoria(PLAYBACK_FILE);
		        		std::cout << dClock::getTimeString() << " Ei n‰y naamoja. Playback p‰‰lle.\n";
		        	}
	        	}

	        	//jos naamoja on, vaihdetaan takaisin playbackilta
	        	else if (ui.moodi == PLAYBACK) {
	        		ui.moodi = KATSELLAAN;
	        		std::cout << dClock::getTimeString() << " Naamoja n‰kyy! Playback pois\n";
	        	}

	        	//std::cout << "Lavalla " << aktiivinenLava << " tapahtuu! Naamoja on " << naamoja << "\n";
        	}
        }


        //nnInterface::mtx.lock();
        
        if (ui.moodi == KATSELLAAN) {
            
            if(!paketti.empty()) {
                dClock clk;
                std::vector<float> outputs;
                //annetaan inputit nnetille
                nnInterface::SetInput(paketti);
                
                //haetaan vaste nnetilt‰
                while (outputs.empty()) {
                    outputs = nnInterface::GetOutput();
                    //std::cout << "outputs oli tyhj‰\n";
                }
                //std::cout << "tuli output\n";
                
                //std::cout << "outputs: " << olentoServer::floatsToString(outputs) << "\n";
                //std::cout << "muodonArvot: " << olentoServer::floatsToString(muodonArvot) << "\n";
                
                //muuta muotoa
                for (int i = 0; i < outputs.size(); i++) {
                    muodonArvot[i] += (outputs[i] * 2 - 1) * 0.22f;
                    bound(muodonArvot[i],0.0001f,0.9999f);
                }

                //aseta viel‰ samat arvot ui:iin ett‰ on helpompi vaihtaa
                ui.arvot = muodonArvot;

                //tallenna paketti, muodon arvot ja NNetin ulostulot tiedostoon
                olentoServer::tallennaTiedot(paketti, muodonArvot, outputs);

                nnInterface::LaskeDesiredOut(muodonArvot);
                //std::cout << "laskeDesired: " << clk.get() << "\n";
            }
            //playback jos paketteja ei kuulu v‰h‰‰n aikaan
	       	else if(paketti_t.get() > 10) {
    	   		std::cout << dClock::getTimeString() << " Ei paketteja " << paketti_t.get() << " sekuntiin. Playback p‰‰lle.\n";
       			ui.moodi = PLAYBACK;
       		}

        }
        
        if (ui.moodi == ASETA_TOIVE) {
            if(!paketti.empty())
            {
                nnInterface::SetInput(paketti);
                nnInterface::TeeTilanne(paketti, ui.arvot);
                ui.moodi = MUOKATAAN;
            }
        }
        
        if (ui.moodi == MUOKATAAN) {
            muodonArvot = ui.arvot;
            if(playback_t.get() > 1.0) {
	            std::vector<float> tyhjaVektori;
	            //olentoServer::tallennaTiedot(tyhjaVektori, muodonArvot, tyhjaVektori, PLAYBACK_FILE);
	            playback_t.reset();
	        }
	    }

        if (ui.moodi == PLAYBACK) {
        	//hae uusi tieto sekunnin v‰lein
        	if(playback_t.get() >= 1.0) {        		
        		muodonArvot = haeHistoriasta();
        		//std::cerr << "ok\n";
        		if(muodonArvot.size() != 8) {
        			std::cerr << dClock::getTimeString() << "HaeHistoriasta: Tuli " << muodonArvot.size() << " arvoa!\n";
        			muodonArvot.resize(8);
        		}
        		playback_t.reset();
        	}

        }
        
        //nnInterface::mtx.unlock();

        //std::thread(asetaMuoto,muodonArvot).detach();
        olentogl::asetaMuoto(muodonArvot);
        olentogl::show();

        olentoServer::asetaVastausviesti(muodonArvot);
        //updateGL();
        
        //print(ui);
        
        //ajasta luuppi
       // std::cout << "Kesti " << t.get() << " s\n";
        t.delay(30);
        
    } while (ui.run);
    
    nnInterface::Close();
    
    net.join();
    olentoServer::lopeta();
    
    return 0;
    
}

