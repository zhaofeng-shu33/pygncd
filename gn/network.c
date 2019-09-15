#include <stdlib.h>
#include "gn/network.h"

// compare edge flows highest to lowest
// return 1 when e1 should be after e2
// return -1 when e1 should be before e2
int cmpBtwn(DEL_ORDER *e1, DEL_ORDER *e2)
{
    if (e1->flow < e2->flow) return 1;
    if (e1->flow > e2->flow) return -1;
    return 0;
}

// squares a double
double square(double x)
{
    return (x * x);
}


void construct_network(NETWORK** network_ptr, double array_list[][3], int num_edges, int num_vertices) {
    NETWORK* network = (NETWORK*)malloc(sizeof(NETWORK));
    network->nvertices = num_vertices;
    network->nedges = num_edges;
    network->vertex = calloc(network->nvertices, sizeof(VERTEX));
    int i;
    for (i = 0; i < network->nedges; i++) {
        int s = (int)array_list[i][0];
        int t = (int)array_list[i][1];
        network->vertex[s].degree++;
        network->vertex[t].degree++;
    }
    for (i = 0; i < network->nvertices; i++) {
        network->vertex[i].id = i;
        network->vertex[i].flowAvail = 0;
        network->vertex[i].edge = malloc(network->vertex[i].degree * sizeof(EDGE));
    }
    int *count;
    count = calloc(network->nvertices, sizeof(int));
    for (i = 0; i < network->nedges; i++) {
        int s = (int)array_list[i][0];
        int t = (int)array_list[i][1];
        double w = array_list[i][2];
        network->vertex[s].edge[count[s]].target = t;
        network->vertex[s].edge[count[s]].flow = 0;
        network->vertex[s].edge[count[s]].flowSum = 0;
        network->vertex[s].edge[count[s]].weight = w;
        count[s]++;
        network->vertex[t].edge[count[t]].target = s;
        network->vertex[t].edge[count[t]].flow = 0;
        network->vertex[t].edge[count[t]].flowSum = 0;
        network->vertex[t].edge[count[t]].weight = w;
        count[t]++;
    }
    free(count);
    *network_ptr = network;
}
