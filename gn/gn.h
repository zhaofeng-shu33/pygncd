#ifndef _GN_H
#define _GN_H
#include "gn/network.h"
void computeGN(NETWORK* network, int initIdx, int endIdx);
void handleDeletion(NETWORK* network, DEL_ORDER* delOrder);
void removeEdge(NETWORK* network, int vIdx1, int vIdx2);
void resetVertices(NETWORK* network);
void girvan_newman(NETWORK* network, LABELLIST *label_header);
int get_community_structure(NETWORK* network, int* labels);
#endif