
DET-K-DECOMP V1.0

G. Gottlob and M. Samer, 2006

Research supported by the Austrian Science Fund (FWF), P17222-N04.


********************************************************************************
- Package

This package consists of the C++ source files in the directory 'sources' and a collection of input files in the directory 'benchmarks' which is further divided into the three subdirectories 'DaimlerChrysler', 'Grid2D', and 'ISCAS89'.


********************************************************************************
- Purpose
 
Hypertree decompositions of hypergraphs are a generalization of tree decompositions of graphs. The corresponding hypertree-width is a measure for the acyclicity and therefore an indicator for the tractability of the associated computation problem. Several NP-hard decision and computation problems are known to be tractable on instances whose structure is represented by hypergraphs of bounded hypertree-width. Roughly speaking, the smaller the hypertree-width, the faster the computation problem can be solved. In this paper, we present the new backtracking-based algorithm det-k-decomp for computing hypertree decompositions of small width. Our benchmark evaluations have shown that det-k-decomp significantly outperforms opt-k-decomp (http://www.deis.unical.it/scarcello/Hypertrees/), the only exact hypertree decomposition algorithm so far. Even compared to the best heuristic algorithm, we obtained competitive results as long as the hypergraphs are sufficiently simple. 
A detailed description of det-k-decomp as well as our evaluation results can be found in [Gottlob and Samer, *].


********************************************************************************
- Compilation

The source code is written in ANSI C++ and should thus be compilable by every ANSI standard compatible C++ compiler like the GNU C++ compiler under Linux or the Microsoft C++ compiler under Windows. For the compilation under Linux (with the GNU C++ compiler installed) change into the directory 'sources' and type:

g++ -o detkdecomp *.cpp

The resulting executable has the name 'detkdecomp' and outputs a usage message by typing 'detkdecomp' without parameters.


********************************************************************************
- Usage: detkdecomp [-def] <k> <filename>

The first optional parameter '-def' tells the program to check the definitions in the optional definition part of the input file. This parameter can always be left out without relevant effect. The second parameter 'k' is a positive integer and defines the upper bound of the hypertree-width (see [Gottlob and Samer, *]). Finally, the third parameter is the path and name of the input file in the usual notation. For example, applying det-k-decomp to the hypergraph in the input file NewSystem1 of the DaimlerChrysler benchmarks with k=3 is done by typing:

detkdecomp 3 benchmarks/DaimlerChrysler/NewSystem1.txt

The input file describes a hypergraph in a very simple format: after an optional definition part, it consists of a sequence of hyperedges of the form HE1(Vi11, Vi12, ..., Vi1m1), HE2(Vi21, Vi22, ..., Vi2m2), ..., HEn(Vin1, Vin2, ..., Vinmn). A more detailed description of this input file format can be found in [Gottlob et al., 2005].
If a hypertree decomposition of width at most k is found, the corresponding hypertree is written into a GML file with the same name as the input file but with the extension '.gml' instead of '.txt'. In our above example, the output file would be benchmarks/DaimlerChrysler/NewSystem1.gml. Graphs described in the GML format (http://infosun.fmi.uni-passau.de/Graphlet/GML/gml-tr.html) can be visualized  for example by the graph drawing tool VGJ (http://www.eng.auburn.edu/department/cse/research/graph_drawing/graph_drawing.html).


********************************************************************************
- References

[Gottlob and Samer, *] Georg Gottlob and Marko Samer. A Backtracking-Based Algorithm for Hypertree-Decomposition. To appear in ACM Journal of Experimental Algorithmics. Preliminary version available as technical report, arXiv:cs/0701083.

[Gottlob et al., 2005] Georg Gottlob, Tobias Ganzow, Nysret Musliu, and Marko Samer. A CSP hypergraph library. Technical report DBAI-TR-2005-50, Institute of Information Systems (DBAI), TU Vienna, 2005.


********************************************************************************
- Note

This program is a prototype implementation and does in no sense claim to be the most efficient way of implementing det-k-decomp. Moreover, several parts of the code have been developed within an implementation framework for evaluating various decomposition algorithms. These parts of the code may therefore be unnecessary or are formulated in a more general way than would be necessary for det-k-decomp.


********************************************************************************
- Contact

For questions and comments please contact:
Marko Samer, E-Mail: samer@dbai.tuwien.ac.at


