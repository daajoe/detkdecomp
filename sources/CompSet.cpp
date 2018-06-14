// CompSet.cpp: implementation of the CompSet class.
//
//////////////////////////////////////////////////////////////////////

#include <cstdlib>

#include "CompSet.h"
#include "Component.h"
#include "Globals.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CompSet::CompSet(int iNbrOfComps, bool bSpecial)
{
	// Create pointer array
	MyComponents = new Component*[iNbrOfComps];
	if(MyComponents == NULL)
		writeErrorMsg("Error assigning memory.", "CompSet::CompSet");
	iMyMaxNbrOfComps = iNbrOfComps;

	// Initialize pointers in the array
	iMyNbrOfComps = 0;
	for(int i=0; i < iMyMaxNbrOfComps; i++)
		MyComponents[i] = NULL;

	bMySpecial = bSpecial;
}


CompSet::~CompSet()
{
	delete [] MyComponents;
}



//////////////////////////////////////////////////////////////////////
// Class methods
//////////////////////////////////////////////////////////////////////


bool CompSet::insComp(Component *Comp)
{
	// Check whether Comp is already contained (multiple occurrences are not allowed)
	for(int i=0; i < iMyNbrOfComps; i++)
		if(MyComponents[i] == Comp)
			return false;

	if(iMyNbrOfComps >= iMyMaxNbrOfComps)
		writeErrorMsg("Set size exceeded.", "CompSet::insComp");

	// Insert the pointer
	MyComponents[iMyNbrOfComps++] = Comp;
	return true;
}


void CompSet::remComp(int iPos)
{
	if(iPos >= iMyNbrOfComps)
		writeErrorMsg("Position not available.", "CompSet::remEdge");

	// Remove pointer
	--iMyNbrOfComps;
	for(int i=iPos; i < iMyNbrOfComps; i++)
		MyComponents[i] = MyComponents[i+1];
	MyComponents[iMyNbrOfComps] = NULL;
}


bool CompSet::remComp(Component *Comp)
{
	int i;
	bool bFound = false;

	for(i=0; i < iMyNbrOfComps; i++)
		if(MyComponents[i] == Comp) {
			bFound = true;
			break;
		}

	if(bFound) {
		--iMyNbrOfComps;
		for(; i < iMyNbrOfComps; i++)
			MyComponents[i] = MyComponents[i+1];
		MyComponents[iMyNbrOfComps] = NULL;
	}

	return bFound;
}


int CompSet::size()
{
	return iMyNbrOfComps;
}


bool CompSet::empty()
{
	return iMyNbrOfComps == 0;
}


void CompSet::clear()
{
	iMyNbrOfComps = 0;
	for(int i=0; i < iMyMaxNbrOfComps; i++)
		MyComponents[i] = NULL;

}


bool CompSet::find(Component *Comp)
{
	int i;

	for(i=0; i < iMyNbrOfComps; i++)
		if(MyComponents[i] == Comp)
			return true;

	return false;
}


Component *CompSet::operator[](int iPos)
{
	if(iPos >= iMyNbrOfComps)
		return NULL;

	return MyComponents[iPos];
}


bool CompSet::isSpecial()
{
	return bMySpecial;
}


void CompSet::sortByLabels()
{
	int *iOrder, i;

	if(iMyNbrOfComps > 1) {
		iOrder = new int[iMyNbrOfComps];
		if(iOrder == NULL)
			writeErrorMsg("Error assigning memory.", "CompSet::sortByLabels");

		for(i=0; i < iMyNbrOfComps; i++)
			iOrder[i] = MyComponents[i]->getLabel();
		sortPointers((void **)MyComponents, iOrder, 0, iMyNbrOfComps-1);
		delete [] iOrder;
	}
}


bool CompSet::isSubset(CompSet *Set)
{
	int i;

	// Set labels of all nodes in the actual set to 0
	for(i=0; i < iMyNbrOfComps; i++)
		MyComponents[i]->setLabel(0);
	// Set labels of all nodes in Set to 1
	for(i=0; i < Set->iMyNbrOfComps; i++)
		Set->MyComponents[i]->setLabel(1);

	// Check whether all node labels in the actual set are 1; if so, the actual set is a subset of Set
	for(i=0; i < iMyNbrOfComps; i++)
		if(MyComponents[i]->getLabel() == 0)
			return false;

	return true;
}


bool CompSet::resize(int iSize)
{
	Component **OldComponents;

	if(iSize < iMyNbrOfComps)
		return false;

	// Create new array
	OldComponents = MyComponents;
	MyComponents = new Component*[iSize];
	if(MyComponents == NULL)
		writeErrorMsg("Error assigning memory.", "CompSet::resize");
	iMyMaxNbrOfComps = iSize;

	// Copy content into the new array
	for(int i=0; i < iMyNbrOfComps; i++)
		MyComponents[i] = OldComponents[i];
	delete [] OldComponents;

	return true;
}

