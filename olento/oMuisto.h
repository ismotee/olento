#ifndef OLENTO_OMUISTO_H
#define OLENTO_OMUISTO_H 

#include <vector>

struct oMuisto 
{

	std::vector<int> muistiIds;
	bool end;
	int currentId;

	oMuoto();

	void add(int i);

    int& operator++(); //prefix
};

#endif