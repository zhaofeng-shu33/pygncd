from libc.stdlib cimport malloc, free

cdef extern from "gn/network.h":
    struct NETWORK
    struct LABELLIST:
         LABELLIST* next
         LABELLIST* prev
         int* labels
    void construct_network(NETWORK** network, double array_list[][3], int num_edges, int num_vertices)

cdef extern from "gn/gn.h":
    void girvan_newman(NETWORK*, LABELLIST*)
    
cpdef gn_inner_routine(networkx_graph):
    cdef int nedges, nvertices
    cdef int i
    cdef NETWORK* network
    cdef LABELLIST* label_header, *tmp
    cdef double (*array_list)[3]
    nedges = len(networkx_graph.edges)
    nvertices = len(networkx_graph.nodes)
    array_list = <double(*)[3]>malloc(3 * sizeof(double) * nedges)
    label_header = <LABELLIST*>malloc(sizeof(LABELLIST))
    
    i = 0
    smallest_node_index = next(networkx_graph.nodes.__iter__())
    for u,v,w in networkx_graph.edges(data='weight', default=1):
        array_list[i][0] = <double>(u-smallest_node_index)
        array_list[i][1] = <double>(v-smallest_node_index)
        array_list[i][2] = <double>w
        i += 1
    
    construct_network(&network, array_list, nedges, nvertices)
    girvan_newman(network, label_header)
    py_labels_list = []
    while label_header.next != NULL:
        py_labels = []
        for i in range(nvertices):
            py_labels.append(label_header.labels[i])
        py_labels_list.append(py_labels)
        label_header = label_header.next        
    py_labels = []
    for i in range(nvertices):
        py_labels.append(label_header.labels[i])
    py_labels_list.append(py_labels)        
    while label_header.prev != NULL:
        tmp = label_header.prev
        free(label_header.labels)
        free(label_header)
        label_header = tmp
    free(array_list)
    return py_labels_list
