#ifndef SERVER_H
#define SERVER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include <vector>
#include <atomic>
#include <string>

namespace olentoServer{

    union CharFloat {
        float f;
        char c[0];
    
    };
    
    
	const int MAX_PAKETTEJA = 100;

	const unsigned short PORT = 1234;            		// The port our server will listen for incoming connecions on
	const unsigned short BUFFER_SIZE = 1400;     		// Size of our message buffer
	const unsigned short MAX_SOCKETS = 100;             // Max number of sockets
	const unsigned short MAX_CLIENTS = MAX_SOCKETS - 1; // Max number of clients in our socket set (-1 because server's listening socket takes the 1st socket in the set)
	const std::string DEFAULT_HIS_FILE = "/media/olento/Uusi asema/ohjelmointi/c++/olento/olento/resources/history.his";

	std::vector<float> haePaketti();
	void tulostaPaketti(std::vector<float> paketti);
	std::vector<float> annaPisteet(std::vector<float> paketti);
	void aloita();
	void kuuntele();
	bool setup();
	void lopeta();
	void testi();
	void asetaVastausviesti(std::vector<float> arvot);

	std::string floatsToString(std::vector<float> v);

	void tallennaTiedot(std::vector<float> paketti, std::vector<float> muodonArvot, std::vector<float> muodonMuutos, std::string filename = DEFAULT_HIS_FILE);
	std::vector<std::vector<float> > lataaHistoria(std::string filename = DEFAULT_HIS_FILE);

}

#endif