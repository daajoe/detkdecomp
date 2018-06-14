// Models set cover algorithms.
//
//////////////////////////////////////////////////////////////////////


#if !defined(CLS_SETCOVER)
#define CLS_SETCOVER


class Node;
class Hyperedge;
class CompSet;
class Hypergraph;

class SetCover  
{
private:
	// Covers a set of nodes by a set of hyperedges
	CompSet *NodeCover1(CompSet *Nodes, CompSet *HEdges, bool bDeterm);

	// Covers a set of nodes by a set of hyperedges
	CompSet *NodeCover2(CompSet *Nodes, CompSet *HEdges, bool bDeterm);

public:
	// Constructor
	SetCover();

	// Destructor
	virtual ~SetCover();

	// Checks whether a set of nodes can be covered by a set of hyperedges
	bool covers(CompSet *Nodes, CompSet *HEdges);

	// Checks whether a set of nodes can be covered by a set of hyperedges
	bool covers(Node **Nodes, Hyperedge **HEdges);

	// Covers a set of nodes by a set of hyperedges
	CompSet *cover(CompSet *Nodes, CompSet *HEdges);
};


#endif // !defined(CLS_SETCOVER)
