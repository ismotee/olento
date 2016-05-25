//main.cpp
#include "olento.h"
#include "random.h"
#include "dClock.h"
#include "server.h"

#include <thread>
#include <vector>
#include <iostream>

int main(int argc, char* argv[]) {
	
	std::vector<float> muodonArvot;
	muodonArvot.resize(8);

	olentoServer::aloita(); //käynnistyy omaan threadiin

	std::thread olentogl(run);

	dClock t;
	int frame = 0;
	do {

		t.reset(); //ota aikaa
		
		//hae paketti
		olentoServer::pakettiDataT p = olentoServer::haePaketti();
		while (!p.empty()) {
			std::cout << "Paketti:\n";
			tulostaPaketti(p);
			p = olentoServer::haePaketti();
		}
		
		//luo paketista arvot neuralnetin avulla...

		//tee sattumanvarainen muoto
		muodonArvot[0] = randf(0, 3);
		for (int i = 1; i < 5; i++) muodonArvot[i] = randf(-1, 1);
		muodonArvot[5] = randf(0, 4.999);
		muodonArvot[6] = randf(0, 0.999);
		muodonArvot[7] = randf(0, 0.999);

		//muuta muotoa gl:ään
		asetaMuoto(muodonArvot);
		olentoServer::asetaVastausviesti(muodonArvot);
		
		//ajasta luuppi (1 fps)
		t.delay(1);
		std::cout << frame++ << "\n";
	} while(olentoPyorii());
	
	std::cerr << "olentogl suljettiin\n";

	olentoServer::lopeta();

	olentogl.join();

	return 0;
}