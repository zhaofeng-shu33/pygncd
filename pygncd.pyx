from libc.stdlib cimport malloc, free

cdef extern from "gn/network.h":
    struct NETWORK:
        pass
    struct LABELLIST:
         LABELLIST* next
    void construct_network(NETWORK**, double**, int, int)

cdef extern from "gn/gn.h":
    void girvan_newman(NETWORK*, LABELLIST*)
    
cdef gn_inner_routine(networkx_graph):
    cdef int nedges, nvertices
    cdef int i
    cdef NETWORK* network
    cdef LABELLIST* label_header
    cdef double** array_list
    nedges = len(networkx_graph.nodes)
    nvertices = len(networkx_graph.edges)
    array_list = <double**>malloc(sizeof(double*) * nedges)
    label_header = <LABELLIST*>malloc(sizeof(LABELLIST))
    for i in range(nedges):
        array_list[i] = <double*>malloc(sizeof(double*) * 3)
    
    i = 0
    for u,v,w in networkx_graph.edges(data='weight', default=1):
        array_list[i][0] = <double>u
        array_list[i][1] = <double>v
        array_list[i][2] = <double>w
        i += 1
    
    construct_network(&network, array_list, nedges, nvertices)
    girvan_newman(network, label_header)
    #while label_header.next != NULL:
    #    label_header = label_header.next
    #while label_header->prev != NULL:
    #    pass
    for i in range(nedges):
        free(array_list[i])
    free(array_list)