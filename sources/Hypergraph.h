// Models a hypergraph consisting of hyperedges and nodes.
//
//////////////////////////////////////////////////////////////////////


#if !defined(CLS_HYPERGRAPH)
#define CLS_HYPERGRAPH


class Parser;
class Hyperedge;
class Node;
class Hypertree;

class Hypergraph  
{
private:
	// Maximum number of edges contained in the hypergraph
	int iMyMaxNbrOfEdges;

	// Maximum number of nodes contained in the hypergraph
	int iMyMaxNbrOfNodes;

	// Array of pointers to the edges contained in the hypergraph
	Hyperedge **MyEdges;

	// Array of pointers to the nodes contained in the hypergraph
	Node **MyNodes;

	// Labels all edges reachable from Edge
	void labelReachEdges(Hyperedge *Edge);

public:
	// Constructor
	Hypergraph();

	// Destructor
	virtual~Hypergraph();

	// Read hypergraph from file and construct internal representation
	void buildHypergraph(Parser *P);

	// Returns the number of edges in the hypergraph
	int getNbrOfEdges();

	// Returns the number of nodes in the hypergraph
	int getNbrOfNodes();

	// Returns the hyperedge stored at position iPos
	Hyperedge *getEdge(int iPos);

	// Returns the node stored at position iPos
	Node *getNode(int iPos);

	// Returns the hyperedge with ID iID
	Hyperedge *getEdgeByID(int iID);

	// Returns the node with ID iID
	Node *getNodeByID(int iID);

	// Sets labels of all edges to zero
	void resetEdgeLabels(int iVal = 0);

	// Sets labels of all nodes to zero
	void resetNodeLabels(int iVal = 0);

	// Checks whether the hypergraph is connected
	bool isConnected();

	// Transforms the hypergraph into its dual hypergraph
	void makeDual();

	// Removes hyperedges that are covered by another hyperedge
	void reduce(bool bFinalOnly = false);

	// Updates the neighbourhood relation of hyperedges and nodes
	void updateNeighbourhood();

	// Updates the sizes of hyperedges and nodes
	void updateCompSizes();

	// Returns nodes in the same order as they are stored in the hypergraph
	Node **getInputOrder();

	// Returns nodes in the order determined by the minimum induced width (MIW) heuristic
	Node **getMIWOrder();

	// Returns nodes in the order determined by the minimum fill-in (MF) heuristic
	Node **getMFOrder();

	// Returns nodes in the order determined by the maximum cardinality search (MCS) heuristic
	Node **getMCSOrder();

	// Returns nodes in a random order
	Node **getRandomOrder();
};


#endif // !defined(CLS_HYPERGRAPH)

