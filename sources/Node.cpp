// Node.cpp: implementation of the Node class.
//
//////////////////////////////////////////////////////////////////////

#include "Node.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Node::Node(int ID, char *cName, int iSize, int iNbrOfNeighbours, bool bSpecial) : Component(ID, cName, iSize, iNbrOfNeighbours, bSpecial)
{
}


Node::~Node()
{
}



//////////////////////////////////////////////////////////////////////
// Class methods
//////////////////////////////////////////////////////////////////////


int Node::getNbrOfEdges()
{
	return Component::size();
}


void Node::insEdge(Hyperedge *Comp, int iPos)
{
	Component::ins((Component *)Comp, iPos);
}


bool Node::remEdge(Hyperedge *Comp)
{
	return Component::rem((Component *)Comp);
}


Hyperedge *Node::getEdge(int iPos)
{
	return (Hyperedge *)Component::get(iPos);
}


void Node::insNeighbour(Node *Neighbour, int iPos)
{
	Component::insNeighbour((Component *)Neighbour, iPos);
}


bool Node::remNeighbour(Node *Neighbour)
{
	return Component::remNeighbour((Component *)Neighbour);
}


Node *Node::getNeighbour(int iPos)
{
	return (Node *)Component::getNeighbour(iPos);
}


Node *Node::clone()
{
	return (Node *)Component::clone();
}

