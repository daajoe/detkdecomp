// Models the algorithm det-k-decomp.
//
//////////////////////////////////////////////////////////////////////

#if !defined(CLS_DetKDecomp)
#define CLS_DetKDecomp


class Hypergraph;
class Hyperedge;
class Hypertree;
class Node;
class CompSet;

class DetKDecomp
{
private:
	// Underlying hypergraph
	Hypergraph *MyHGraph;

	// Maximum separator size
	int iMyK;

	// Separator component already checked without success
	list<Hyperedge **> MySeps;

	// Separator component already successfully decomposed
	list<list<Hyperedge *> *> MySuccSepParts;

	// Separator component not decomposable
	list<list<Hyperedge *> *> MyFailSepParts;

	// Initializes a Boolean array representing a subset selection
	int setInitSubset(Node **Nodes, Hyperedge **Edges, int *Set, bool *bInComp, int *CovWeights, int iSize);

	// Selects the next subset in a Boolean array representing a subset selection
	int setNextSubset(Node **Nodes, Hyperedge **Edges, int *Set, bool *bInComp, int *CovWeights, int iSize);

	// Covers a set of nodes by a set of edges
	int coverNodes(Hyperedge **Edges, int *Set, bool *bInComp, int *CovWeights, int iSize, int iUncovered, bool bReconstr);

	// Collects connected hyperedges and the corresponding boundary nodes
	void collectReachEdges(Hyperedge *Edge, int iLabel, list<Hyperedge *> *Edges, list<Node *> *Connector);

	// Creates a hypertree node
	Hypertree *getHTNode(Hyperedge **HEdges, Node **ChiConnect, list<Hypertree *> *Subtrees);

	// Separates a set of hyperedges into partitions with corresponding connecting nodes
	int separate(Hyperedge **HEdges, Hyperedge ****Partitions, Node ****Connectors);

	// Orders hyperedges according to maximum cardinality search
	void orderMCS(Hyperedge **HEdges, int iNbrOfEdges);

	// Divides a set of hyperedges into inner hyperedges and those containing given nodes
	int divideCompEdges(Hyperedge **HEdges, Node **Nodes, Hyperedge ***Inner, Hyperedge ***Bound);

	// Returns the partitions to a given separator that are known to be decomposable or undecomposable
	bool getSepParts(int iSepSize, Hyperedge ***Separator, list<Hyperedge *> **SuccParts, list<Hyperedge *> **FailParts);

	// Checks whether HEdges contains an edge labeled with iLabel
	bool containsLabel(list<Hyperedge *> *HEdges, int iLabel);

	// Checks whether the parent connector nodes are distributed to different components
	bool isSplitSep(Node **Connector, Node ***ChildConnectors);

	// Builds a hypertree decomposition according to k-decomp by covering connector nodes
	Hypertree *decomp(Hyperedge **HEdges, Node **Connector, int iRecLevel);

	// Expands cut hypertree nodes
	void expandHTree(Hypertree *HTree);

public:
	// Constructor
	DetKDecomp();

	// Destructor
	virtual ~DetKDecomp();

	// Constructs a hypertree decomposition of width at most iK (if it exists)
	Hypertree *buildHypertree(Hypergraph *HGraph, int iK);
};


#endif // !defined(CLS_DetKDecomp)

