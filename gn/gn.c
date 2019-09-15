#include "gn.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include <stdio.h>
#include "time.h"

#define ERROR_BOUND .0001

void computeGN(NETWORK* network, int initIdx, int endIdx)
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

#if _DEBUG
	printf("Communities: \n");
#endif	
    network->ncomponents = 0;
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
#if _DEBUG
				printf("%02d ", network->vertex[head->vertexIdx].id);
#endif
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

		resetVertices(network);

		if(communityPrinted)
		{
            network->ncomponents++;
#if _DEBUG
			printf("\n");
#endif
			modularity += ((double)comEdgeTotal/(double)(network->nedges)) - (square(comDegreeTotal) / (4*square(network->nedges)));
			comDegreeTotal = 0;
			comEdgeTotal = 0;
		}
	}
#if _DEBUG
	printf("\nModularity: %f\n\n", modularity);
#endif
}

// adds flows to delOrder array. sorts and removes edgeCnt from graph
// resets flowSum for each node
void handleDeletion(NETWORK* network, DEL_ORDER* delOrder)
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
	qsort(delOrder, orderIdx, sizeof(DEL_ORDER), cmpBtwn);

	
	delOrderIdx = -1;
	do{
		delOrderIdx++;
		//remove the edge from each vertex it belongs to
		removeEdge(network, delOrder[delOrderIdx].vertex1Idx,delOrder[delOrderIdx].vertex2Idx);
		removeEdge(network, delOrder[delOrderIdx].vertex2Idx,delOrder[delOrderIdx].vertex1Idx);
		(network->nedges)--;
#if _DEBUG
		printf("%02d <-> %02d = %.2f\n", network->vertex[delOrder[delOrderIdx].vertex1Idx].id,
										 network->vertex[delOrder[delOrderIdx].vertex2Idx].id,
										 delOrder[delOrderIdx].flow);
#endif
	}while(fabs(delOrder[delOrderIdx].flow - delOrder[delOrderIdx+1].flow) < ERROR_BOUND);

#if _DEBUG
	printf("\n----------------------------------------\n");
	// we check the difference above to account for rounding errors with doubles
	// eg 3.99999997 vs 4.0	
	printf("\n");
#endif
}

// removes edges from vertices and shifts edge[] array left accordingly
void removeEdge(NETWORK* network, int vIdx1, int vIdx2)
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



// reset the vertices shortest path and visited variables for the next
// round with a new root node
void resetVertices(NETWORK* network)
{
	int vertexIdx;

	for (vertexIdx = 0; vertexIdx < network->nvertices;vertexIdx++)
	{
		network->vertex[vertexIdx].visited = 0;
		network->vertex[vertexIdx].shortPaths = 0;
	}
}

// after usage of label_header, it is the user's responsibility to free the memory used

void girvan_newman(NETWORK* network, LABELLIST *label_header) {
    DEL_ORDER* delOrder;
    int        delOrderSize; // size of delOrder[]        
    int step = 0;
    delOrderSize = network->nedges;
    delOrder = (DEL_ORDER *)malloc(delOrderSize * sizeof(DEL_ORDER));
    label_header->prev = NULL;
    label_header->labels = calloc(network->nvertices, sizeof(int));
    int ncomponents = get_community_structure(network, label_header->labels);
#if _DEBUG
    printf("Nodes: %d\n", network->nvertices);
    printf("Edges: %d\n\n", network->nedges);
#endif
    while (network->nedges > 0)
    {
#if _DEBUG
        printf("Step %d:\n", step);
        step++;
#endif
        memset(delOrder, 0, delOrderSize * sizeof(DEL_ORDER));
        computeGN(network, 0, network->nvertices);
        if (ncomponents < network->ncomponents) {
            label_header->next = malloc(sizeof(LABELLIST));
            label_header->next->labels = calloc(network->nvertices, sizeof(int));
            (label_header->next)->prev = label_header;
            label_header = label_header->next;
            ncomponents = get_community_structure(network, label_header->labels);
        }
        handleDeletion(network, delOrder);
        
    }
    label_header->next = malloc(sizeof(LABELLIST));
    label_header->next->labels = calloc(network->nvertices, sizeof(int));
    (label_header->next)->prev = label_header;
    label_header = label_header->next;
    ncomponents = get_community_structure(network, label_header->labels);
    label_header->next = NULL;

#if _DEBUG
    printf("Nodes: %d\n", network->nvertices);
    printf("Edges: %d\n\n", network->nedges);
#endif
    free(delOrder);
}

// ncomponents is returned

int get_community_structure(NETWORK* network, int* labels) {
    VERTEXNODE *head,
        *tail,
        *temp;
    int    vertexIdx,
        degreeIdx,
        edgeIdx;
    int label_index = -1;
    int initIdx = 0;
    int endIdx = network->nvertices;    
    int *grouped;
    grouped = calloc(network->nvertices, sizeof(int));
    for (vertexIdx = initIdx; vertexIdx < endIdx; vertexIdx++)
    {
        if (grouped[vertexIdx])
            continue;
        label_index++;
        head = (VERTEXNODE *)malloc(sizeof(VERTEXNODE));

        // init head to root
        head->vertexIdx = vertexIdx;
        // mark root visited and prime depth
        network->vertex[head->vertexIdx].visited = 1;
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

                }
            }
            head = head->next;
        }

        // go back through the list to the root node
        head = tail;

        while (head)
        {
            // find the separate communities. We need to mark the vertex as "grouped"
            // so that we don't print the community as many times as it has members.
            // aka, only print it from one community members perspective

            if (grouped[head->vertexIdx] == 0)
            {
                labels[head->vertexIdx] = label_index;
                grouped[head->vertexIdx] = 1;
            }
       
            temp = head->prev;
            // cleanup on the way out
            free(head);
            head = temp;
        }

    }
    free(grouped);
    resetVertices(network);
    return label_index + 1;
}
