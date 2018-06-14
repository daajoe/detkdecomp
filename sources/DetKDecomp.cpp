// DetKDecomp.cpp: implementation of the DetKDecomp class.
//
//////////////////////////////////////////////////////////////////////


#include <cstdio>
#include <iostream>
#include <cmath>
#include <algorithm>

#include <list>
#include <vector>

using namespace std;

#include "DetKDecomp.h"
#include "Hypertree.h"
#include "Hypergraph.h"
#include "Hyperedge.h"
#include "Node.h"
#include "Globals.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


DetKDecomp::DetKDecomp()
{
	MyHGraph = NULL;
	iMyK = 0;
}


DetKDecomp::~DetKDecomp()
{
}



//////////////////////////////////////////////////////////////////////
// Class methods
//////////////////////////////////////////////////////////////////////


/*
***Description***
The method selects an initial subset within a set of hyperedges such that a given set of 
nodes is covered.

INPUT:	Nodes: Array of nodes to be covered
		Edges: Array of hyperedges
		bInComp: Boolean array indicating the position of each hyperedge
		iSize: Number of elements in Edges, bInComp, and CovWeights
OUTPUT: Set: Selection of hyperedges in Edges such that all nodes in Nodes are covered
		CovWeights: Array with the number of connector nodes covered by each hyperedge
		return: Number of selected hyperedges; -1 if nodes cannot be covered
*/

int DetKDecomp::setInitSubset(Node **Nodes, Hyperedge **Edges, int *Set, bool *bInComp, int *CovWeights, int iSize)
{
	int iUncov, iWeight, i, j;

	// Reset node labels
	MyHGraph->resetNodeLabels(-1);
	for(iUncov=0; Nodes[iUncov] != NULL; iUncov++)
		Nodes[iUncov]->setLabel(0);

	// Sort hyperedges according to their weight,
	// i.e., the number of nodes in Nodes they contain
	for(i=0; i < iSize; i++) {
		Edges[i]->setLabel((int)bInComp[i]);
		CovWeights[i] = 0;
		for(j=0; j < Edges[i]->getNbrOfNodes(); j++)
			if(Edges[i]->getNode(j)->getLabel() == 0)
				--CovWeights[i];
	}
	sortPointers((void **)Edges, CovWeights, 0, iSize-1);

	// Reset bInComp and summarize weights
	for(i=0; i < iSize; i++) {
		Edges[i]->getLabel() == 0 ? bInComp[i] = false : bInComp[i] = true;
		CovWeights[i] = -CovWeights[i];
	}
	iWeight = 0;
	for(i=iSize-1; i >= 0; i--) {
		iWeight += CovWeights[i];
		CovWeights[i] = iWeight;
	}

	// Select an initial subset of at most iMyK hyperedges
	return coverNodes(Edges, Set, bInComp, CovWeights, iSize, iUncov, false);
}


/*
***Description***
The method selects the next subset within a set of hyperedges such that a given set of 
nodes is covered.

INPUT:	Nodes: Array of nodes to be covered
		Edges: Array of hyperedges
		Set: Integer array of indices in Edges representing a subset selection
		bInComp: Boolean array indicating the position of each hyperedge
		CovWeights: Array with the number of connector nodes covered by each hyperedge
		iSize: Number of elements in Edges, bInComp, and CovWeights
OUTPUT: Set: Selection of hyperedges in Edges such that all nodes in Nodes are covered
		return: Number of selected hyperedges; -1 if there is no alternative selection
*/

int DetKDecomp::setNextSubset(Node **Nodes, Hyperedge **Edges, int *Set, bool *bInComp, int *CovWeights, int iSize)
{
	int iUncov;

	MyHGraph->resetNodeLabels(-1);
	for(iUncov=0; Nodes[iUncov] != NULL; iUncov++)
		Nodes[iUncov]->setLabel(0);

	// Select the next subset of at most iMyK hyperedges
	return coverNodes(Edges, Set, bInComp, CovWeights, iSize, iUncov, true);
}


/*
***Description***
The method selects elements in a Boolean array representing a subset selection within 
a set of hyperedges such that a given set of nodes is covered. If bReconstr is false, the first
subset selection covering the given nodes is chosen. Otherwise, the search tree of the
previous selection is reconstructed and it is searched for the next selection. It is assumed that
all nodes to be covered are labeled by 0 and all other nodes are labeled by -1.

INPUT:	Edges: Array of hyperedges
		Set: Integer array of indices in Edges representing a subset selection
		bInComp: Boolean array indicating the position of each hyperedge
		CovWeights: Array with the number of connector nodes covered by each hyperedge
		iSize: Number of elements in Edges, Set, bInComp, and CovWeights
		iUncovered: Number of nodes not covered by some hyperedge
		bReconstr: true if the search tree has to be reconstructed according to Set
OUTPUT: Set: Selection of hyperedges in Edges such that all nodes are covered
		return: Number of selected hyperedges; -1 if there is no such selection
*/

int DetKDecomp::coverNodes(Hyperedge **Edges, int *Set, bool *bInComp, int *CovWeights, int iSize, int iUncovered, bool bReconstr)
{
	int iPos, iNbrSel, *iTmpLabels, iWeight, iInCompSel, i;
	bool bSelect, bCovered, bBack;
	list<int *> LabelStack;
	list<int *>::iterator ListIter;

	iPos = iNbrSel = iInCompSel = 0;
	iUncovered == 0 ? bCovered = true : bCovered = false;

	// Reconstruct the search tree according to Set
	if(bReconstr) {
		if(Set[0] == -1) return -1;

		for(iNbrSel=0; Set[iNbrSel+1] != -1; iNbrSel++) {
			iPos = Set[iNbrSel];
			if(bInComp[iPos])
				++iInCompSel;

			iTmpLabels = new int[Edges[iPos]->getNbrOfNodes()];
			if(iTmpLabels == NULL)
				writeErrorMsg("Error assigning memory.", "DetKDecomp::coverNodes");
	
			// Save labels of nodes in the actual hyperedge and label these nodes by 1
			for(i=0; i < Edges[iPos]->getNbrOfNodes(); i++) {
				iTmpLabels[i] = Edges[iPos]->getNode(i)->getLabel();
				Edges[iPos]->getNode(i)->setLabel(1);
			}

			LabelStack.push_back(iTmpLabels);
		}

		iPos = Set[iNbrSel];
		iUncovered = 0;
		for(i=0; i < Edges[iPos]->getNbrOfNodes(); i++)
			if(Edges[iPos]->getNode(i)->getLabel() == 0)
				++iUncovered;
		++iPos;
	}

	// Search for a new set of covering hyperedges
	while(!bCovered) {
		for(bBack=false; !bCovered; iPos++) {

			// Check if nodes can be covered by the remaining hyperedges;
			// prune the search tree if not
			i = iPos + (iMyK-iNbrSel);
			if(i < iSize)
				iWeight = CovWeights[iPos] - CovWeights[i];
			else
				if(iPos < iSize)
					iWeight = CovWeights[iPos];
				else
					iWeight = 0;
			if((iWeight < iUncovered) || (iWeight == 0)) {
				bBack = true;
				break;
			}

			// Check if the actual hyperedge covers some uncovered node
			bSelect = false;
			if(bInComp[iPos] || (iInCompSel > 0) || (iNbrSel < iMyK-1))
				for(i=0; i < Edges[iPos]->getNbrOfNodes(); i++)
					if(Edges[iPos]->getNode(i)->getLabel() == 0) {
						bSelect = true;
						break;
					}

			if(bSelect) {
				// Select the actual hyperedge
				Set[iNbrSel++] = iPos;
				if(bInComp[iPos])
					++iInCompSel;

				iTmpLabels = new int[Edges[iPos]->getNbrOfNodes()];
				if(iTmpLabels == NULL)
					writeErrorMsg("Error assigning memory.", "DetKDecomp::coverNodes");
	
				// Save labels of nodes in the actual hyperedge and label these nodes by 1
				for(i=0; i < Edges[iPos]->getNbrOfNodes(); i++) {
					iTmpLabels[i] = Edges[iPos]->getNode(i)->getLabel();
					Edges[iPos]->getNode(i)->setLabel(1);
					if(iTmpLabels[i] == 0)
						--iUncovered;
				}
				LabelStack.push_back(iTmpLabels);

				// Check whether all nodes are covered
				if(iUncovered == 0)
					bCovered = true;
			}
		}

		if(bBack) {
			if(iNbrSel == 0) {
				// No more possibilities to cover the nodes
				iNbrSel = -1;
				break;
			}
			else {
				// Undo the last selection if the nodes cannot be covered in this way
				iPos = Set[--iNbrSel];
				if(bInComp[iPos])
					--iInCompSel;
				
				iTmpLabels = LabelStack.back();
				LabelStack.pop_back();
				
				for(i=0; i < Edges[iPos]->getNbrOfNodes(); i++) {
					Edges[iPos]->getNode(i)->setLabel(iTmpLabels[i]);
					if(iTmpLabels[i] == 0)
						++iUncovered;
				}
				delete [] iTmpLabels;
				++iPos;
			}
		}
	}

	if(iNbrSel >= 0)
		Set[iNbrSel] = -1;
	for(ListIter=LabelStack.begin(); ListIter != LabelStack.end(); ListIter++)
		delete [] *ListIter;

	return iNbrSel;
}


/*
***Description***
The method labels all unlabeled hyperedges reachable from Edge with iLabel. It is assumed that
all separating nodes/edges are labeled by -1 and all other nodes/edges are labeled by 0.

INPUT:	Edge: Hyperedge
		iLabel: Label of all hyperedges in the same component as Edge
OUTPUT: Egdes: List of all hyperedges in the same component as Edge
        Connector: List of nodes connecting the component with the separator
*/

void DetKDecomp::collectReachEdges(Hyperedge *Edge, int iLabel, list<Hyperedge *> *Edges, list<Node *> *Connector)
{
	int iNbrOfNodes, iNbrOfEdges, i, j;
	Node *ConnNode;
	list<Hyperedge *>::iterator EdgeIter;

	Edge->setLabel(iLabel);
	Edges->push_back(Edge);

	for(EdgeIter=Edges->begin(); EdgeIter != Edges->end(); EdgeIter++) {

		iNbrOfNodes = (*EdgeIter)->getNbrOfNodes();
		for(i=0; i < iNbrOfNodes; i++) {
			ConnNode = (*EdgeIter)->getNode(i);
			switch(ConnNode->getLabel()) {
				case 0:	 // Collect hyperedges connected via each node
						 ConnNode->setLabel(iLabel);
						 iNbrOfEdges = ConnNode->getNbrOfEdges();
						 for(j=0; j < iNbrOfEdges; j++)
							if(ConnNode->getEdge(j)->getLabel() == 0) {
								ConnNode->getEdge(j)->setLabel(iLabel);
								Edges->push_back(ConnNode->getEdge(j));
							}
						 break;
				case -1: // Node connects the component with the separator
						 Connector->push_back(ConnNode);
						 ConnNode->setLabel(-2);
						 break;
			}
		}
	}
}


/*
***Description***
The method creates a new hypertree-node, inserts the given hyperedges into the lambda-set,
inserts the nodes of hyperedges labeled by -1 to the chi-set, inserts the nodes in 
ChiConnect to the chi-set, and adds the given hypertrees as subtrees. It is assumed that
hyperedges that should be covered by the chi-set are labeled by -1.

INPUT:	HEdges: Hyperedges to be inserted into the lambda-set
		ChiConnect: Connector nodes that must be a subset of the chi-set
		Subtrees: Subtrees of the new hypertree-node
OUTPUT: return: Labeled hypertree-node
*/

Hypertree *DetKDecomp::getHTNode(Hyperedge **HEdges, Node **ChiConnect, list<Hypertree *> *Subtrees)
{
	int i, j;
	Hypertree *HTree;
	list<Hypertree *>::iterator ListIter;

	// Create a new hypertree-node
	HTree = new Hypertree;
	if(HTree == NULL)
		writeErrorMsg("Error assigning memory.", "DetKDecomp::getHTNode");

	// Insert hyperedges and nodes into the hypertree-node
	for(i=0; HEdges[i] != NULL; i++) {
		HTree->insLambda(HEdges[i]);
		if(HEdges[i]->getLabel() == -1)
			for(j=0; j < HEdges[i]->getNbrOfNodes(); j++)
				HTree->insChi(HEdges[i]->getNode(j));
	}

	if(ChiConnect != NULL)
		// Insert additional chi-labels to guarantee connectedness
		for(i=0; ChiConnect[i] != NULL; i++)
			HTree->insChi(ChiConnect[i]);

	if(Subtrees != NULL)
		// Insert children into the hypertree-node
		for(ListIter=Subtrees->begin(); ListIter != Subtrees->end(); ListIter++)
			HTree->insChild(*ListIter);

	return HTree;
}


/*
***Description***
The method partitions a given set of hyperedges into connected components, i.e., into sets
of hyperedges in the same component and sets of nodes connecting the components with the
separator. It is assumed that separating nodes and hyperedges are labeled by -1 and all
other nodes and hyperedges are labeled by 0.

INPUT:	HEdges: Hyperedges to be partitioned
OUTPUT: Partitions: Components consisting of sets of hyperedges
		Connectors: Sets of nodes connecting each component with the separator
		return: Number of components
*/

int DetKDecomp::separate(Hyperedge **HEdges, Hyperedge ****Partitions, Node ****Connectors)
{
	int iLabel, i, j;
	Hyperedge **Part;
	list<Hyperedge *> Edges;
	list<Hyperedge **> Parts;
	list<Hyperedge *>::iterator EIter1;
	list<Hyperedge **>::iterator EIter2;
	Node **Conn;
	list<Node *> Connector;
	list<Node **> Conns;
	list<Node *>::iterator NIter1;
	list<Node **>::iterator NIter2;

	for(i=0; HEdges[i] != NULL; i++)
		if(HEdges[i]->getLabel() == 0) {
			Edges.clear();
			Connector.clear();
			
			// Search for connected hyperedges
			iLabel = (int)Parts.size()+1;
			collectReachEdges(HEdges[i], iLabel, &Edges, &Connector);

			Part = new Hyperedge*[Edges.size()+1];
			Conn = new Node*[Connector.size()+1];
			if((Part == NULL) || (Conn == NULL))
				writeErrorMsg("Error assigning memory.", "DetKDecomp::separate");

			// Store connected hyperedges in an array
			for(j=0, EIter1=Edges.begin(); EIter1 != Edges.end(); j++, EIter1++)
				Part[j] = *EIter1;
			Part[j] = NULL;

			// Store connecting nodes in an array
			for(j=0, NIter1=Connector.begin(); NIter1 != Connector.end(); j++, NIter1++) {
				Conn[j] = *NIter1;
				Conn[j]->setLabel(-1);
			}
			Conn[j] = NULL;

			Parts.push_back(Part);
			Conns.push_back(Conn);
		}

	*Partitions = new Hyperedge**[Parts.size()+1];
	*Connectors = new Node**[Conns.size()+1];
	if((*Partitions == NULL) || (*Connectors == NULL))
		writeErrorMsg("Error assigning memory.", "DetKDecomp::separate");

	// Store partitions and connectors in arrays
	for(i=0, EIter2=Parts.begin(), NIter2=Conns.begin(); EIter2 != Parts.end(); i++, EIter2++, NIter2++) {
		(*Partitions)[i] = *EIter2;
		(*Connectors)[i] = *NIter2;
	}
	(*Partitions)[i] = NULL;
	(*Connectors)[i] = NULL;

	return i;
}


/*
***Description***
The method orders hyperedges according to maximum cardinality search (MCS) by using the 
number of nodes labeled by -1 as connectivity measure.

INPUT:	HEdges: Hyperedges to be ordered
OUTPUT: HEdges: Hyperedges ordered by MCS
*/

void DetKDecomp::orderMCS(Hyperedge **HEdges, int iNbrOfEdges)
{
	int *iOrder, iMaxCard, iTmpCard, iMaxCardPos, i, j, k;
	Hyperedge *Edge;
	vector<int> Candidates;

	iOrder = new int[iNbrOfEdges];
	if(iOrder == NULL)
		writeErrorMsg("Error assigning memory.", "DetKDecomp::orderMCS");

	// Select hyperedges with highest connectivity iteratively
	for(i=0; i < iNbrOfEdges; i++) {

		// Search for the first edge that has not been selected yet and count its connectivity
		for(j=0; HEdges[j]->getLabel() == -1; j++);
		Edge = HEdges[j];
		for(iMaxCard=k=0; k < Edge->getNbrOfNodes(); k++)
			if(Edge->getNode(k)->getLabel() == -1)
				++iMaxCard;
		Candidates.push_back(j);

		// Search for the edge with highest connectivity, 
		// i.e., with the highest number of nodes contained in edges already selected
		for(++j; j < iNbrOfEdges; j++) {
			Edge = HEdges[j];
			if(Edge->getLabel() != -1) {
				for(iTmpCard=k=0; k < Edge->getNbrOfNodes(); k++)
					if(Edge->getNode(k)->getLabel() == -1)
						++iTmpCard;
				if(iTmpCard >= iMaxCard) {
					if(iTmpCard > iMaxCard) {
						iMaxCard = iTmpCard;
						Candidates.clear();
					}
					Candidates.push_back(j);
				}
			}
		}

		// Randomly select the next edge with highest connectivity
		iMaxCardPos = Candidates[random_range(0, (int)Candidates.size()-1)];
		Candidates.clear();

		// Invalidate the selected edge
		Edge = HEdges[iMaxCardPos];
		Edge->setLabel(-1);
		for(k=0; k < Edge->getNbrOfNodes(); k++)
			Edge->getNode(k)->setLabel(-1);

		// Remember the order of the node removal
		iOrder[iMaxCardPos] = i;
	}

	// Sort variables according to their removal order
	sortPointers((void **)HEdges, iOrder, 0, iNbrOfEdges-1);

	delete [] iOrder;
}


/*
***Description***
The method divides a given set of hyperedges into inner hyperedges and its boundary according
to a given set of boundary nodes. The inner hyperedges are those not containing a boundary node.
All other hyperedges containing a boundary node (within the given set or outside) belong to
the boundary hyperedges.

INPUT:	HEdges: Hyperedges
		Nodes: Boundary nodes
OUTPUT: Inner: Inner hyperedges not containing a boundary node
		Bound: Boundary hyperedges containing a boundary node
		return: Index in Bound that separates boundary hyperedges within and outside the given 
		set of hyperedges; all hyperedges before this index are inside the given set and all
		hyperedges starting at this index are outside the given set.
*/

int DetKDecomp::divideCompEdges(Hyperedge **HEdges, Node **Nodes, Hyperedge ***Inner, Hyperedge ***Bound)
{
	int iNbrOfNodes, iNbrOfEdges, iNbrOfNeighbours, i, j;
	bool bCovered;
	Hyperedge *Edge;
	list<Hyperedge *> InnerB, OuterB;
	list<Hyperedge *>::iterator ListIterI, ListIterO;

	MyHGraph->resetEdgeLabels();
	for(iNbrOfEdges=0; HEdges[iNbrOfEdges] != NULL; iNbrOfEdges++)
		HEdges[iNbrOfEdges]->setLabel(1);

	// Compute the hyperedges containing a boundary node and store them
	// in InnerB if they are contained in the given set and in OuterB otherwise
	for(i=0; Nodes[i] != NULL; i++)
		for(j=0; j < Nodes[i]->getNbrOfEdges(); j++) {
			Edge = Nodes[i]->getEdge(j);
			switch(Edge->getLabel()) {
				case 0:	// Hyperedge is not contained in HEdges
						Edge->setLabel(-1);
						OuterB.push_back(Edge);
						break;
				case 1:	// Hyperedge is contained in HEdges
						Edge->setLabel(-1);
						InnerB.push_back(Edge);
						break;
			}
		}

	// Create a pointer array for the hyperedges not containing a boundary node
	*Inner = new Hyperedge*[(iNbrOfEdges-InnerB.size())+1];
	if(*Inner == NULL)
		writeErrorMsg("Error assigning memory.", "DetKDecomp::divideCompEdges");

	// Store hyperedges not containing a boundary node in the array
	for(i=j=0; i < iNbrOfEdges; i++)
		if(HEdges[i]->getLabel() > 0)
			(*Inner)[j++] = HEdges[i];
	(*Inner)[j] = NULL;

	MyHGraph->resetNodeLabels();
	MyHGraph->resetEdgeLabels();
	for(ListIterO=OuterB.begin(); ListIterO != OuterB.end(); ListIterO++)
		(*ListIterO)->setLabel(1);

	// Remove redundant hyperedges from OuterB; such a hyperedge is redundant if the set of its
	// boundary nodes is covered by some other hyperedge in OuterB
	for(ListIterO=OuterB.begin(); ListIterO != OuterB.end(); ListIterO++) {
		// Label all boundary nodes by 1
		for(i=0; Nodes[i] != NULL; i++)
			Nodes[i]->setLabel(1);

		// Reset the labels of all nodes of the actual hyperedge in OuterB
		iNbrOfNodes = (*ListIterO)->getNbrOfNodes();
		for(i=0; i < iNbrOfNodes; i++)
			(*ListIterO)->getNode(i)->setLabel(0);

		// Check whether some hyperedge in OuterB in the neighbourhood of the actual hyperedge
		// contains no node labeled with 1; in this case it can be removed from OuterB since its 
		// boundery nodes are covered by the actual hyperedge in OuterB
		iNbrOfNeighbours = (*ListIterO)->getNbrOfNeighbours();
		for(i=0; i < iNbrOfNeighbours; i++)
			if((*ListIterO)->getNeighbour(i)->getLabel() != 0) {
				bCovered = true;
				iNbrOfNodes = (*ListIterO)->getNeighbour(i)->getNbrOfNodes();
				for(j=0; j < iNbrOfNodes; j++)
					if((*ListIterO)->getNeighbour(i)->getNode(j)->getLabel() != 0) {
						bCovered = false;
						break;
					}
				if(bCovered)
					OuterB.remove((*ListIterO)->getNeighbour(i));
			}
	}

	// Create a pointer array for the hyperedges containing a boundary node
	*Bound = new Hyperedge*[OuterB.size()+InnerB.size()+1];
	if(*Bound == NULL)
		writeErrorMsg("Error assigning memory.", "DetKDecomp::divideCompEdges");

	// Store hyperedges containing a boundary node in the array
	for(i=0, ListIterI=InnerB.begin(); ListIterI != InnerB.end(); i++, ListIterI++)
		(*Bound)[i] = *ListIterI;
	for(j=i, ListIterO=OuterB.begin(); ListIterO != OuterB.end(); j++, ListIterO++)
		(*Bound)[j] = *ListIterO;
	(*Bound)[j] = NULL;

	return i;
}


/*
***Description***
The method checks whether the actual separator occurs in the MySeps list of already
used separators. If so, the method returns two lists of hyperedges identifiying the 
decomposable and undecomposable partitions. It is assumed that separator hyperedges 
are labeled by -1 and no other hyperedges are labeled by -1.

INPUT:	iSepSize: Size of the actual separator
OUTPUT: Separator: Stored separator
		SuccParts: List of decomposable partitions
		FailParts: List of undecomposable partitions
		return: true if separator was found; otherwise false
*/

bool DetKDecomp::getSepParts(int iSepSize, Hyperedge ***Separator, list<Hyperedge *> **SuccParts, list<Hyperedge *> **FailParts)
{
	int i;
	bool bFound = false;
	list<Hyperedge **>::iterator SepIter;
	list<list<Hyperedge *> *>::iterator SuccPartIter, FailPartIter;

	// Check whether the actual separator is known to be decomposable or undecomposable
	SuccPartIter = MySuccSepParts.begin();
	FailPartIter = MyFailSepParts.begin();
	for(SepIter=MySeps.begin(); SepIter != MySeps.end(); SepIter++) {
		bFound = true;
		for(i=0; (*SepIter)[i] != NULL; i++)
			if((*SepIter)[i]->getLabel() != -1) {
				bFound = false;
				break;
			}

		if(bFound && (i == iSepSize)) {
			*Separator = *SepIter;
			*SuccParts = *SuccPartIter;
			*FailParts = *FailPartIter;
			return true;
		}

		++SuccPartIter;
		++FailPartIter;
	}

	return false;
}


/*
***Description***
The method searches in a list of hyperedges for an edge with a given label.

INPUT:	HEdges: Pointer to a list of hyperedges
		iLabel: Label
OUTPUT: return: true if there exists an edge in HEdges labeled by iLabel; otherwise false
*/

bool DetKDecomp::containsLabel(list<Hyperedge *> *HEdges, int iLabel)
{
	list<Hyperedge *>::iterator ListIter;

	// Search for a hyperedge labeled by iLabel
	for(ListIter=HEdges->begin(); ListIter != HEdges->end(); ListIter++)
		if((*ListIter)->getLabel() == iLabel)
			return true;

	return false;
}


/*
***Description***
The method checks whether the subgraph was split into more than one component such that 
the connector nodes are distributed to different components. It is assumed that all
connector nodes are labeled by -1.

INPUT:	Connector: Parent connector nodes
		ChildConnectors: Connector nodes of the components
OUTPUT: return: true if the parent connector nodes were divided; otherwise false
*/

bool DetKDecomp::isSplitSep(Node **Connector, Node ***ChildConnectors)
{
	int iPConnSize, iConnCtr, i, j;

	// Label all parent connector nodes by 1
	for(iPConnSize=0; Connector[iPConnSize] != NULL; iPConnSize++)
		Connector[iPConnSize]->setLabel(0);

	// Check whether alle connector nodes are contained in a single child connector
	if(iPConnSize > 0)
		for(i=0; ChildConnectors[i] != NULL; i++) {
			iConnCtr = 0;
			for(j=0; ChildConnectors[i][j] != NULL; j++)
				if(ChildConnectors[i][j]->getLabel() == 0)
					++iConnCtr;
			if(iConnCtr >= iPConnSize)
				return false;
		}

	return true;
}


/*
***Description***
The method decomposes the hyperedges in a subhypergraph as described in Gottlob and 
Samer: A Backtracking-Based Algorithm for Computing Hypertree-Decompositions.

INPUT:	HEdges: Hyperedges in the subgraph
		Connector: Connector nodes that must be covered
		iRecLevel: Recursion level
OUTPUT: return: Hypertree decomposition of HEdges
*/

Hypertree *DetKDecomp::decomp(Hyperedge **HEdges, Node **Connector, int iRecLevel)
{
	int *CovSepSet, *CovWeights, iBoundSize, iInnerSize, iNbrSelCov, iAddSize, iCompEnd, i, j;
	int iNbrOfEdges, iAddEdge, iSepSize, iNbrOfNodes, iNbrOfParts, iPartSize;
	bool bFailSep, bAddEdge, bReusedSep, *bInComp, *bCutParts;
	Hyperedge **InnerEdges, **BoundEdges, **Separator, ***Partitions, **AddEdges, *Edge;
	Hypertree *HTree = NULL;
	Node ***ChildConnectors;
	list<Hypertree *> Subtrees;
	list<Hypertree *>::iterator TreeIter;
	list<Hyperedge *> *SuccParts, *FailParts;

	for(iNbrOfEdges=0; HEdges[iNbrOfEdges] != NULL; iNbrOfEdges++);

	// Stop if the hypergraph can be decomposed into two hypertree-nodes
	if((Connector[0] == NULL) && (iNbrOfEdges > 1) && ((int)ceil(iNbrOfEdges/2.0) <= iMyK)) {
		for(i=0; i < iNbrOfEdges; i++)
			HEdges[i]->setLabel(-1);
		i = iNbrOfEdges / 2;
		HTree = getHTNode(&HEdges[i], NULL, NULL);
		HEdges[i] = NULL;
		HTree->insChild(getHTNode(HEdges, NULL, NULL));
		return HTree;
	}

	// Stop if the hypergraph can be decomposed into a single hypertree-node
	if(iNbrOfEdges <= iMyK) {
		for(i=0; i < iNbrOfEdges; i++)
			HEdges[i]->setLabel(-1);
		return getHTNode(HEdges, Connector, NULL);
	}

	// Divide hyperedges into inner hyperedges and hyperedges containing some connecting nodes
	iCompEnd = divideCompEdges(HEdges, Connector, &InnerEdges, &BoundEdges);

	// Count the number of inner and boundary hyperedges
	for(iInnerSize=0; InnerEdges[iInnerSize] != NULL; iInnerSize++);
	for(iBoundSize=0; BoundEdges[iBoundSize] != NULL; iBoundSize++);

	// Create auxiliary arrays
	CovSepSet = new int[iMyK+1];
	bInComp = new bool[iBoundSize];
	CovWeights = new int[iBoundSize];
	AddEdges = new Hyperedge*[iInnerSize+iCompEnd+1];
	if((CovSepSet == NULL) || (bInComp == NULL) || (CovWeights == NULL) || (AddEdges == NULL))
		writeErrorMsg("Error assigning memory.", "DetKDecomp::decomp");

	// Initialize bInComp array
	iAddSize = 0;
	for(i=0; i < iBoundSize; i++)
		i < iCompEnd ? bInComp[i] = true : bInComp[i] = false;

	// Select initial hyperedges to cover the connecting nodes
	iNbrSelCov = setInitSubset(Connector, BoundEdges, CovSepSet, bInComp, CovWeights, iBoundSize);

	// Initialize AddEdges array
	iAddSize = 0;
	for(i=0; i < iBoundSize; i++)
		if(bInComp[i])
			AddEdges[iAddSize++] = BoundEdges[i];
	for(i=0; i < iInnerSize; i++)
		AddEdges[iAddSize++] = InnerEdges[i];
	AddEdges[iAddSize] = NULL;
	if(iAddSize <= 0)
		writeErrorMsg("Illegal number of hyperedges.", "DetKDecomp::decomp");

	if(iNbrSelCov >= 0)
		do {
			// Check whether a covering hyperedge within the component was selected
			bAddEdge = true;
			for(i=0; i < iNbrSelCov; i++)
				if(bInComp[CovSepSet[i]]) {
					bAddEdge = false;
					break;
				}

			// Stop if no inner hyperedge can be in the separator
			if(!bAddEdge || (iMyK-iNbrSelCov > 0)) {

				iAddEdge = 0;
				bAddEdge ? iSepSize = iNbrSelCov+1 : iSepSize = iNbrSelCov;

				do {
					// Output the search progress
					// cout << "(" << iRecLevel << ")"; cout.flush();

					// Set labels of separating nodes and hyperedges to -1
					MyHGraph->resetEdgeLabels();
					MyHGraph->resetNodeLabels();
					for(i=0; i < iNbrSelCov; i++) {
						Edge = BoundEdges[CovSepSet[i]];
						Edge->setLabel(-1);
						iNbrOfNodes = Edge->getNbrOfNodes();
						for(j=0; j < iNbrOfNodes; j++)
							Edge->getNode(j)->setLabel(-1);
					}
					if(bAddEdge) {
						Edge = AddEdges[iAddEdge];
						Edge->setLabel(-1);
						iNbrOfNodes = Edge->getNbrOfNodes();
						for(j=0; j < iNbrOfNodes; j++)
							Edge->getNode(j)->setLabel(-1);
					}

					// Check if selected hyperedges were already used before as separator
					bReusedSep = getSepParts(iSepSize, &Separator, &SuccParts, &FailParts);
					if(!bReusedSep) {
						// Create a separator array and lists for decomposable and undecomposable parts
						Separator = new Hyperedge*[iSepSize+1];
						SuccParts = new list<Hyperedge *>;
						FailParts = new list<Hyperedge *>;
						if((Separator == NULL) || (SuccParts == NULL) || (FailParts == NULL))
							writeErrorMsg("Error assigning memory.", "DetKDecomp::decomp");

						// Store separating hyperedges in the separator array
						for(i=0; i < iNbrSelCov; i++)
							Separator[i] = BoundEdges[CovSepSet[i]];
						if(bAddEdge)
							Separator[i++] = AddEdges[iAddEdge];
						Separator[i] = NULL;

						MySeps.push_back(Separator);
						MySuccSepParts.push_back(SuccParts);
						MyFailSepParts.push_back(FailParts);
					}

					// Separate hyperedges into partitions with corresponding connector nodes
					iNbrOfParts = separate(HEdges, &Partitions, &ChildConnectors);

					// Create auxiliary array
					bCutParts = new bool[iNbrOfParts];
					if(bCutParts == NULL)
						writeErrorMsg("Error assigning memory.", "DetKDecomp::decomp");

					// Check partitions for decomposibility and undecomposibility
					bFailSep = false;
					for(i=0; i < iNbrOfParts; i++) {
						for(iPartSize=0; Partitions[i][iPartSize] != NULL; iPartSize++);
						if(iPartSize >= iNbrOfEdges)
							writeErrorMsg("Monotonicity violated.", "DetKDecomp::decomp");

						// Check for undecomposability
						if(containsLabel(FailParts, Partitions[i][0]->getLabel())) {
							bFailSep = true;
							break;
						}

						// Check for decomposibility
						if(containsLabel(SuccParts, Partitions[i][0]->getLabel()))
							bCutParts[i] = true;
						else
							bCutParts[i] = false;
					}

					if(!bFailSep) {
						// Decompose partitions into hypertrees
						Subtrees.clear();
						for(i=0; i < iNbrOfParts; i++) {
							if(bCutParts[i]) {
								// Prune subtree
								HTree = getHTNode(Partitions[i], ChildConnectors[i], NULL);
								HTree->setCut();
								HTree->setLabel(iRecLevel+1);
							}
							else {
								// Decompose component recursively
								HTree = decomp(Partitions[i], ChildConnectors[i], iRecLevel+1);
								if(HTree == NULL)
									FailParts->push_back(Partitions[i][0]);
								else
									SuccParts->push_back(Partitions[i][0]);
							}
			
							delete [] Partitions[i];
							delete [] ChildConnectors[i];
		
							if(HTree != NULL)
								Subtrees.push_back(HTree);
							else break;
						}

						// Delete remaining partitions and connectors
						for(i++; i < iNbrOfParts; i++) {
							delete [] Partitions[i];
							delete [] ChildConnectors[i];
						}
						delete [] Partitions;
						delete [] ChildConnectors;
						delete [] bCutParts;

						if(HTree == NULL) {
							// Delete previously created subtrees
							for(TreeIter=Subtrees.begin(); TreeIter != Subtrees.end(); TreeIter++)
								delete *TreeIter;
						}
						else {
							// Create a new hypertree node
							for(i=0; i < iNbrSelCov; i++) {
								j = CovSepSet[i];
								bInComp[j] ? BoundEdges[j]->setLabel(-1) : BoundEdges[j]->setLabel(0);
							}
							if(bAddEdge)
								AddEdges[iAddEdge]->setLabel(-1);
							HTree = getHTNode(Separator, Connector, &Subtrees);
						}
					}
					else {
						// Delete partitions and connectors
						for(i=0; i < iNbrOfParts; i++) {
							delete [] Partitions[i];
							delete [] ChildConnectors[i];
						}
						delete [] Partitions;
						delete [] ChildConnectors;
						delete [] bCutParts;
					}
					
				} while(bAddEdge && (HTree == NULL) && (++iAddEdge < iAddSize));
			}

		} while((HTree == NULL) && ((iNbrSelCov = setNextSubset(Connector, BoundEdges, CovSepSet, bInComp, CovWeights, iBoundSize)) > 0));

	delete [] InnerEdges;
	delete [] BoundEdges;
	delete [] AddEdges;
	delete [] CovWeights;
	delete [] bInComp;
	delete [] CovSepSet;

	return HTree;
}


/*
***Description***
The method expands pruned hypertree nodes, i.e., subgraphs which were not decomposed but are
known to be decomposable are decomposed.

INPUT:	HTree: Hypertree that has to be expanded
OUTPUT: HTree: Expanded hypertree
*/

void DetKDecomp::expandHTree(Hypertree *HTree)
{
	int iNbrOfEdges, i;
	Hypertree *CutNode, *Subtree;
	Hyperedge **HEdges, **ParentSep;
	Node **Connector;
	set<Hyperedge *> *Lambda;
	set<Node *> *Chi;
	set<Hyperedge *>::iterator SetIter1;
	set<Node *>::iterator SetIter2;
	
	ParentSep = new Hyperedge*[iMyK+1];
	if(ParentSep == NULL)
		writeErrorMsg("Error assigning memory.", "DetKDecomp::expandHTree");

	while((CutNode = HTree->getCutNode()) != NULL) {

		// Store subgraph in an array
		Lambda = CutNode->getLambda();
		HEdges = new Hyperedge*[Lambda->size()+1];
		if(HEdges == NULL)
			writeErrorMsg("Error assigning memory.", "DetKDecomp::expandHTree");
		for(iNbrOfEdges=0, SetIter1 = Lambda->begin(); SetIter1 != Lambda->end(); iNbrOfEdges++, SetIter1++)
			HEdges[iNbrOfEdges] = *SetIter1;
		HEdges[iNbrOfEdges] = NULL;

		// Store connector nodes in an array
		Chi = CutNode->getChi();
		Connector = new Node*[Chi->size()+1];
		if(Connector == NULL)
			writeErrorMsg("Error assigning memory.", "DetKDecomp::expandHTree");
		for(i=0, SetIter2 = Chi->begin(); SetIter2 != Chi->end(); i++, SetIter2++)
			Connector[i] = *SetIter2;
		Connector[i] = NULL;

		// Reconstruct parent separator
		Lambda = CutNode->getParent()->getLambda();
		for(i=0, SetIter1 = Lambda->begin(); SetIter1 != Lambda->end(); i++, SetIter1++)
			ParentSep[i] = *SetIter1;
		ParentSep[i] = NULL;

		// Decompose subgraph
		Subtree = decomp(HEdges, Connector, CutNode->getLabel());
		if(Subtree == NULL)
			writeErrorMsg("Illegal decomposition pruning.", "H_DetKDecomp::expandHTree");

		// Replace the pruned node by the corresponding subtree
		CutNode->getParent()->insChild(Subtree);
		CutNode->getParent()->remChild(CutNode);
		delete CutNode;

		delete [] HEdges;
		delete [] Connector;
	}

	delete [] ParentSep;
}


/*
***Description***
The method builds a hypertree decomposition of a given hypergraph as described in Gottlob 
and Samer: A Backtracking-Based Algorithm for Computing Hypertree-Decompositions.

INPUT:	HGraph: Hypergraph that has to be decomposed
		iK: Maximum separator size
OUTPUT: return: Hypertree decomposition of HGraph
*/

Hypertree *DetKDecomp::buildHypertree(Hypergraph *HGraph, int iK)
{
	Hypertree *HTree;
	Hyperedge **HEdges;
	Node *Connector[1];
	list<Hyperedge **>::iterator ListIter;
	list<list<Hyperedge *> *>::iterator ListIter1, ListIter2;

	if(iK <= 0)
		writeErrorMsg("Illegal hypertree-width.", "DetKDecomp::buildHypertree");

	MyHGraph = HGraph;
	iMyK = iK;

	// Order hyperedges heuristically
	HGraph->makeDual();
	HEdges = (Hyperedge **)HGraph->getMCSOrder();
	HGraph->makeDual();

	// Store initial heuristic order as weight
	for(int i=0; i < HGraph->getNbrOfEdges(); i++)
		HEdges[i]->setWeight(i);

	// Build hypertree decomposition
	Connector[0] = NULL;
	HTree = decomp(HEdges, Connector, 0);
	delete [] HEdges;

	// Expand pruned hypertree nodes
	if((HTree != NULL) && (HTree->getCutNode() != NULL)) {
		cout << "Expanding hypertree ..." << endl;
		expandHTree(HTree);
	}

	// Free memory
	for(ListIter=MySeps.begin(); ListIter != MySeps.end(); ListIter++)
		delete [] *ListIter;
	MySeps.clear();

	for(ListIter1=MySuccSepParts.begin(), ListIter2=MyFailSepParts.begin(); ListIter1 != MySuccSepParts.end(); ListIter1++, ListIter2++) {
		delete *ListIter1;
		delete *ListIter2;
	}
	MySuccSepParts.clear();
	MyFailSepParts.clear();

	return HTree;
}


