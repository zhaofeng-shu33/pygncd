#include "time.h"
#include <stdlib.h>
#include "gn/gn.h"
#include "gn/readgml.h"
int main()
{
    NETWORK* network = NULL;
    clock_t    begin, end;
	FILE *fp;

	network  = (NETWORK*) malloc(sizeof(NETWORK));
	fp = fopen("./karate.gml", "r");

	if (fp == NULL)
	{
	  fprintf(stderr, "Can't open input file\n");
	  exit(1);
	}
	// parse file into NETWORK structure
	read_network(network, fp);
    begin = clock();

    girvan_newman(network);

    end = clock();
    printf("\nExecution time: %lf", (double)(end - begin) / CLOCKS_PER_SEC);

	free_network(network);
}