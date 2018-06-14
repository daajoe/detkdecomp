// Library of useful global functions.
//
//////////////////////////////////////////////////////////////////////


#if !defined(CLS_GLOBALS)
#define CLS_GLOBALS

#define REAL double

#include <set>

using namespace std;

class Hypergraph;
class Component;
class CompSet;
class Hyperedge;
class Node;


// Writes an error message to the standard error output stream
void writeErrorMsg(const char *cMessage, const char *cLocation, bool bExitProgram = true);

// Converts an unsigned integer number into a string
char *uitoa(unsigned int iNumber, char *cString);

// Sorts an integer array in non-decreasing order
void sortArray(int *iArray, int iL, int iR);

// Searches in an integer array for a given value
bool searchArray(int *iArray, int iL, int iR, int iKey);

// Sorts an array of pointers in non-decreasing order according to a given int array
void sortPointers(void **Ptr, int *iEval, int iL, int iR);

// Sorts an array of pointers in non-decreasing order according to a given REAL array
void sortPointers(void **Ptr, REAL *rEval, int iL, int iR);

// Checks whether Set1 is a subset of Set2
bool isSubset(set<Node *> *Set1, set<Node *> *Set2);

// Checks whether Set1 is a subset of Set2
bool isSubset(set<Hyperedge *> *Set1, set<Hyperedge *> *Set2);

// Returns a random integer between iLB and iUB
int random_range(int iLB, int iUB);

#endif // !defined(CLS_GLOBALS)

