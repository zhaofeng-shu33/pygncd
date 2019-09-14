#include "time.h"

#include "gn/gn.h"
#include "gn/readgml.h"
int main()
{
    DEL_ORDER* delOrder;
    NETWORK* network = NULL;
    clock_t    begin, end;
    int        delOrderSize = 0, // size of delOrder[]        
        step = 1; 	 	 // steps of the GN alg
	FILE *fp;
	int  vertexIdx;

	network  = (NETWORK*) malloc(sizeof(NETWORK));
	fp = fopen("./karate.gml", "r");

	if (fp == NULL)
	{
	  fprintf(stderr, "Can't open input file\n");
	  exit(1);
	}
    begin = clock();
	// parse file into NETWORK structure
	read_network(network, fp);

	delOrderSize = network->nedges;
	delOrder = (DEL_ORDER *) malloc(delOrderSize * sizeof(DEL_ORDER));

	printf("Nodes: %d\n", network->nvertices);
	printf("Edges: %d\n\n", network->nedges);

	while(network->nedges > 0)
	{
		computeGN(network, delOrder, 0, network->nvertices, delOrderSize);
		handleDeletion(network, delOrder, &delOrderSize);
	}

	printf("Nodes: %d\n", network->nvertices);
	printf("Edges: %d\n\n", network->nedges);

	end = clock();
	printf("\nExecution time: %lf", (double)(end - begin) / CLOCKS_PER_SEC);
	free(delOrder);
	free_network(network);
}