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

#include "tilanteet.h"
#include "vektorilaskut.h"

tilanteet tilanteet_;

enum moodiT {
    KOULUTETAAN, ASETA_TOIVE, MUOKATAAN, KATSELLAAN, PLAYBACK
};

struct userInput {
    std::vector<float> arvot;
    bool run;
    moodiT moodi;
    //userInput() : arvot(8, 0.5), run(true), moodi(MUOKATAAN) {}
    float mouse_x;
    float mouse_y;

    userInput() : arvot(8, 0.5), run(true), moodi(KATSELLAAN), mouse_x(0), mouse_y(0) {
    }
};

std::mutex mtx;
userInput ui;
std::vector<float> muodonArvot(8, 0.5);
std::vector<float> viimeisinPaketti(102, 0);
int viimeisinTilanne = -1;

std::vector< std::vector<float> > historia;

int aktiivinenLava = 1; //joko 1 tai 2
float naamoja = 0; //naamojen m��r� viimeksi aktiivisella lavalla

float min_naamoja = 0.0; //naamojen v�himm�ism��r�
float herkkyys = 0.22;
int neighbours_n = 8;

std::vector<float> haeHistoriasta() {
    static int i;

    if (historia.empty()) {
        std::cout << "Ei historiaa!\n";
        return std::vector<float>(0);
    }

    i++;

    if (i >= historia.size() || i < 0)
        i = 0;


    //std::cout << dClock::getTimeString() << " Playback [" << i << "]: " << olentoServer::floatsToString(historia[i]) << "\n";

    return historia[i];

}

userInput handleEvent(SDL_Event e) {


    const float MUUTOS = 0.01;

    if (e.type == SDL_QUIT)
        ui.run = false;

    else if (e.type == SDL_MOUSEMOTION) {

        int x, y;
        const int w = 1000;
        const int h = 800;

        SDL_GetMouseState(&x, &y);
        ui.mouse_x = (float) x / w;
        ui.mouse_y = (float) y / h;

    } else if (e.type == SDL_KEYDOWN) {

        //std::cout << SDL_GetKeyName(e.key.keysym.sym) << " pressed\n";

        switch (e.key.keysym.sym) {
                using namespace std;
            case SDLK_F12:
                ui.run = false;
                break;
                /*    
                case SDLK_F11:
                    nnInterface::SaveNN("testitalle");
                    break;

                case SDLK_F10:
                    nnInterface::LoadNN("testitalle");
                    break;
                 */
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
                    std::cout << "muokataan\n";
                } else {
                    ui.moodi = KATSELLAAN;
                    ui.arvot = muodonArvot;
                    std::cout << "katsellaan\n";
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
                //Muodon s��t�

            case SDLK_KP_7:
                min_naamoja += 0.5;
                cerr << "min_naamoja: " << min_naamoja << "\n";
                break;

            case SDLK_KP_4:
                min_naamoja -= 0.5;
                if (min_naamoja < 0) min_naamoja = 0;
                cerr << "min_naamoja: " << min_naamoja << "\n";
                break;

            case SDLK_KP_8:
                herkkyys += 0.01;
                cerr << "herkkyys: " << herkkyys << "\n";
                break;

            case SDLK_KP_5:
                herkkyys -= 0.01;
                if (herkkyys < 0.01) herkkyys = 0.01;
                cerr << "herkkyys: " << herkkyys << "\n";
                break;

            case SDLK_KP_9:
                neighbours_n += 1;
                cerr << "neighbours_n: " << neighbours_n << "\n";
                break;

            case SDLK_KP_6:
                neighbours_n -= 1;
                if (neighbours_n < 1) neighbours_n = 1;
                cerr << "neighbours_n: " << neighbours_n << "\n";
                break;


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

    tilanteet_.lataa("resources/tilanteet.net");
    //tilanteet_.laskeGeenit();

    olentoServer::aloita(); //k�ynnistyy omaan threadiin

    olentogl::initialize(1280, 800, false);

    SDL_Event e;
    std::vector<float> paketti;
    std::vector<float> outputs;
    outputs.resize(8, 0.5);

    dClock t;
    dClock paketti_t;
    int viimeksiValittuTilanneId = -1;

    //    dClock runTime;
    //    dClock kappaleenVaihdonOdotus;
    dClock playback_t;

    std::string PLAYBACK_FILE = "resources/playback.his";

    historia = olentoServer::lataaHistoria(PLAYBACK_FILE);

    do {
        //ota aikaa
        t.reset();

        int hours = dClock::getTime_hours();

        olentogl::vaihdaKuva();

        //hae k�ytt�j�n sy�te
        while (SDL_PollEvent(&e))
            handleEvent(e);

        //hae paketti

        paketti = olentoServer::haePaketti(); //palauttaa tyhj�n paketin jos paketteja ei ole tullut. Tarkista p.empty()
        if (!paketti.empty()) {
            if (paketti.size() < 102) //paketin koko pit�isi olla 104. Jos tuli v�hemm�n, hyl�t��n paketti.
                paketti.clear();
            else {
                std::cout << dClock::getTimeString() << " Tuli paketti! Koko: " << paketti.size() << " Lava: " << paketti[0] << " Naamoja: " << paketti[1] << "\n";
                paketti_t.reset();
                paketti.resize(102);
            }
        }


        //nnInterface::mtx.lock();



        if (ui.moodi == KATSELLAAN) {

            bool samaNaama;

            if (paketti.size() == 102) {

                if (vektori::pituus(vektori::erotus(viimeisinPaketti, paketti)) < 0.2) {
                    samaNaama = true;
                    viimeisinPaketti = vektori::miksaus(paketti, viimeisinPaketti, 0.85f);
                    std::cout << "sama naama\n";
                } else {
                    viimeisinPaketti = vektori::miksaus(paketti, viimeisinPaketti, 0.7f);
                    samaNaama = false;
                }

            }

            if (viimeisinPaketti.size() == 102) {
                dClock clk;


                //maaritellaan paamaara
                std::vector<int> lahimmatInputit = tilanteet_.teeLahimpienTilanteidenIndeksiTaulukko(viimeisinPaketti, 60);


                    bool viimeInputLoytyy;

                    for (int i = 0; i < lahimmatInputit.size(); i++) {
                        if (lahimmatInputit[i] == viimeisinTilanne) {
                            viimeInputLoytyy = true;
                            break;
                        }
                    }


                    if (!viimeInputLoytyy) {
                        
                        float rajaarvo = vektori::pituus(vektori::erotus(tilanteet_.annaTilanne(lahimmatInputit[0]).getOutput(),muodonArvot));
                        std::cout << "rajaarvo: " << 1+rajaarvo*100 << "\n";
                        
                        int id = tilanteet_.lahimmatTilanteetOutputinMukaan(1 +(int)rajaarvo * 100, tilanteet_.annaTilanne(lahimmatInputit[0]).getOutput())[0];
                        muodonArvot = vektori::miksaus(muodonArvot,tilanteet_.annaTilanne(id).getOutput(),0.15);
                    }
                        
                        //muutoin sama kuin ennen
                




                //aseta viel� samat arvot ui:iin ett� on helpompi vaihtaa
                ui.arvot = muodonArvot;

                //tallenna paketti, muodon arvot ja NNetin ulostulot tiedostoon
                olentoServer::tallennaTiedot(viimeisinPaketti, muodonArvot, outputs);

                //nnInterface::LaskeDesiredOut(muodonArvot);
                //std::cout << "laskeDesired: " << clk.get() << "\n";
            }//playback jos paketteja ei kuulu v�h��n aikaan

            else if (paketti_t.get() > 10) {
                //std::cout << dClock::getTimeString() << " Ei paketteja " << paketti_t.get() << " sekuntiin. Playback p��lle.\n";
                //ui.moodi = PLAYBACK;
            }

        }

        if (ui.moodi == ASETA_TOIVE) {
            if (!paketti.empty()) {
                //nnInterface::SetInput(paketti);
                //nnInterface::TeeTilanne(paketti, ui.arvot);
                tilanteet_.lisaa(paketti, ui.arvot);
                tilanteet_.tallenna("resources/testi.net");
                ui.moodi = MUOKATAAN;
            }
        }

        if (ui.moodi == MUOKATAAN) {
            muodonArvot = ui.arvot;
            if (playback_t.get() > 1.0) {
                std::vector<float> tyhjaVektori;
                olentoServer::tallennaTiedot(tyhjaVektori, muodonArvot, tyhjaVektori, PLAYBACK_FILE);
                playback_t.reset();
            }
        }
        /*
                if (ui.moodi == PLAYBACK) {
                    //hae uusi tieto sekunnin v�lein
                    if (playback_t.get() >= 1.0) {
                        muodonArvot = haeHistoriasta();
                        //std::cout << "ok\n";
                        if (muodonArvot.size() != 8) {
                            std::cout << dClock::getTimeString() << "HaeHistoriasta: Tuli " << muodonArvot.size() << " arvoa!\n";
                            muodonArvot.resize(8);
                        }
                        playback_t.reset();
                    }

                }
         */
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

    //net.join();
    olentoServer::lopeta();

    return 0;

}

