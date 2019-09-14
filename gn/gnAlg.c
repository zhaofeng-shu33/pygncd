#include "readgml.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "time.h"

#define ERROR_BOUND .0001

DEL_ORDER  *delOrder;
NETWORK    *network = NULL;
clock_t    begin,end;
int        delOrderSize = 0, // size of delOrder[]
		   totalEdges = 0, 	 // total edges
		   edgeCnt = 0, 	 // counter for edges
		   step = 1; 	 	 // steps of the GN alg

int main()
{
	FILE *fp;
	int  vertexIdx;

	network  = (NETWORK*) malloc(sizeof(NETWORK));
	fp = fopen("./karate.gml", "r");

	if (fp == NULL)
	{
	  fprintf(stderr, "Can't open input file\n");
	  exit(1);
	}

	// parse file into NETWORK structure
	read_network(network,fp);

	// compute number of edgeCnt
	for (vertexIdx = 0; vertexIdx < network->nvertices;vertexIdx++)
	{
		edgeCnt += network->vertex[vertexIdx].degree;
	}

	delOrderSize = edgeCnt;
	delOrder = (DEL_ORDER *) malloc(delOrderSize * sizeof(DEL_ORDER));
	edgeCnt /= 2;
	totalEdges = edgeCnt;

	printf("Nodes: %d\n", network->nvertices);
	printf("Edges: %d\n\n", totalEdges);

	while(edgeCnt > 0)
	{
		computeGN(0,network->nvertices);
		handleDeletion();
	}

	printf("Nodes: %d\n", network->nvertices);
	printf("Edges: %d\n\n", totalEdges);

	end = clock();
	printf("\nExecution time: %lf", (double)(end - begin) / CLOCKS_PER_SEC);
	free(delOrder);
	free_network(network);
}

void computeGN(int initIdx, int endIdx)
{
	VERTEXNODE *head,
			   *tail,
			   *temp;
	double     modularity = 0;
	int 	   communityPrinted,
	   	   	   comDegreeTotal = 0,  // total degree  of nodes in community
	   	   	   comEdgeTotal = 0, // total edges in the community
			   vertexIdx,
	           degreeIdx,
			   edgeIdx;


	printf("Communities: \n");
	memset(delOrder, 0, delOrderSize*sizeof(*delOrder));

	for (vertexIdx = initIdx; vertexIdx < endIdx; vertexIdx++)
	{
		head = (VERTEXNODE *) malloc(sizeof(VERTEXNODE));
		communityPrinted = 0;

		// init head to root
		head->vertexIdx = vertexIdx;
		// mark root visited and prime depth
		network->vertex[head->vertexIdx].visited = 1;
		network->vertex[head->vertexIdx].bfsLevel = 0;
		head->prev = NULL;
		tail = head;
		// prevents corruption if head-next is valid when it
		// shouldn't be
		head->next = NULL;

		while (head)
		{
			for (degreeIdx = 0; degreeIdx < network->vertex[head->vertexIdx].degree; degreeIdx++)
			{
				edgeIdx = network->vertex[head->vertexIdx].edge[degreeIdx].target;

				// add nodes not visited to list
				if (0 == network->vertex[edgeIdx].visited)
				{
					tail->next = malloc(sizeof(VERTEXNODE));
					(tail->next)->prev = tail;
					tail = tail->next;
					tail->next = NULL;
					tail->vertexIdx = edgeIdx;

					// mark visited
					network->vertex[tail->vertexIdx].visited = 1;
					network->vertex[tail->vertexIdx].flowAvail = 1;

					// compute BFS value
					network->vertex[tail->vertexIdx].bfsLevel = network->vertex[head->vertexIdx].bfsLevel + 1;

					if(1 == network->vertex[tail->vertexIdx].bfsLevel)
					{
						network->vertex[tail->vertexIdx].shortPaths = 1;
					}
				}
				// compute shortest path
				if (network->vertex[head->vertexIdx].bfsLevel > network->vertex[edgeIdx].bfsLevel)
				{
					network->vertex[head->vertexIdx].shortPaths += network->vertex[edgeIdx].shortPaths;
				}
			}
			head = head->next;
		}

		// go back through the list to the root node
		head = tail;

		while(head)
		{
			// print the separate communities. We need to mark the vertex as "grouped"
			// so that we don't print the community as many times as it has members.
			// aka, only print it from one community members perspective

			if (network->vertex[head->vertexIdx].grouped == 0)
			{
				printf("%02d ", network->vertex[head->vertexIdx].id);
				network->vertex[head->vertexIdx].grouped = 1;
				comDegreeTotal += network->vertex[head->vertexIdx].degree;
				comEdgeTotal = comDegreeTotal/2;
				communityPrinted = 1;
			}

			for (degreeIdx = 0; degreeIdx < network->vertex[head->vertexIdx].degree; degreeIdx++)
			{
				edgeIdx = network->vertex[head->vertexIdx].edge[degreeIdx].target;
				// get the ratio of flow that should be sent to each node that has BFS level lower than
				// current nodes BFS (flow should only go up, not sideways or down)
				 if(network->vertex[edgeIdx].bfsLevel < network->vertex[head->vertexIdx].bfsLevel)
				{
					// the flow of the edge is the flow available from the lower node times the ratio of shortest paths
					network->vertex[head->vertexIdx].edge[degreeIdx].flow = ((network->vertex[head->vertexIdx].flowAvail) *
																	        ((double)(network->vertex[edgeIdx].shortPaths) /
																	        ((double)network->vertex[head->vertexIdx].shortPaths)));

					if (0 == network->vertex[edgeIdx].bfsLevel)
					{
						// our edge (with lower BFS) is the root, send all flow available
						network->vertex[head->vertexIdx].edge[degreeIdx].flow = network->vertex[head->vertexIdx].flowAvail;
					}
					else
					{
						// our edge is not the root, calculate how much flow we are able to send after the additional flow is
						// received from the edge[degreeIdx]
						network->vertex[edgeIdx].flowAvail += network->vertex[head->vertexIdx].edge[degreeIdx].flow;
					}
					network->vertex[head->vertexIdx].edge[degreeIdx].flowSum += (network->vertex[head->vertexIdx].edge[degreeIdx].flow);
				}
			}

			temp = head->prev;
			// cleanup on the way out
			free(head);
			head = temp;
		}

		resetVertices();

		if(communityPrinted)
		{
			printf("\n");
			modularity += ((double)comEdgeTotal/(double)(totalEdges)) - (square(comDegreeTotal) / (4*square(totalEdges)));
			comDegreeTotal = 0;
			comEdgeTotal = 0;
		}
	}
	printf("\nModularity: %f\n\n", modularity);
}

// adds flows to delOrder array. sorts and removes edgeCnt from graph
// resets flowSum for each node
void handleDeletion()
{
	int delOrderIdx,
		vertexIdx,
	    degreeIdx,
		orderIdx = 0,
		edgeIdx;

	for (vertexIdx = 0; vertexIdx < network->nvertices;vertexIdx++)
	{
		network->vertex[vertexIdx].grouped = 0;
		for (degreeIdx = 0; degreeIdx < network->vertex[vertexIdx].degree; degreeIdx++)
		{
			edgeIdx = network->vertex[vertexIdx].edge[degreeIdx].target;
			// realloc size of delOrder if need be...double it to be safe...realloc is expensive...
			if ((orderIdx + 1) > delOrderSize)
			{
				delOrderSize = delOrderSize*2;
				delOrder = (DEL_ORDER *) realloc(delOrder, (delOrderSize*2) * sizeof(DEL_ORDER));
			}

			// only add the edge in 1 direction to avoid duplicates. this 'if' checks if we have already visited
			// one of the vertices corresponding to the same edge. this could also be > if we wanted. need to simply
			// pick one
			if(edgeIdx < vertexIdx)
			{
				delOrder[orderIdx].flow 	       = network->vertex[vertexIdx].edge[degreeIdx].flowSum;
				delOrder[orderIdx].vertex1Idx      = vertexIdx;
				delOrder[orderIdx++].vertex2Idx    = edgeIdx;
				network->vertex[vertexIdx].edge[degreeIdx].flowSum = 0;
			}
		}
	}

	// sort the edgeCnt according to cmpBtwn: highest to lowest
	qsort(delOrder,(orderIdx),sizeof(DEL_ORDER),(void*)cmpBtwn);

	printf("Step %d:\n", step);
	delOrderIdx = -1;
	do{
		delOrderIdx++;
		//remove the edge from each vertex it belongs to
		removeEdge(delOrder[delOrderIdx].vertex1Idx,delOrder[delOrderIdx].vertex2Idx);
		removeEdge(delOrder[delOrderIdx].vertex2Idx,delOrder[delOrderIdx].vertex1Idx);
		edgeCnt--;

		printf("%02d <-> %02d = %.2f\n", network->vertex[delOrder[delOrderIdx].vertex1Idx].id,
										 network->vertex[delOrder[delOrderIdx].vertex2Idx].id,
										 delOrder[delOrderIdx].flow);
	}while(fabs(delOrder[delOrderIdx].flow - delOrder[delOrderIdx+1].flow) < ERROR_BOUND);

	printf("\n----------------------------------------\n");
	// we check the difference above to account for rounding errors with doubles
	// eg 3.99999997 vs 4.0

	step++;
	printf("\n");
}

// removes edges from vertices and shifts edge[] array left accordingly
void removeEdge(int vIdx1, int vIdx2)
{
	int degreeShift,
		degreeIdx;

	for (degreeIdx = 0; degreeIdx < network->vertex[vIdx1].degree; degreeIdx++)
	{
		if ((vIdx2) == (network->vertex[vIdx1].edge[degreeIdx].target))
		{
			// we found vIdx2 in vIdx2 target array. shift everything in target array down/left by 1
			// and decrement the degree since we have removed an edge
			for (degreeShift = degreeIdx; degreeShift < network->vertex[vIdx1].degree; degreeShift++)
			{
				network->vertex[vIdx1].edge[degreeShift] =
						network->vertex[vIdx1].edge[degreeShift+1];
			}
			network->vertex[vIdx1].degree--;
		}
	}
}

// compare edge flows highest to lowest
// return 1 when e1 should be after e2
// return -1 when e1 should be before e2
int cmpBtwn(DEL_ORDER *e1, DEL_ORDER *e2)
{
	if (e1->flow < e2->flow) return 1;
	if (e1->flow > e2->flow) return -1;
	return 0;
}

// reset the vertices shortest path and visited variables for the next
// round with a new root node
void resetVertices()
{
	int vertexIdx;

	for (vertexIdx = 0; vertexIdx < network->nvertices;vertexIdx++)
	{
		network->vertex[vertexIdx].visited = 0;
		network->vertex[vertexIdx].shortPaths = 0;
	}
}

// squares a double
double square (double x)
{
	return (x * x);
}
