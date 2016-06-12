
#include "oMuisto.h"

oMuisto::oMuisto () : currentId(-1)
{
	
}

void oMuisto::add (int i)
{
	muistoIds.push_back(i);
}

int& oMuisto::operator++ ()
{
	if(++currentId >= muistoIds.size()) {
		currentId = -1;
		return currentId;
	}
	return muistoIds[currentId];
}

