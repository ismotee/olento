#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <thread>
#include "server.h"

using namespace std;

namespace olentoServer{

	// Viestit jotka lähetetään clientille, joka yrittää yhdistää: onko serveri täynnä vai hyväksytäänkö
	const string SERVER_NOT_FULL = "OK";
	const string SERVER_FULL = "FULL";

	// Tarvittavat globaalit muuttujat
	IPaddress serverIP;                  // Serverin IP -- käytännössä 0.0.0.0
	TCPsocket serverSocket;
	TCPsocket clientSocket[MAX_CLIENTS]; // Clienttien socketit arrayssa
	bool      socketIsFree[MAX_CLIENTS]; // Bool flagit, joista näkee onko arrayn paikka vapaa
	char buffer[BUFFER_SIZE];            // Tuleva data laitetaan tähän  
	std::string response;					//lähetettävä viesti  
	int receivedByteCount = 0;           // Montako tavua on luettu viimeisestä paketista
	int clientCount = 0;                 // Montako clienttiä on yhdistettynä
	SDLNet_SocketSet socketSet;

	std::thread* t; // pakettien vastaanottajathread

	std::atomic<bool> running(false);
	std::atomic<bool> paketitVarattu(false);

	std::vector<pakettiDataT> paketit;


	void tulostaPaketti(pakettiDataT paketti) {
		for (int i = 0; i < paketti.pisteet.size(); i++) {
			cout << "x: " << paketti.pisteet[i].x << " y: " << paketti.pisteet[i].y << "\n";
		}
	}

	std::vector<float> annaPisteet(pakettiDataT paketti) {
		std::vector<float> result;
		for (int i = 0; i < paketti.pisteet.size(); i++) {
			result.push_back(paketti.pisteet[i].x);
			result.push_back(paketti.pisteet[i].y);
		}
		return result;
	}


	bool initializeServer() {

		static bool isReady = false;

		if (!isReady) {
			if (SDLNet_Init() == -1) {
				std::cerr << "Failed to intialise SDL_net: " << SDLNet_GetError() << std::endl;
				return false;
			}
			else {
				isReady = true;
				return true;
			}
		}
		return true;
	}


	bool pakettiDataT::empty() {
		return pisteet.empty();
	}


	pakettiDataT haePaketti() {
		//Palauta alkupäästä eli ensimmäisenä lisätty paketti
		pakettiDataT result;
		while (paketitVarattu)
			SDL_Delay(1);

		paketitVarattu = true;

		if (!paketit.empty()) {
			result = paketit[0];
			paketit.erase(paketit.begin());
		}

		paketitVarattu = false;

		return result;
	}


	pakettiDataT luoPaketti(char* data, int kokoTavuina) {
		stringstream ss;
		pakettiDataT uusiPaketti;
		ss.write(data, kokoTavuina);

		vector<float> floats;

		while (ss.good()) {
			float f;
			ss >> f;
			floats.push_back(f);
		}

		//cerr << floats.size() << "\n";

		while (floats.size() > 1) {
			vec2 uusiPiste;
			uusiPiste.x = floats[0];
			uusiPiste.y = floats[1];
			floats.erase(floats.begin(), floats.begin() + 2);
			uusiPaketti.pisteet.push_back(uusiPiste);
		}


		int n_floats = kokoTavuina / sizeof(float);

		//cerr << kokoTavuina << "tavua, " << n_floats << "lukua\n";

		for (int i = 0; i < n_floats - 1; i += 2) {
			vec2 uusiPiste;
			uusiPiste.x = ((float*)data)[i];
			uusiPiste.y = ((float*)data)[i + 1];

			uusiPaketti.pisteet.push_back(uusiPiste);
		}

		return uusiPaketti;
	}


	void aloita() {

		//Tämä on päättymätön silmukka, joka katkeaa kun running asetetaan false:ksi
		//Tarkoitettu ajettavaksi omassa threadissa

		if (!setup())
			cerr << "Ei voitu asettaa serveriä!\n"; //Abort!

		std::thread(kuuntele).detach();
	}

	void kuuntele() {
		while (running) {

			// Check for activity on the entire socket set. The second parameter is the number of milliseconds to wait for.
			// For the wait-time, 0 means do not wait (high CPU!), -1 means wait for up to 49 days (no, really), and any other number is a number of milliseconds, i.e. 5000 means wait for 5 seconds
			int numActiveSockets = SDLNet_CheckSockets(socketSet, 0);

			if (numActiveSockets != 0) {
				;
				//std::cout << "There are currently " << numActiveSockets << " socket(s) with data to be processed." << std::endl;
			}

			// Check if our server socket has received any data
			// Note: SocketReady can only be called on a socket which is part of a set and that has CheckSockets called on it (the set, that is)
			// SDLNet_SocketRead returns non-zero for activity, and zero is returned for no activity. Which is a bit bass-ackwards IMHO, but there you go.
			int serverSocketActivity = SDLNet_SocketReady(serverSocket);

			// If there is activity on our server socket (i.e. a client has trasmitted data to us) then...
			if (serverSocketActivity != 0) {
				// If we have room for more clients...
				if (clientCount < MAX_CLIENTS) {

					// Find the first free socket in our array of client sockets
					int freeSpot = -99;
					for (int loop = 0; loop < MAX_CLIENTS; loop++) {
						if (socketIsFree[loop] == true) {
							//cout << "Found a free spot at element: " << loop << endl;
							socketIsFree[loop] = false; // Set the socket to be taken
							freeSpot = loop;            // Keep the location to add our connection at that index in the array of client sockets
							break;                      // Break out of the loop straight away
						}
					}

					// ...accept the client connection and then...
					clientSocket[freeSpot] = SDLNet_TCP_Accept(serverSocket);

					// ...add the new client socket to the socket set (i.e. the list of sockets we check for activity)
					SDLNet_TCP_AddSocket(socketSet, clientSocket[freeSpot]);

					// Increase our client count
					clientCount++;

					// Send a message to the client saying "OK" to indicate the incoming connection has been accepted
					strcpy(buffer, SERVER_NOT_FULL.c_str());
					int msgLength = strlen(buffer) + 1;
					SDLNet_TCP_Send(clientSocket[freeSpot], (void *)buffer, msgLength);

					cout << "Client connected. There are now " << clientCount << " client(s) connected." << endl << endl;
				}
				else { // If we don't have room for new clients...

					cout << "*** Maximum client count reached - rejecting client connection ***" << endl;

					// Accept the client connection to clear it from the incoming connections list
					TCPsocket tempSock = SDLNet_TCP_Accept(serverSocket);

					// Send a message to the client saying "FULL" to tell the client to go away
					strcpy(buffer, SERVER_FULL.c_str());
					int msgLength = strlen(buffer) + 1;
					SDLNet_TCP_Send(tempSock, (void *)buffer, msgLength);

					// Shutdown, disconnect, and close the socket to the client
					SDLNet_TCP_Close(tempSock);
				}

			} // End of if server socket is has activity check



			// Loop to check all possible client sockets for activity
			for (int clientNumber = 0; clientNumber < MAX_CLIENTS; clientNumber++) {
				// If the socket is ready (i.e. it has data we can read)... (SDLNet_SocketReady returns non-zero if there is activity on the socket, and zero if there is no activity)
				int clientSocketActivity = SDLNet_SocketReady(clientSocket[clientNumber]);

				//cout << "Just checked client number " << clientNumber << " and received activity status is: " << clientSocketActivity << endl;

				// If there is any activity on the client socket...
				if (clientSocketActivity != 0) {
					// Check if the client socket has transmitted any data by reading from the socket and placing it in the buffer character array
					receivedByteCount = SDLNet_TCP_Recv(clientSocket[clientNumber], buffer, BUFFER_SIZE);

					// If there's activity, but we didn't read anything from the client socket, then the client has disconnected...
					if (receivedByteCount <= 0) {
						//...so output a suitable message and then...
						cout << "Client " << clientNumber << " disconnected." << endl << endl;

						//... remove the socket from the socket set, then close and reset the socket ready for re-use and finally...
						SDLNet_TCP_DelSocket(socketSet, clientSocket[clientNumber]);
						SDLNet_TCP_Close(clientSocket[clientNumber]);
						clientSocket[clientNumber] = NULL;

						// ...free up their slot so it can be reused...
						socketIsFree[clientNumber] = true;

						// ...and decrement the count of connected clients.
						clientCount--;

						cout << "Server is now connected to: " << clientCount << " client(s)." << endl << endl;
					}
					else { // If we read some data from the client socket...

						// Output the message the server received to the screen
						//std::cout << "Received: >>>> " << buffer << " from client number: " << clientNumber << endl; //huom ei katkaise viestiä oikein
						//std::cout << "length: " << strlen(buffer) << "\n";
						//std::cout << "Byte count: " << receivedByteCount << "\n";

						//Luo paketti saadun datan perusteella
						pakettiDataT uusiPaketti = luoPaketti(buffer, receivedByteCount);

						if (strcmp(buffer, "shutdown") == 0)
						{
							running = false;

							cout << "Disconnecting all clients and shutting down the server..." << endl << endl;
						}

						else if (strcmp(buffer, "get;\n") == 0) {
							//lähetä vastaus
							//std::cerr << "Send response: " << sendBuffer << endl;
							SDLNet_TCP_Send(clientSocket[clientNumber], (void *)response.c_str(), response.length());

						}

						else { //
							int piste_n = uusiPaketti.pisteet.size();

							//cout << "Tuli " << uusiPaketti.pisteet.size() << " pistettä\n";
							//tulostaPaketti(uusiPaketti);

							//Odota lupaa kirjoittaa.
							while (paketitVarattu)
								SDL_Delay(1);

							paketitVarattu = true;

							//Jos paketti oli kunnollinen, työnnä vektoriin	 
							paketit.push_back(uusiPaketti);

							//Jos vektori on täynnä, poista alkupäästä. HUOM tätä pitää optimoida!
							while (paketit.size() > MAX_PAKETTEJA) {
								paketit.erase(paketit.begin());
							}

							paketitVarattu = false;
						}

						/*
						 *						// Send message to all other connected clients
						 *						int originatingClient = clientNumber;
						 *
						 *						for (int loop = 0; loop < MAX_CLIENTS; loop++)
						 *						{
						 *						// Send a message to the client saying "OK" to indicate the incoming connection has been accepted
						 *						strcpy( buffer, SERVER_NOT_FULL.c_str() );
						 *						int msgLength = strlen(buffer) + 1;
						 *
						 *						// If the message length is more than 1 (i.e. client pressed enter without entering any other text), then
						 *						// send the message to all connected clients except the client who originated the message in the first place
						 *						if (msgLength > 1 && loop != originatingClient && socketIsFree[loop] == false)
						 *						{
						 *						cout << "Retransmitting message: " << buffer << " (" << msgLength << " bytes) to client number: " << loop << endl;
						 *						SDLNet_TCP_Send(clientSocket[loop], (void *)buffer, msgLength);
						 }

						 }
						 */
						// If the client told us to shut down the server, then set the flag to get us out of the main loop and shut down


					}

				} // End of if client socket is active check

			} // End of server socket check sockets loop

		}

	}


	//void server() {
	bool setup(){

		// Älä aseta jos ollaan jo käynnissä
		if (running)
			return true;

		if (!initializeServer()) {
			std::cerr << "Ei voitu alustaa SDL_Net:iä\n";
			return false;
		}

		// Create the socket set with enough space to store our desired number of connections (i.e. sockets)
		socketSet = SDLNet_AllocSocketSet(MAX_SOCKETS);
		if (socketSet == NULL) {
			std::cerr << "Failed to allocate the socket set: " << SDLNet_GetError() << "\n";
			return false;
		}
		else {
			std::cerr << "Allocated socket set with size:  " << MAX_SOCKETS << ", of which " << MAX_CLIENTS << " are availble for use by clients." << std::endl;
		}


		// Initialize all the client sockets (i.e. blank them ready for use!)
		for (int loop = 0; loop < MAX_CLIENTS; loop++) {
			clientSocket[loop] = NULL;
			socketIsFree[loop] = true; // Set all our sockets to be free (i.e. available for use for new client connections)
		}


		// Try to resolve the provided server hostname. If successful, this places the connection details in the serverIP object and creates a listening port on the provided port number
		// Note: Passing the second parameter as "NULL" means "make a listening port". SDLNet_ResolveHost returns one of two values: -1 if resolving failed, and 0 if resolving was successful
		int hostResolved = SDLNet_ResolveHost(&serverIP, NULL, PORT);

		if (hostResolved == -1)	{
			std::cout << "Failed to resolve the server host: " << SDLNet_GetError() << std::endl;
		}
		else { // If we resolved the host successfully, output the details
			// Get our IP address in proper dot-quad format by breaking up the 32-bit unsigned host address and splitting it into an array of four 8-bit unsigned numbers...
			Uint8 * dotQuad = (Uint8*)&serverIP.host;

			//... and then outputting them cast to integers. Then read the last 16 bits of the serverIP object to get the port number
			std::cout << "Successfully resolved server host to IP: " << (unsigned short)dotQuad[0] << "." << (unsigned short)dotQuad[1] << "." << (unsigned short)dotQuad[2] << "." << (unsigned short)dotQuad[3];
			std::cout << " port " << SDLNet_Read16(&serverIP.port) << std::endl << std::endl;
		}


		// Try to open the server socket
		serverSocket = SDLNet_TCP_Open(&serverIP);

		if (!serverSocket) {
			std::cout << "Failed to open the server socket: " << SDLNet_GetError() << "\n";
			exit(-1);
		}
		else {
			std::cout << "Sucessfully created server socket." << std::endl;
		}


		// Add our server socket to the socket set
		SDLNet_TCP_AddSocket(socketSet, serverSocket);

		std::cout << "Awaiting clients..." << std::endl;

		running = true;
		return true;
	}


	void lopeta() {
		running = false;

		// Free our socket set (i.e. all the clients in our socket set)
		SDLNet_FreeSocketSet(socketSet);

		// Close our server socket, cleanup SDL_net and finish!
		SDLNet_TCP_Close(serverSocket);

		SDLNet_Quit();

		cerr << "OlentoServer suljettiin\n";

	}




	void testi() {
		char* testiData = "1.4\n1\n9.5\n4\n0\n0.0\n33\n16\n";

		pakettiDataT uusiPaketti = luoPaketti(testiData, strlen(testiData));
		cout << "Testipaketti: " << uusiPaketti.pisteet.size() << " pistettä\n";
		tulostaPaketti(uusiPaketti);

	}

	void asetaVastausviesti(vector<float> arvot) {

		response = "list ";
		for (float i = 0; i < arvot.size(); i++) {
			stringstream ss;
			ss << arvot[i];
			string luku = ss.str();
			response += luku;
			if (i < arvot.size() - 1) response += " ";
		}
		response += ";\n";
		/*response.copy(sendBuffer, response.length() );
		msgLength = response.length();*/
	}

}
