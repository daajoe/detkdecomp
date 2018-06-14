// Globals.cpp: implementation of global functions.
//
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdlib>

using namespace std;


#include "Globals.h"
#include "Hypergraph.h"
#include "CompSet.h"
#include "Hyperedge.h"
#include "Node.h"



void writeErrorMsg(const char *cMessage, const char *cLocation, bool bExitProgram)
{ 
	if(bExitProgram) {
		cerr << "ERROR in " << cLocation << ": " << cMessage << endl; 
		exit(EXIT_FAILURE);
	}
	else
		cerr << "WARNING in " << cLocation << ": " << cMessage << endl;
}


char *uitoa(unsigned int iNumber, char *cString)
{
	int iPos = 0;
	char cTmp;

	// Compute the string representation in reverse order
	do {
		cString[iPos++] = (char)((iNumber%10) + 48);
		iNumber /= 10;
	} while (iNumber != 0);
	cString[iPos] = '\0';

	// Invert the order of the digits
	for(int i=0, j=iPos-1; i < iPos/2; i++, j--) {
		cTmp = cString[i];
		cString[i] = cString[j];
		cString[j] = cTmp;
	}

	return cString;
}


void sortArray(int *iArray, int iL, int iR)
{
	int i = iL-1, j = iR, iTmp;

	if(iR-iL > 200) {   // Quicksort
		while(true) {
			while(iArray[++i] < iArray[iR]);
			while(iArray[--j] > iArray[iR]);
			if(i >= j) break;
			
			// Swap array entries
			iTmp = iArray[i];
			iArray[i] = iArray[j];
			iArray[j] = iTmp;
		}

		// Swap array entries
		iTmp = iArray[i];
		iArray[i] = iArray[iR];
		iArray[iR] = iTmp;

		sortArray(iArray, iL, i-1);
		sortArray(iArray, i+1, iR);
	}
	else {  // Insertion sort
		for(i=iL+1; i <= iR; i++) {
			iTmp = iArray[i];
			for(j=i-1; (j >= iL) && (iTmp < iArray[j]); j--)
				iArray[j+1] = iArray[j];
			iArray[j+1] = iTmp;
		}
	}
}


bool searchArray(int *iArray, int iL, int iR, int iKey)
{
	int i;

	while(iL != iR) {
		i = (iL+iR)/2;
		iKey <= iArray[i] ? iR = i : iL = i+1;
	}
	if(iKey == iArray[iL])
		return true;

	return false;
}


void sortPointers(void **Ptr, int *iEval, int iL, int iR)
{
	int i = iL-1, j = iR;
	void *pTmp;
	int iTmp;

	if(iR-iL > 200) {  // Quicksort
		while(true) {
			while(iEval[++i] < iEval[iR]);
			while(iEval[--j] > iEval[iR]);
			if(i >= j) break;
			
			// Swap valuation entries
			iTmp = iEval[i];
			iEval[i] = iEval[j];
			iEval[j] = iTmp;

			// Swap pointers
			pTmp = Ptr[i];
			Ptr[i] = Ptr[j];
			Ptr[j] = pTmp;
		}

		// Swap valuation entries
		iTmp = iEval[i];
		iEval[i] = iEval[iR];
		iEval[iR] = iTmp;

		// Swap pointers
		pTmp = Ptr[i];
		Ptr[i] = Ptr[iR];
		Ptr[iR] = pTmp;

		sortPointers(Ptr, iEval, iL, i-1);
		sortPointers(Ptr, iEval, i+1, iR);
	}
	else {  // Insertion sort
		for(i=iL+1; i <= iR; i++) {
			iTmp = iEval[i];
			pTmp = Ptr[i];
			for(j=i-1; (j >= iL) && (iTmp < iEval[j]); j--) {
				iEval[j+1] = iEval[j];
				Ptr[j+1] = Ptr[j];
			}
			iEval[j+1] = iTmp;
			Ptr[j+1] = pTmp;
		}
	}
}


void sortPointers(void **Ptr, REAL *rEval, int iL, int iR)
{
	int i = iL-1, j = iR;
	void *pTmp;
	REAL rTmp;

	if(iR-iL > 200) {  // Quicksort
		while(true) {
			while(rEval[++i] < rEval[iR]);
			while(rEval[--j] > rEval[iR]);
			if(i >= j) break;
			
			// Swap valuation entries
			rTmp = rEval[i];
			rEval[i] = rEval[j];
			rEval[j] = rTmp;

			// Swap pointers
			pTmp = Ptr[i];
			Ptr[i] = Ptr[j];
			Ptr[j] = pTmp;
		}

		// Swap valuation entries
		rTmp = rEval[i];
		rEval[i] = rEval[iR];
		rEval[iR] = rTmp;

		// Swap pointers
		pTmp = Ptr[i];
		Ptr[i] = Ptr[iR];
		Ptr[iR] = pTmp;

		sortPointers(Ptr, rEval, iL, i-1);
		sortPointers(Ptr, rEval, i+1, iR);
	}
	else {  // Insertion sort
		for(i=iL+1; i <= iR; i++) {
			rTmp = rEval[i];
			pTmp = Ptr[i];
			for(j=i-1; (j >= iL) && (rTmp < rEval[j]); j--) {
				rEval[j+1] = rEval[j];
				Ptr[j+1] = Ptr[j];
			}
			rEval[j+1] = rTmp;
			Ptr[j+1] = pTmp;
		}
	}
}


bool isSubset(set<Node *> *Set1, set<Node *> *Set2)
{	
	set<Node *>::iterator SetIter;

	// Set labels of all nodes in Set1 to 0
	for(SetIter=Set1->begin(); SetIter != Set1->end(); SetIter++)
		(*SetIter)->setLabel(0);
	// Set labels of all nodes in Set2 to 1
	for(SetIter=Set2->begin(); SetIter != Set2->end(); SetIter++)
		(*SetIter)->setLabel(1);

	// Check whether all node labels in Set1 are 1; if so, Set1 is a subset of Set2
	for(SetIter=Set1->begin(); SetIter != Set1->end(); SetIter++)
		if((*SetIter)->getLabel() == 0)
			return false;

	return true;
}


bool isSubset(set<Hyperedge *> *Set1, set<Hyperedge *> *Set2)
{	
	set<Hyperedge *>::iterator SetIter;

	// Set labels of all nodes in Set1 to 0
	for(SetIter=Set1->begin(); SetIter != Set1->end(); SetIter++)
		(*SetIter)->setLabel(0);
	// Set labels of all nodes in Set2 to 1
	for(SetIter=Set2->begin(); SetIter != Set2->end(); SetIter++)
		(*SetIter)->setLabel(1);

	// Check whether all node labels in Set1 are 1; if so, Set1 is a subset of Set2
	for(SetIter=Set1->begin(); SetIter != Set1->end(); SetIter++)
		if((*SetIter)->getLabel() == 0)
			return false;

	return true;
}


int random_range(int iLB, int iUB)
{
	int iRange;

	if(iLB > iUB)
		writeErrorMsg("Lower bound larger than upper bound.", "random_range");

	iRange = (iUB - iLB) + 1;
	return iLB + (int)(iRange * (rand() / (RAND_MAX + 1.0)));
}

