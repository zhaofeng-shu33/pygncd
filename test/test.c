#include "time.h"
#include <stdlib.h>
#include <assert.h>
#include "gn/gn.h"
#include "gn/readgml.h"
void test_complete_routine()
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
void test_get_community_structure() {
    double a[][3] = { {0, 1, 1}, {2, 3, 1}};
    NETWORK* network = NULL;
    construct_network(&network, a, 2, 4);
    int labels[4];
    int ncomponents = get_community_structure(network, labels);
    assert(ncomponents, 2);
    assert(labels[0], 0);
    assert(labels[1], 0);
    assert(labels[2], 1);
    assert(labels[3], 1);
    free_network(network);
}

void test_construct_network() {
    double a[][3] = { {0, 1, 1}, {0, 2, 1}, {1, 2, 5} };
    NETWORK* network = NULL;
    construct_network(&network, a, 3, 3);
    free_network(network);
}

void test_remove_edge_connected_components() {
    double a[][3] = { {0, 1, 1}, {0, 2, 1}, {1, 2, 5} };
    NETWORK* network = NULL;
    construct_network(&network, a, 3, 3);
    int labels[3];
    int ncomponents = get_community_structure(network, labels);
    assert(ncomponents, 1);
    removeEdge(network, 0, 1);
    removeEdge(network, 1, 0);
    removeEdge(network, 0, 2);
    removeEdge(network, 2, 0);
    ncomponents = get_community_structure(network, labels);
    assert(ncomponents, 2);
    removeEdge(network, 1, 2);
    removeEdge(network, 2, 1);
    ncomponents = get_community_structure(network, labels);
    assert(ncomponents, 3);
    free_network(network);
}

int main() {
    test_construct_network();
    test_get_community_structure();
    test_remove_edge_connected_components();
    test_complete_routine();
}