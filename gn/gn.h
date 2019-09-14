#ifndef _GN_H
#define _GN_H
#include "gn/network.h"
void computeGN(NETWORK* network, DEL_ORDER* delOrder, int initIdx, int endIdx, int delOrderSize, int totalEdges);
void handleDeletion(NETWORK* network, DEL_ORDER* delOrder, int* delOrderSize, int* edgeCnt);
#endif