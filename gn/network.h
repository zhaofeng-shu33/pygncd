// Header file for VERTEX, EDGE, and NETWORK data structures
//
// Mark Newman  11 AUG 06

#ifndef _NETWORK_H
#define _NETWORK_H

typedef struct {
	int vertex1Idx;
	int vertex2Idx;
	double flow;
}DEL_ORDER;

int cmpBtwn(DEL_ORDER *e1, DEL_ORDER *e2);

double square (double x); //faster than pow() for doubles

typedef struct {
  int target;        // Index in the vertex[] array of neighboring vertex.
                     // (Note that this is not necessarily equal to the GML
                     // ID of the neighbor if IDs are nonconsecutive or do
                     // not start at zero.)
  double weight;     // Weight of edge.  1 if no weight is specified.
  double flow;		 // The flow to be sent through the edge.
  double flowSum;    // The total flow from all BFS permutations.
} EDGE;

typedef struct {
  int id;            // GML ID number of vertex
  int degree;        // Degree of vertex (out-degree for directed nets)
  int shortPaths;	 // Number of shortest paths to the node from root node
  int bfsLevel;	     // The depth of the vertex from the root
  int visited;	     // 1 if visited, 0 otherwise
  double flowAvail;  // flow available to send up
  char *label;       // GML label of vertex.  NULL if no label specified
  EDGE *edge;        // Array of EDGE structs, one for each neighbor
  int grouped;		 // indicates if a node has been placed in a community yet
} VERTEX;

typedef struct {
  int nvertices;     // Number of vertices in network
  int ncomponents;   // Number of connected components in network
  int nedges;        // Number of edges in network
  int directed;      // 1 = directed network, 0 = undirected
  VERTEX *vertex;    // Array of VERTEX structs, one for each vertex
} NETWORK;

void construct_network(NETWORK** network_ptr, double array_list[][3], int num_edges, int num_vertices);

typedef struct VERTEXNODE
{
	int vertexIdx;
	struct VERTEXNODE *next;
	struct VERTEXNODE *prev;
} VERTEXNODE;

typedef struct LABELLIST
{
    int* labels;
    struct LABELLIST* next;
    struct LABELLIST* prev;
} LABELLIST;
#endif
