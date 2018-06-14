// SetCover.cpp: implementation of the SetCover class.
//
//////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

#include "SetCover.h"
#include "Node.h"
#include "Hyperedge.h"
#include "Hypergraph.h"
#include "CompSet.h"
#include "Globals.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


SetCover::SetCover()
{
}


SetCover::~SetCover()
{
}



//////////////////////////////////////////////////////////////////////
// Class methods
//////////////////////////////////////////////////////////////////////


CompSet *SetCover::NodeCover1(CompSet *Nodes, CompSet *HEdges, bool bDeterm)
{
	int iNbrOfCovEdges, iNbrOfUncovNodes, iMax, iMaxPos, i, j;
	CompSet *CovEdges = NULL;
	Hyperedge *HEdge;
	vector<int> CovCand;

	iNbrOfCovEdges = 0;
	iNbrOfUncovNodes = Nodes->size();

	for(i=0; i < Nodes->size(); i++) {
		for(j=0; j < ((Node *)((*Nodes)[i]))->getNbrOfEdges(); j++)
			((Node *)((*Nodes)[i]))->getEdge(j)->setLabel(-1);
		((*Nodes)[i])->setLabel(0);
	}
	for(i=0; i < HEdges->size(); i++)
		((*HEdges)[i])->setLabel(0);

	// Choose all hyperedges that cover a node which is not contained in any other hyperedge
	for(i=0; i < Nodes->size(); i++) {
		// Check whether there is only one hyperedge that covers the node
		HEdge = NULL;
		for(j=0; j < ((Node *)((*Nodes)[i]))->getNbrOfEdges(); j++)
			if(((Node *)((*Nodes)[i]))->getEdge(j)->getLabel() != -1)
				if(HEdge != NULL) {
					HEdge = NULL;
					break;
				}
				else
					HEdge = ((Node *)((*Nodes)[i]))->getEdge(j);
		
		if((((*Nodes)[i])->getLabel() == 0) && (HEdge != NULL)) {
			++iNbrOfCovEdges;
			HEdge->setLabel(-1);
			
			// Mark all nodes covered by the chosen edge
			for(j=0; j < HEdge->getNbrOfNodes(); j++)
				HEdge->getNode(j)->setLabel(1);
		}
	}

	// Mark covered nodes
	for(i=0; i < Nodes->size(); i++)
		if(((*Nodes)[i])->getLabel() > 0) {
			((*Nodes)[i])->setLabel(-1);
			--iNbrOfUncovNodes;
		}

	// Choose iteratively hyperedges that cover a maximum number of uncovered nodes
	while(iNbrOfUncovNodes > 0) {
		// Label each hyperedge with the number of uncovered nodes it covers
		for(i=0; i < Nodes->size(); i++)
			if(((Node *)((*Nodes)[i]))->getLabel() == 0)
				for(j=0; j < ((Node *)((*Nodes)[i]))->getNbrOfEdges(); j++)
					((Node *)((*Nodes)[i]))->getEdge(j)->incLabel();

		// Search for the hyperedges with the maximum labeling
		iMax = iMaxPos = -1;
		for(i=0; i < HEdges->size(); i++) {
			if(((*HEdges)[i])->getLabel() > iMax) {
				CovCand.clear();
				CovCand.push_back(i);
				iMax = ((*HEdges)[i])->getLabel();
			}
			else
				if(((*HEdges)[i])->getLabel() == iMax)
					CovCand.push_back(i);

			// Reset hyperedge labels
			if(((*HEdges)[i])->getLabel() > 0)
				((*HEdges)[i])->setLabel(0);
		}

		// Select a hyperedge with maximum labeling
		if(bDeterm) {
			iMaxPos = CovCand[0];
			for(i=1; i < (int)CovCand.size(); i++)
				if(((*HEdges)[CovCand[i]])->getID() < ((*HEdges)[iMaxPos])->getID())
					iMaxPos = CovCand[i];
		}
		else
			iMaxPos = CovCand[rand()%CovCand.size()];
		CovCand.clear();

		++iNbrOfCovEdges;
		((*HEdges)[iMaxPos])->setLabel(-1);

		// Mark all nodes covered by the chosen edge
		for(i=0; i < ((Hyperedge *)((*HEdges)[iMaxPos]))->getNbrOfNodes(); i++)
			if(((Hyperedge *)((*HEdges)[iMaxPos]))->getNode(i)->getLabel() == 0)
				((Hyperedge *)((*HEdges)[iMaxPos]))->getNode(i)->setLabel(1);

		// Mark covered nodes
		for(i=0; i < Nodes->size(); i++)
			if(((*Nodes)[i])->getLabel() > 0) {
				((*Nodes)[i])->setLabel(-1);
				--iNbrOfUncovNodes;
			}
	}

	CovEdges = new CompSet(iNbrOfCovEdges);
	if(CovEdges == NULL)
		writeErrorMsg("Error assigning memory.", "NodeCover1");

	// Copy all covering edges into a component set
	for(i=0; i < HEdges->size(); i++)
		if(((*HEdges)[i])->getLabel() < 0)
			CovEdges->insComp((*HEdges)[i]);

	return CovEdges;
}


CompSet *SetCover::NodeCover2(CompSet *Nodes, CompSet *HEdges, bool bDeterm)
{
	int iNbrOfCovEdges, iNbrOfUncovNodes, iMaxPos, iContained, i, j;
	CompSet *CovEdges = NULL;
	double iMax, *NodeWeights, *EdgeWeights;
	Hyperedge *HEdge;
	vector<int> CovCand;

	iNbrOfCovEdges = 0;
	iNbrOfUncovNodes = Nodes->size();

	// Allocate memory for the weights
	NodeWeights = new double[Nodes->size()];
	EdgeWeights = new double[HEdges->size()];
	if((NodeWeights == NULL) || (EdgeWeights == NULL))
		writeErrorMsg("Error assigning memory.", "NodeCover2");

	// Compute the node weights
	for(i=0; i < Nodes->size(); i++) {
		iContained = 0;
		for(j=0; j < ((Node *)((*Nodes)[i]))->getNbrOfEdges(); j++) {
			((Node *)((*Nodes)[i]))->getEdge(j)->setLabel(-1);
			if(HEdges->find(((Node *)((*Nodes)[i]))->getEdge(j)))
				++iContained;
		}
		((*Nodes)[i])->setLabel(0);
		NodeWeights[i] = 1.0 - ((double)iContained / (double)HEdges->size());
	}

	// Label each edge with its index in the EdgeWeights array
	for(i=0; i < HEdges->size(); i++) {
		((*HEdges)[i])->setLabel(i);
		EdgeWeights[i] = 0;
	}

	// Choose all hyperedges that cover a node which is not contained in any other hyperedge
	for(i=0; i < Nodes->size(); i++) {
		// Check whether there is only one hyperedge that covers the node
		HEdge = NULL;
		for(j=0; j < ((Node *)((*Nodes)[i]))->getNbrOfEdges(); j++)
			if(((Node *)((*Nodes)[i]))->getEdge(j)->getLabel() != -1)
				if(HEdge != NULL) {
					HEdge = NULL;
					break;
				}
				else
					HEdge = ((Node *)((*Nodes)[i]))->getEdge(j);
		
		if((((*Nodes)[i])->getLabel() == 0) && (HEdge != NULL)) {
			++iNbrOfCovEdges;
			HEdge->setLabel(-1);
			
			// Mark all nodes covered by the chosen edge
			for(j=0; j < HEdge->getNbrOfNodes(); j++)
				HEdge->getNode(j)->setLabel(1);
		}
	}

	// Reset the node weight of covered nodes
	for(i=0; i < Nodes->size(); i++)
		if(((*Nodes)[i])->getLabel() > 0) {
			NodeWeights[i] = 0;
			((*Nodes)[i])->setLabel(-1);
			--iNbrOfUncovNodes;
		}

	// Compute the hyperedge weights
	for(i=0; i < Nodes->size(); i++)
		if(NodeWeights[i] > 0)
			for(j=0; j < ((Node *)((*Nodes)[i]))->getNbrOfEdges(); j++)
				if(((Node *)((*Nodes)[i]))->getEdge(j)->getLabel() >= 0)
					EdgeWeights[((Node *)((*Nodes)[i]))->getEdge(j)->getLabel()] += NodeWeights[i];

	// Choose iteratively hyperedges that have the highest weight
	while(iNbrOfUncovNodes > 0) {
		// Search for the hyperedge with the highest weight
		for(i=0; ((*HEdges)[i])->getLabel() == -1; i++);
		iMax = EdgeWeights[i];
		CovCand.push_back(i);
		for(; i < HEdges->size(); i++)
			if(EdgeWeights[i] > iMax) {
				CovCand.clear();
				CovCand.push_back(i);
				iMax = EdgeWeights[i];
			}
			else
				if(EdgeWeights[i] == iMax)
					CovCand.push_back(i);

		// Select a hyperedge with maximum labeling
		if(bDeterm) {
			iMaxPos = CovCand[0];
			for(i=1; i < (int)CovCand.size(); i++)
				if(((*HEdges)[CovCand[i]])->getID() < ((*HEdges)[iMaxPos])->getID())
					iMaxPos = CovCand[i];
		}
		else
			iMaxPos = CovCand[rand()%CovCand.size()];
		CovCand.clear();

		++iNbrOfCovEdges;
		EdgeWeights[iMaxPos] = 0;
		((*HEdges)[iMaxPos])->setLabel(-1);

		// Mark all nodes covered by the chosen edge
		for(i=0; i < ((Hyperedge *)((*HEdges)[iMaxPos]))->getNbrOfNodes(); i++)
			if(((Hyperedge *)((*HEdges)[iMaxPos]))->getNode(i)->getLabel() == 0)
				((Hyperedge *)((*HEdges)[iMaxPos]))->getNode(i)->setLabel(1);

		// Reset the node weight of covered nodes and update the hyperedge weights
		for(i=0; i < Nodes->size(); i++)
			if(((*Nodes)[i])->getLabel() > 0) {
				// Update the hyperedge weights
				for(j=0; j < ((Node *)((*Nodes)[i]))->getNbrOfEdges(); j++)
					if(((Node *)((*Nodes)[i]))->getEdge(j)->getLabel() >= 0)
						EdgeWeights[((Node *)((*Nodes)[i]))->getEdge(j)->getLabel()] -= NodeWeights[i];
				NodeWeights[i] = 0;
				((*Nodes)[i])->setLabel(-1);
				--iNbrOfUncovNodes;
			}
	}

	delete [] NodeWeights;
	delete [] EdgeWeights;

	CovEdges = new CompSet(iNbrOfCovEdges);
	if(CovEdges == NULL)
		writeErrorMsg("Error assigning memory.", "NodeCover2");

	// Copy all covering edges into a component set
	for(i=0; i < HEdges->size(); i++)
		if(((*HEdges)[i])->getLabel() == -1)
			CovEdges->insComp((*HEdges)[i]);

	return CovEdges;
}


bool SetCover::covers(CompSet *Nodes, CompSet *HEdges)
{
	int i, j;

	// Reset node labels
	for(i=0; i < Nodes->size(); i++)
		((*Nodes)[i])->setLabel(0);

	// Mark all nodes that can be covered
	for(i=0; i < HEdges->size(); i++)
		for(j=0; j < ((Hyperedge *)((*HEdges)[i]))->getNbrOfNodes(); j++)
			((Hyperedge *)((*HEdges)[i]))->getNode(j)->setLabel(1);

	// Check whether there are unmarked nodes
	for(i=0; i < Nodes->size(); i++)
		if(((*Nodes)[i])->getLabel() == 0)
			return false;

	return true;
}


bool SetCover::covers(Node **Nodes, Hyperedge **HEdges)
{
	int i, j;

	// Reset node labels
	for(i=0; Nodes[i] != NULL; i++)
		Nodes[i]->setLabel(0);

	// Mark all nodes that can be covered
	for(i=0; HEdges[i] != NULL; i++)
		for(j=0; j < HEdges[i]->getNbrOfNodes(); j++)
			HEdges[i]->getNode(j)->setLabel(1);

	// Check whether there are unmarked nodes
	for(i=0; Nodes[i] != NULL; i++)
		if(Nodes[i]->getLabel() == 0)
			return false;

	return true;
}


CompSet *SetCover::cover(CompSet *Nodes, CompSet *HEdges)
{
	CompSet *CovEdges1, *CovEdges2;

	if(!covers(Nodes, HEdges))
		writeErrorMsg("Covering not possible.", "SetCover::cover");

	// Apply set covering
	CovEdges1 = NodeCover1(Nodes, HEdges, true);

	CovEdges2 = NodeCover1(Nodes, HEdges, false);
	if(CovEdges2->size() < CovEdges1->size()) {
		delete CovEdges1;
		CovEdges1 = CovEdges2;
	}
	else
		delete CovEdges2;

	CovEdges2 = NodeCover2(Nodes, HEdges, true);
	if(CovEdges2->size() < CovEdges1->size()) {
		delete CovEdges1;
		CovEdges1 = CovEdges2;
	}
	else
		delete CovEdges2;

	CovEdges2 = NodeCover2(Nodes, HEdges, false);
	if(CovEdges2->size() < CovEdges1->size()) {
		delete CovEdges1;
		CovEdges1 = CovEdges2;
	}
	else
		delete CovEdges2;

	return CovEdges1;
}

