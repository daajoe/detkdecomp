// Component.cpp: implementation of the Component class.
//
//////////////////////////////////////////////////////////////////////

#include <cstdlib>

#include "Component.h"
#include "Globals.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Component::Component(int ID, char *cName, int iSize, int iNbrOfNeighbours, bool bSpecial)
{
	int i;
	iMyID = ID;
	cMyName = cName;
	bMySpecial = bSpecial;
	
	MyComponents = new Component*[iSize];
	if(MyComponents == NULL)
		writeErrorMsg("Error assigning memory.", "Component::Component");
	for(i=0; i < iSize; i++)
		MyComponents[i] = NULL;
	iMyMaxSize = iSize;

	MyNeighbours = new Component*[iNbrOfNeighbours];
	if(MyNeighbours == NULL)
		writeErrorMsg("Error assigning memory.", "Component::Component");
	for(i=0; i < iNbrOfNeighbours; i++)
		MyNeighbours[i] = NULL;
	iMyMaxNbrOfNeighbours = iNbrOfNeighbours;

	iMyLabel = 0;
	iMyWeight = 1;
}


Component::~Component()
{
	delete [] MyComponents;
	delete [] MyNeighbours;
}



//////////////////////////////////////////////////////////////////////
// Class methods
//////////////////////////////////////////////////////////////////////


int Component::getID()
{
	return iMyID;
}


char *Component::getName()
{
	return cMyName;
}


int Component::size()
{
	return iMyMaxSize;
}


int Component::getNbrOfNeighbours()
{
	return iMyMaxNbrOfNeighbours;
}


void Component::updateNeighbourhood()
{
	int i, j;
	set<Component *> Neighbours;
	set<Component *>::iterator SetIter;

	// Compute new neighbours
	for(i=0; i < iMyMaxSize; i++)
		for(j=0; j < MyComponents[i]->iMyMaxSize; j++)
			if(MyComponents[i]->get(j) != this)
				Neighbours.insert(MyComponents[i]->get(j));

	// Allocate memory for pointers to new neighbours
	if(MyNeighbours != NULL)
		delete [] MyNeighbours;
	iMyMaxNbrOfNeighbours = (int)Neighbours.size();
	MyNeighbours = new Component*[iMyMaxNbrOfNeighbours];
	if(MyNeighbours == NULL)
		writeErrorMsg("Error assigning memory.", "Component::updateNeighbourhood");

	// Set new neighbours
	for(i=0, SetIter=Neighbours.begin(); SetIter != Neighbours.end(); i++, SetIter++)
		MyNeighbours[i] = *SetIter;
}


void Component::updateSize()
{
	int i;

	for(i=0; (i < iMyMaxSize) && (MyComponents[i] != NULL); i++);
	iMyMaxSize = i;
}


void Component::ins(Component *Comp, int iPos)
{
	if(iPos < iMyMaxSize)
		MyComponents[iPos] = Comp;
	else
		writeErrorMsg("Position not available.", "Component::ins");
}


bool Component::rem(Component *Comp)
{
	int i;
	bool bFound = false;

	for(i=0; i < iMyMaxSize; i++)
		if(MyComponents[i] == Comp) {
			bFound = true;
			break;
		}

	if(bFound) {
		--iMyMaxSize;
		for(; i < iMyMaxSize; i++)
			MyComponents[i] = MyComponents[i+1];
		MyComponents[iMyMaxSize] = NULL;
	}

	return bFound;
}


Component *Component::get(int iPos)
{
	if(iPos < iMyMaxSize)
		return MyComponents[iPos];
	else {
		writeErrorMsg("Position not available.", "Component::get");
		return NULL;
	}
}


void Component::insNeighbour(Component *Neighbour, int iPos)
{
	if(iPos < iMyMaxNbrOfNeighbours)
		MyNeighbours[iPos] = Neighbour;
	else
		writeErrorMsg("Position not available.", "Component::insNeighbour");
}


bool Component::remNeighbour(Component *Neighbour)
{
	int i;
	bool bFound = false;

	for(i=0; i < iMyMaxNbrOfNeighbours; i++)
		if(MyNeighbours[i] == Neighbour) {
			bFound = true;
			break;
		}

	if(bFound) {
		--iMyMaxNbrOfNeighbours;
		for(; i < iMyMaxNbrOfNeighbours; i++)
			MyNeighbours[i] = MyNeighbours[i+1];
		MyNeighbours[iMyMaxNbrOfNeighbours] = NULL;
	}

	return bFound;
}


Component *Component::getNeighbour(int iPos)
{
	if(iPos < iMyMaxNbrOfNeighbours)
		return MyNeighbours[iPos];
	else {
		writeErrorMsg("Position not available.", "Component::getNeighbour");
		return NULL;
	}
}


void Component::setLabel(int iLabel)
{
	iMyLabel = iLabel;
}


void Component::incLabel(int iInc)
{
	iMyLabel += iInc;
}


void Component::decLabel(int iDec)
{
	iMyLabel -= iDec;
}


int Component::getLabel()
{
	return iMyLabel;
}


bool Component::isSpecial()
{
	return bMySpecial;
}


Component *Component::clone()
{
	int i;
	Component *Clone;
	list<int>::iterator ListIter;

	// Create clone
	Clone = new Component(iMyID, cMyName, iMyMaxSize, 0, bMySpecial);
	if(Clone == NULL)
		writeErrorMsg("Error assigning memory.", "Component::clone");
		
	// Initialize pointers
	for(i=0; i < iMyMaxSize; i++)
		Clone->MyComponents[i] = NULL;
	Clone->MyNeighbours = NULL;

	// Copy covered component IDs
	for(ListIter = MyCoveredCompIDs.begin(); ListIter != MyCoveredCompIDs.end(); ListIter++)
		Clone->MyCoveredCompIDs.push_back(*ListIter);

	// copy label and weight
	Clone->setLabel(iMyLabel);
	Clone->setWeight(iMyWeight);
	
	return Clone;
}


void Component::insCoveredID(int iID)
{
	MyCoveredCompIDs.push_back(iID);
}


list<int> *Component::getCoveredIDs()
{
	return &MyCoveredCompIDs;
}


int Component::getWeight()
{
	return iMyWeight;
}


void Component::setWeight(int iWeight)
{
	iMyWeight = iWeight;
}

