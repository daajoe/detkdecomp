// Hypergraph.cpp: implementation of the Hypergraph class.
//
//////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <ctime>
#include <algorithm>


#include "Hypergraph.h"
#include "Parser.h"
#include "Hyperedge.h"
#include "Node.h"
#include "Globals.h"

int G_NodeID = 0;
int G_EdgeID = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Hypergraph::Hypergraph()
{
	iMyMaxNbrOfEdges = 0;
	iMyMaxNbrOfNodes = 0;
	MyEdges = NULL;
	MyNodes = NULL;
}


Hypergraph::~Hypergraph()
{
	int i;

	for(i=0; i < iMyMaxNbrOfEdges; i++)
		delete MyEdges[i];
	for(i=0; i < iMyMaxNbrOfNodes; i++)
		delete MyNodes[i];

	delete [] MyEdges;
	delete [] MyNodes;
}



//////////////////////////////////////////////////////////////////////
// Class methods
//////////////////////////////////////////////////////////////////////



void Hypergraph::labelReachEdges(Hyperedge *Edge)
{
	Edge->setLabel(1);
	for(int i=0; i < Edge->getNbrOfNeighbours(); i++)
		if(Edge->getNeighbour(i)->getLabel() == 0)
			labelReachEdges(Edge->getNeighbour(i));
}


void Hypergraph::buildHypergraph(Parser *P)
{
    int i, j;

	// Allocate arrays with pointers to the edges and nodes of the hypergraph
 	MyEdges = new Hyperedge*[P->getNbrOfAtoms()];
	MyNodes = new Node*[P->getNbrOfVars()];

	if((MyEdges == NULL) || (MyNodes == NULL))
		writeErrorMsg("Error assigning memory.", "Hypergraph::buildHypergraph");
	else
	{
		iMyMaxNbrOfEdges = P->getNbrOfAtoms();
		iMyMaxNbrOfNodes = P->getNbrOfVars();
	}

	// Create the edges of the hypergraph
	for(i=0; i < iMyMaxNbrOfEdges; i++) {
		MyEdges[i] = new Hyperedge(G_EdgeID++, P->getAtom(i), P->getNbrOfVars(i), P->getNbrOfAtomNeighbours(i));
		if(MyEdges[i] == NULL)
			writeErrorMsg("Error assigning memory.", "Hypergraph::buildHypergraph");
	}

	// Create the nodes of the hypergraph
	for(i=0; i < iMyMaxNbrOfNodes; i++) {
		MyNodes[i] = new Node(G_NodeID++, P->getVariable(i), P->getNbrOfAtoms(i), P->getNbrOfVarNeighbours(i));
		if(MyNodes[i] == NULL)
			writeErrorMsg("Error assigning memory.", "Hypergraph::buildHypergraph");
	}

	// Set the pointers of the edges of the hypergraph
	for(i=0; i < iMyMaxNbrOfEdges; i++) {
		// Set the pointers of the actual edge to the nodes it contains
		for(j=0; j < P->getNbrOfVars(i); j++)
			MyEdges[i]->insNode(MyNodes[P->getNextAtomVar()], j);
		// Set the pointers of the actual edge to its neighbours, i.e., edges that have common nodes
		for(j=0; j < P->getNbrOfAtomNeighbours(i); j++)
			MyEdges[i]->insNeighbour(MyEdges[P->getNextAtomNeighbour()], j);
	}

	// Set the pointers of the nodes of the hypergraph
	for(i=0; i < iMyMaxNbrOfNodes; i++) {
		// Set the pointers of the actual node to the edges in which it is cointained
		for(j=0; j < P->getNbrOfAtoms(i); j++)
			MyNodes[i]->insEdge(MyEdges[P->getNextVarAtom()], j);
		// Set the pointers of the actual node to its neighbours, i.e., nodes that have common edges
		for(j=0; j < P->getNbrOfVarNeighbours(i); j++)
			MyNodes[i]->insNeighbour(MyNodes[P->getNextVarNeighbour()], j);
	}
}


int Hypergraph::getNbrOfEdges()
{
	return iMyMaxNbrOfEdges;
}


int Hypergraph::getNbrOfNodes()
{
	return iMyMaxNbrOfNodes;
}


Hyperedge *Hypergraph::getEdge(int iPos)
{
	if(iPos < iMyMaxNbrOfEdges)
		return MyEdges[iPos];
	else {
		writeErrorMsg("Position not available.", "Hypergraph::getEdge");
		return NULL;
	}
}


Node *Hypergraph::getNode(int iPos)
{
	if(iPos < iMyMaxNbrOfNodes)
		return MyNodes[iPos];
	else {
		writeErrorMsg("Position not available.", "Hypergraph::getNode");
		return NULL;
	}
}


Hyperedge *Hypergraph::getEdgeByID(int iID)
{
	Hyperedge *HEdge = NULL;

	for(int i=0; i < iMyMaxNbrOfEdges; i++)
		if(MyEdges[i]->getID() == iID) {
			HEdge = MyEdges[i];
			break;
		}

	return HEdge;	
}


Node *Hypergraph::getNodeByID(int iID)
{
	Node *Node = NULL;

	for(int i=0; i < iMyMaxNbrOfNodes; i++)
		if(MyNodes[i]->getID() == iID) {
			Node = MyNodes[i];
			break;
		}

	return Node;	
}


void Hypergraph::resetEdgeLabels(int iVal)
{
	for(int i=0; i < iMyMaxNbrOfEdges; i++)
		MyEdges[i]->setLabel(iVal);
}


void Hypergraph::resetNodeLabels(int iVal)
{
	for(int i=0; i < iMyMaxNbrOfNodes; i++)
		MyNodes[i]->setLabel(iVal);
}


bool Hypergraph::isConnected()
{
	// Label all edges reachable from the first edge
	resetEdgeLabels();
	if(iMyMaxNbrOfEdges > 0)
		labelReachEdges(MyEdges[0]);

	// Check whether all edges are labeled
	for(int i=0; i < iMyMaxNbrOfEdges; i++)
		if(MyEdges[i]->getLabel() == 0)
			return false;

	return true;
}


void Hypergraph::makeDual()
{
	int iTmp;
	Component **Tmp;

	// Swap hyperedges and nodes
	Tmp = (Component **)MyEdges;
	MyEdges = (Hyperedge **)MyNodes;
	MyNodes = (Node **)Tmp;

	// Swap limiters
	iTmp = iMyMaxNbrOfEdges;
	iMyMaxNbrOfEdges = iMyMaxNbrOfNodes;
	iMyMaxNbrOfNodes = iTmp;
}


void Hypergraph::reduce(bool bFinalOnly)
{
	int i, j, k;
	bool bCovered;
	list<int>::iterator ListIter;

	resetNodeLabels();
	for(i=0; i < iMyMaxNbrOfEdges; i++) {
		// Label all nodes in the hyperedge
		for(k=0; k < MyEdges[i]->getNbrOfNodes(); k++)
			MyEdges[i]->getNode(k)->setLabel(1);
		
		// Search for a hyperedge that is covered, i.e. whose nodes are labelled
		bFinalOnly && (i < iMyMaxNbrOfEdges-1) ? j=iMyMaxNbrOfEdges-1 : j=0;
		for(; j < iMyMaxNbrOfEdges; j++)
			if(j != i) {
				bCovered = true;
				for(k=0; k < MyEdges[j]->getNbrOfNodes(); k++)
					if(MyEdges[j]->getNode(k)->getLabel() == 0) {
						bCovered = false;
						break;
					}

				if(bCovered) {
					MyEdges[i]->insCoveredID(MyEdges[j]->getID());
					for(ListIter = MyEdges[j]->getCoveredIDs()->begin(); ListIter != MyEdges[j]->getCoveredIDs()->end(); ListIter++)
						MyEdges[i]->insCoveredID(*ListIter);

					// Remove the hyperedge
					--iMyMaxNbrOfEdges;
					for(k=0; k < MyEdges[j]->getNbrOfNodes(); k++)
						MyEdges[j]->getNode(k)->remEdge(MyEdges[j]);
					for(k=0; k < MyEdges[j]->getNbrOfNeighbours(); k++)
						MyEdges[j]->getNeighbour(k)->remNeighbour(MyEdges[j]);
					for(k=j; k < iMyMaxNbrOfEdges; k++)
						MyEdges[k] = MyEdges[k+1];
					MyEdges[iMyMaxNbrOfEdges] = NULL;
					if(j < i) --i; --j;
				}
			}
		
		// Reset all node labels in the hyperedge
		for(k=0; k < MyEdges[i]->getNbrOfNodes(); k++)
			MyEdges[i]->getNode(k)->setLabel(0);
	}
}


void Hypergraph::updateNeighbourhood()
{
	int i;

	for(i=0; i < iMyMaxNbrOfNodes; i++){
		MyNodes[i]->updateNeighbourhood();
	}

	for(i=0; i < iMyMaxNbrOfEdges; i++){   
			MyEdges[i]->updateNeighbourhood();
	}
	
}


void Hypergraph::updateCompSizes()
{
	int i;

	for(i=0; i < iMyMaxNbrOfNodes; i++)
		MyNodes[i]->updateSize();
	for(i=0; i < iMyMaxNbrOfEdges; i++)
		MyEdges[i]->updateSize();
}


Node **Hypergraph::getInputOrder()
{
	int i;
	Node **VarOrder;

	VarOrder = new Node*[iMyMaxNbrOfNodes+1];
	if(VarOrder == NULL)
		writeErrorMsg("Error assigning memory.", "Hypergraph::getInputOrder");

	// Copy pointers to the nodes from the hypergraph into the array
	for(i=0; i < iMyMaxNbrOfNodes; i++)
		VarOrder[i] = MyNodes[i];
	VarOrder[i] = NULL;

	return VarOrder;
}


Node **Hypergraph::getMIWOrder()
{
	int *iOrder, iMinDegree, iMinDegreePos, i, j;
	Node **VarOrder;
	set<Node *> **NodeNeighbours;
	set<Node *>::iterator SetIter, SetIter2;
	vector<int> Candidates;

	iOrder = new int[iMyMaxNbrOfNodes];
	VarOrder = new Node*[iMyMaxNbrOfNodes+1];
	if((iOrder == NULL) || (VarOrder == NULL))
		writeErrorMsg("Error assigning memory.", "Hypergraph::getMIWOrder");

	// Create storage space for the neighbours of each node
	NodeNeighbours = new set<Node *>*[iMyMaxNbrOfNodes];
	if(NodeNeighbours == NULL)
		writeErrorMsg("Error assigning memory.", "Hypergraph::getMIWOrder");
	for(i=0; i < iMyMaxNbrOfNodes; i++) {
		NodeNeighbours[i] = new set<Node *>;
		if(NodeNeighbours[i] == NULL)
			writeErrorMsg("Error assigning memory.", "Hypergraph::getMIWOrder");
	}

	// Initialize variable order and set the set of neighbours of each node
	for(i=0; i < iMyMaxNbrOfNodes; i++) {
		VarOrder[i] = MyNodes[i];
		VarOrder[i]->setLabel(i);
		for(j=0; j < VarOrder[i]->getNbrOfNeighbours(); j++)
			NodeNeighbours[i]->insert(VarOrder[i]->getNeighbour(j));
	}
	VarOrder[i] = NULL;

	// Remove nodes with smallest degree iteratively
	for(i=0; i < iMyMaxNbrOfNodes; i++) {

		// Search for the first node that has not been removed yet
		for(j=0; NodeNeighbours[j] == NULL; j++);
		iMinDegree = (int)NodeNeighbours[j]->size();
		Candidates.push_back(j);

		// Search for the node with smallest degree
		for(++j; j < iMyMaxNbrOfNodes; j++)
			if(NodeNeighbours[j] != NULL)
				if((int)NodeNeighbours[j]->size() <= iMinDegree) {
					if((int)NodeNeighbours[j]->size() < iMinDegree) {
						iMinDegree = (int)NodeNeighbours[j]->size();
						Candidates.clear();
					}
					Candidates.push_back(j);
				}

		// Randomly select the next node with smallest degree
		iMinDegreePos = Candidates[random_range(0, (int)Candidates.size()-1)];
		Candidates.clear();

		// Disconnect the selected node and connect all its neighbours
		for(SetIter = NodeNeighbours[iMinDegreePos]->begin(); SetIter != NodeNeighbours[iMinDegreePos]->end(); SetIter++) {
			
			// Search for the selected node in the neighbour set of each neighbour
			SetIter2 = NodeNeighbours[(*SetIter)->getLabel()]->find(VarOrder[iMinDegreePos]);
			if(SetIter2 == NodeNeighbours[(*SetIter)->getLabel()]->end())
				writeErrorMsg("Illegal neighbourhood relation.", "Hypergraph::getMIWOrder");
			
			// Remove the selected node from the neighbour set of each neighbour
			NodeNeighbours[(*SetIter)->getLabel()]->erase(*SetIter2);
			
			// Connect all neighbours of the selected node
			for(SetIter2=NodeNeighbours[iMinDegreePos]->begin(); SetIter2 != NodeNeighbours[iMinDegreePos]->end(); SetIter2++)
				if(*SetIter2 != *SetIter)
					NodeNeighbours[(*SetIter)->getLabel()]->insert(*SetIter2);
		}

		// Remove the selected node
		delete NodeNeighbours[iMinDegreePos];
		NodeNeighbours[iMinDegreePos] = NULL;

		// Remember the order of the node removal
		iOrder[iMinDegreePos] = iMyMaxNbrOfNodes-i;
	}

	// Sort variables according to their removal order
	sortPointers((void **)VarOrder, iOrder, 0, iMyMaxNbrOfNodes-1);

	delete [] NodeNeighbours;
	delete [] iOrder;

	return VarOrder;
}


Node **Hypergraph::getMFOrder()
{
	int *iOrder, iMinFill, iMinFillPos, iTmp, i, j;
	Node **VarOrder;
	set<Node *> **NodeNeighbours, *Neighbours;
	set<Node *>::iterator SetIter, SetIter2;
	vector<int> Candidates;

	iOrder = new int[iMyMaxNbrOfNodes];
	VarOrder = new Node*[iMyMaxNbrOfNodes+1];
	if((iOrder == NULL) || (VarOrder == NULL))
		writeErrorMsg("Error assigning memory.", "Hypergraph::getMFOrder");

	// Create storage space for the neighbours of each node
	NodeNeighbours = new set<Node *>*[iMyMaxNbrOfNodes];
	if(NodeNeighbours == NULL)
		writeErrorMsg("Error assigning memory.", "Hypergraph::getMFOrder");
	for(i=0; i < iMyMaxNbrOfNodes; i++) {
		NodeNeighbours[i] = new set<Node *>;
		if(NodeNeighbours[i] == NULL)
			writeErrorMsg("Error assigning memory.", "Hypergraph::getMFOrder");
	}

	// Initialize variable order and set the set of neighbours of each node
	for(i=0; i < iMyMaxNbrOfNodes; i++) {
		VarOrder[i] = MyNodes[i];
		VarOrder[i]->setLabel(i);
		for(j=0; j < VarOrder[i]->getNbrOfNeighbours(); j++)
			NodeNeighbours[i]->insert(VarOrder[i]->getNeighbour(j));
	}
	VarOrder[i] = NULL;

	// Remove nodes with smallest fill-in set iteratively
	for(i=0; i < iMyMaxNbrOfNodes; i++) {

		// Search for the first node that has not been removed yet
		for(j=0; NodeNeighbours[j] == NULL; j++);
		// Compute the cardinality of the  minimum fill-in set
		for(iMinFill = 0, SetIter = NodeNeighbours[j]->begin(); SetIter != NodeNeighbours[j]->end();) {
			Neighbours = NodeNeighbours[(*SetIter)->getLabel()];
			for(SetIter2 = ++SetIter; SetIter2 != NodeNeighbours[j]->end(); SetIter2++)
				if(Neighbours->find(*SetIter2) == Neighbours->end())
					++iMinFill;
		}
		Candidates.push_back(j);

		// Search for the node with smallest minimum fill-in set
		for(++j; j < iMyMaxNbrOfNodes; j++) {
			if(NodeNeighbours[j] != NULL) {
				// Compute the cardinality of the  minimum fill-in set
				for(iTmp = 0, SetIter = NodeNeighbours[j]->begin(); SetIter != NodeNeighbours[j]->end();) {
					Neighbours = NodeNeighbours[(*SetIter)->getLabel()];
					for(SetIter2 = ++SetIter; SetIter2 != NodeNeighbours[j]->end(); SetIter2++)
						if(Neighbours->find(*SetIter2) == Neighbours->end())
							++iTmp;
				}
				if(iTmp <= iMinFill) {
					if(iTmp < iMinFill) {
						iMinFill = iTmp;
						Candidates.clear();
					}
					Candidates.push_back(j);
				}
			}
		}

		// Randomly select the next node with minimum fill-in set
		iMinFillPos = Candidates[random_range(0, (int)Candidates.size()-1)];
		Candidates.clear();

		// Disconnect the selected node and connect all its neighbours
		for(SetIter = NodeNeighbours[iMinFillPos]->begin(); SetIter != NodeNeighbours[iMinFillPos]->end(); SetIter++) {
			
			// Search for the selected node in the neighbour set of each neighbour
			SetIter2 = NodeNeighbours[(*SetIter)->getLabel()]->find(VarOrder[iMinFillPos]);
			if(SetIter2 == NodeNeighbours[(*SetIter)->getLabel()]->end())
				writeErrorMsg("Illegal neighbourhood relation.", "Hypergraph::getMIWOrder");
			
			// Remove the selected node from the neighbour set of each neighbour
			NodeNeighbours[(*SetIter)->getLabel()]->erase(*SetIter2);
			
			// Connect all neighbours of the selected node
			for(SetIter2=NodeNeighbours[iMinFillPos]->begin(); SetIter2 != NodeNeighbours[iMinFillPos]->end(); SetIter2++)
				if(*SetIter2 != *SetIter)
					NodeNeighbours[(*SetIter)->getLabel()]->insert(*SetIter2);
		}

		// Remove the selected node
		delete NodeNeighbours[iMinFillPos];
		NodeNeighbours[iMinFillPos] = NULL;

		// Remember the order of the node removal
		iOrder[iMinFillPos] = iMyMaxNbrOfNodes-i;
	}

	// Sort variables according to their removal order
	sortPointers((void **)VarOrder, iOrder, 0, iMyMaxNbrOfNodes-1);

	delete [] NodeNeighbours;
	delete [] iOrder;

	return VarOrder;
}


Node **Hypergraph::getMCSOrder()
{
	int *iOrder, iMaxCard, iTmpCard, iMaxCardPos, iInitialVertex, i, j, k;
	Node **VarOrder, *Var;
	vector<int> Candidates;

	iOrder = new int[iMyMaxNbrOfNodes];
	VarOrder = new Node*[iMyMaxNbrOfNodes+1];
	if((iOrder == NULL) || (VarOrder == NULL))
		writeErrorMsg("Error assigning memory.", "Hypergraph::getMCSOrder");

	// Initialize variable order
	for(i=0; i < iMyMaxNbrOfNodes; i++) {
		VarOrder[i] = MyNodes[i];
		VarOrder[i]->setLabel(0);
	}
	VarOrder[i] = NULL;

	// Select randomly an initial vertex
	iInitialVertex = random_range(0, iMyMaxNbrOfNodes-1);
	iOrder[iInitialVertex] = 0;
	VarOrder[iInitialVertex]->setLabel(1);

	// Remove nodes with highest connectivity iteratively
	for(i=1; i < iMyMaxNbrOfNodes; i++) {

		// Search for the first node that has not been removed yet and count its connectivity
		for(iMaxCard=j=0; MyNodes[j]->getLabel() != 0; j++);
		Var = MyNodes[j];
		for(k=0; k < Var->getNbrOfNeighbours(); k++)
			iMaxCard += Var->getNeighbour(k)->getLabel();
		Candidates.push_back(j);

		// Search for the node with highest connectivity, 
		// i.e., with highest number of neighbours in the set of nodes already removed
		for(++j; j < iMyMaxNbrOfNodes; j++) {
			Var = MyNodes[j];
			if(Var->getLabel() == 0) {
				iTmpCard = 0;
				for(k=0; k < Var->getNbrOfNeighbours(); k++)
					iTmpCard += Var->getNeighbour(k)->getLabel();
				if(iTmpCard >= iMaxCard) {
					if(iTmpCard > iMaxCard) {
						iMaxCard = iTmpCard;
						Candidates.clear();
					}
					Candidates.push_back(j);
				}
			}
		}

		// Randomly select the next node with highest connectivity
		iMaxCardPos = Candidates[random_range(0, (int)Candidates.size()-1)];
		Candidates.clear();

		// Remove the selected node
		VarOrder[iMaxCardPos]->setLabel(1);

		// Remember the order of the node removal
		iOrder[iMaxCardPos] = i;
	}

	// Sort variables according to their removal order
	sortPointers((void **)VarOrder, iOrder, 0, iMyMaxNbrOfNodes-1);

	delete [] iOrder;

	return VarOrder;
}


Node **Hypergraph::getRandomOrder()
{
	int i;
	Node **VarOrder;
	vector<int> iOrder;

	VarOrder = new Node*[iMyMaxNbrOfNodes+1];
	if(VarOrder == NULL)
		writeErrorMsg("Error assigning memory.", "Hypergraph::getRandomOrder");

	// Initialize variable order
	for(i=0; i < iMyMaxNbrOfNodes; i++)
		iOrder.push_back(i);

	random_shuffle(iOrder.begin(), iOrder.end());

	// Store randomized variable order
	for(i=0; i < iMyMaxNbrOfNodes; i++)
		VarOrder[i] = MyNodes[iOrder[i]];
	VarOrder[i] = NULL;

	return VarOrder;
}


