// Hyperedge.cpp: implementation of the Hyperedge class.
//
//////////////////////////////////////////////////////////////////////

#include "Hyperedge.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Hyperedge::Hyperedge(int ID, char *cName, int iSize, int iNbrOfNeighbours, bool bSpecial) : Component(ID, cName, iSize, iNbrOfNeighbours, bSpecial)
{
}


Hyperedge::~Hyperedge()
{
}



//////////////////////////////////////////////////////////////////////
// Class methods
//////////////////////////////////////////////////////////////////////


int Hyperedge::getNbrOfNodes()
{
	return Component::size();
}


void Hyperedge::insNode(Node *Comp, int iPos)
{
	Component::ins((Component *)Comp, iPos);
}


bool Hyperedge::remNode(Node *Comp)
{
	return Component::rem((Component *)Comp);
}


Node *Hyperedge::getNode(int iPos)
{
	return (Node *)Component::get(iPos);
}


void Hyperedge::insNeighbour(Hyperedge *Neighbour, int iPos)
{
	Component::insNeighbour((Component *)Neighbour, iPos);
}


bool Hyperedge::remNeighbour(Hyperedge *Neighbour)
{
	return Component::remNeighbour((Component *)Neighbour);
}


Hyperedge *Hyperedge::getNeighbour(int iPos)
{
	return (Hyperedge *)Component::getNeighbour(iPos);
}


Hyperedge *Hyperedge::clone()
{
	return (Hyperedge *)Component::clone();
}

