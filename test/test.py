import os
import unittest
import networkx as nx
from pygncd import gn_inner_routine

def label_list_to_partition_list(label_list):
    partition_list = []
    for labels in label_list:
        dic = {}
        for pos, i in enumerate(labels):
            if dic.get(i) is None:
                dic[i] = set()
            dic[i].add(pos)
        partition = [grp for grp in dic.values()]            
        partition_list.append(partition)
    return partition_list
    
class TestLtP(unittest.TestCase):
    def test_main_routine(self):
        label_list = [[0,0,0],[0,1,0],[0,1,2]]
        partition_list = label_list_to_partition_list(label_list)
        print(partition_list)
        
class TestGNCD(unittest.TestCase):
    def test_return_value(self):
        current_dir = os.path.dirname(os.path.realpath(__file__))
        g = nx.readwrite.gml.read_gml(os.path.join(current_dir, 'karate.gml'), label=None)
        rv = gn_inner_routine(g)

    def test_remove_largest(self):
        g = nx.Graph()
        g.add_edge(0, 1)
        g.add_edge(1, 2)
        g.add_edge(0, 2, weight=2)
        rv = gn_inner_routine(g)
        self.assertEqual(rv[0], [0,0,0])
        self.assertEqual(rv[1], [0,1,2])
        
if __name__ == '__main__':
    unittest.main()
