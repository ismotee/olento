#ifndef SERVER_H
#define SERVER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include <vector>
#include <atomic>

namespace olentoServer{

	const int MAX_PAKETTEJA = 1000;

	const unsigned short PORT = 1234;            // The port our server will listen for incoming connecions on
	const unsigned short BUFFER_SIZE = 1400;             // Size of our message buffer
	const unsigned short MAX_SOCKETS = 100;               // Max number of sockets
	const unsigned short MAX_CLIENTS = MAX_SOCKETS - 1; // Max number of clients in our socket set (-1 because server's listening socket takes the 1st socket in the set)

	struct vec2{
		float x;
		float y;
	};

	struct pakettiDataT {
		std::vector<vec2> pisteet;
		bool empty();
	};

	pakettiDataT haePaketti();
	void tulostaPaketti(pakettiDataT paketti);
	std::vector<float> annaPisteet(pakettiDataT paketti);
	void aloita();
	void kuuntele();
	bool setup();
	void lopeta();
	void testi();
	void asetaVastausviesti(std::vector<float> arvot);
}

#endif