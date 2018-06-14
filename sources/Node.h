// Models a node of a hypergraph.
//
//////////////////////////////////////////////////////////////////////


#if !defined(CLS_NODE)
#define CLS_NODE


#include "Component.h"


class Hyperedge;

class Node : public Component
{
public:
	// Constructor
	Node(int ID, char *cName, int iSize, int iNbrOfNeighbours, bool bSepcial = false);

	// Destructor
	virtual~Node();

	// Returns the number of hyperedges containing the node
	int getNbrOfEdges();

	// Inserts a hyperedge at position iPos
	void insEdge(Hyperedge *Comp, int iPos);

	// Removes a hyperedge
	bool remEdge(Hyperedge *Comp);

	// Returns the hyperedge contained at position iPos
	Hyperedge *getEdge(int iPos);

	// Inserts a neighbour node at position iPos
	void insNeighbour(Node *Neighbour, int iPos);

	// Removes a neighbour node
	bool remNeighbour(Node *Neighbour);

	// Returns the neighbour node at position iPos
	Node *getNeighbour(int iPos);

	// Creates a clone of the node
	Node *clone();
};


#endif // !defined(CLS_NODE);

