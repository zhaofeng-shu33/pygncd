'''
    wrapper of Girvan-Newman community detection algorithm
'''
import networkx as nx
import numpy as np
from ete3 import Tree

try:
    from cmty import cmty # cython version first
except ImportError:
    import cmty

class GN_OLD:
    def __init__(self):
        self.reinit()
        
    def reinit(self):
        self.partition_num_list = []
        self.partition_list = []
        self.tree = Tree()
        self.tree_depth = 0
        
    def fit(self, G_outer, initialize_tree = True):
        '''
            G_outer: nx.Graph like object
            returns the partition
        '''    
        self.reinit()
        self.G = G_outer.copy()
        G = G_outer.copy()# copy the graph
        n = G.number_of_nodes()    #|V|
        A = nx.adj_matrix(G)    # adjacenct matrix

        m_ = 0.0    # the weighted version for number of edges
        for i in range(0,n):
            for j in range(0,n):
                m_ += A[i,j]
        self.m_ = m_/2.0

        # calculate the weighted degree for each node
        Orig_deg = {}
        self.Orig_deg = cmty.UpdateDeg(A, G.nodes())

        # run Newman alg
        self.runGirvanNewman() 
        if(initialize_tree):
            self._get_hierarchical_tree()
        return self
        
    def runGirvanNewman(self):
        # let's find the best split of the graph
        BestQ = 0.0
        Q = 0.0
        self.partition_num_list.append(1)
        nvertices = len(self.G.nodes)
        self.partition_list.append([set(i for i in range(nvertices))])
        while True:    
            cmty.CmtyGirvanNewmanStep(self.G)
            partition = list(nx.connected_components(self.G))
            self.partition_num_list.append(len(partition))
            self.partition_list.append(partition)
            Q = cmty._GirvanNewmanGetModularity(self.G, self.Orig_deg, self.m_)
            if Q > BestQ:
                BestQ = Q
                Bestcomps = partition    # Best Split
            if self.G.number_of_edges() == 0:
                break
        if BestQ > 0.0:
            self.Bestcomps = Bestcomps

    def get_category(self, i):
        index = 0
        for ind,val in enumerate(self.partition_num_list):
            if(val >= i):
                index = ind
                break
        cat = np.zeros(len(self.Orig_deg))
        t = 0
        for j in self.partition_list[index]:
            for r in j:
                cat[r] = t
            t += 1
        return cat

    def get_tree_depth(self):
        return 0

    def _add_node(self, root, node_list, num_index):

        label_list = self.get_category(self.partition_num_list[num_index])
        cat_list = []
        for i in node_list:
            if(cat_list.count(label_list[i]) == 0):
                cat_list.append(label_list[i])
        max_cat = len(cat_list)
        label_list_list = [[] for i in range(max_cat)]
        for i in node_list:
            j = cat_list.index(label_list[i])
            label_list_list[j].append(i)
        for node_list_i in label_list_list:
            node_name = ''.join([str(ii) for ii in node_list_i])
            if(node_name != root.name):
                root_i = root.add_child(name= node_name)
            else:
                root_i = root
            if(len(node_list_i)>1):
                self._add_node(root_i, node_list_i, num_index+1)
                
    def _get_hierarchical_tree(self):
        max_num = self.partition_num_list[-1]
        node_list = [ i for i in range(0, max_num)]
        self._add_node(self.tree, node_list, 1)

    def _set_tree_depth(self, node, depth):
        if(node.is_leaf()):
            if(depth > self.tree_depth):
                self.tree_depth = depth
            return
        for node_i in node.children: # depth first search
            self._set_tree_depth(node_i, depth+1)
            
    def get_tree_depth(self):
        if(self.tree.is_leaf()):
            self._get_hierarchical_tree()
        if(self.tree_depth != 0):
            return self.tree_depth
        self._set_tree_depth(self.tree, 0)
        return self.tree_depth
