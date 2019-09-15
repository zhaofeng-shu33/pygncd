import os
import unittest
import networkx as nx
from pygncd import gn_inner_routine

class TestGNCD(unittest.TestCase):
    def test_return_value(self):
        g = nx.readwrite.gml.read_gml(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'karate.gml'), label=None)
        rv = gn_inner_routine(g)
    def test_remove_largest(self):
        g = nx.Graph()
        g.add_edge(0, 1)
        g.add_edge(1, 2)
        g.add_edge(0, 2, weight=2)
        rv = gn_inner_routine(g)
        self.assertEqual(rv[0], [0,0,0])
        self.assertEqual(rv[0], [0,1,2])
        
if __name__ == '__main__':
    unittest.main()
