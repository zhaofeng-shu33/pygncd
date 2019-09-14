#ifndef _GN_H
#define _GN_H
#include "gn/network.h"
void computeGN(NETWORK* network, DEL_ORDER* delOrder, int initIdx, int endIdx, int delOrderSize);
void handleDeletion(NETWORK* network, DEL_ORDER* delOrder, int* delOrderSize);
void removeEdge(NETWORK* network, int vIdx1, int vIdx2);
void resetVertices(NETWORK* network);
#endif