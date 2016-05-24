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
	muodonArvot.resize(7);

	olentoServer::aloita(); //käynnistyy omaan threadiin

	std::thread olentogl(run);

	//hae paketti

	//paketti -> neuralnet -> arvot

	//asetaMuoto(arvot)
	
	dClock t;

	while(olentoPyorii() ) {

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
		for (int i = 1; i < 7; i++)
			muodonArvot[i] = randf(-1, 1);

		//muuta muotoa gl:ään
		asetaMuoto(muodonArvot);

		//ajasta luuppi (1 fps)
		t.delay(1);
	}
	
	std::cerr << "olentogl suljettiin\n";

	olentoServer::lopeta();

	olentogl.join();

	return 0;
}