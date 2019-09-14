import os
import unittest
import networkx as nx
from pygncd import gn_inner_routine

class TestGNCD(unittest.TestCase):
    def test_return_value(self):
        g = nx.readwrite.gml.read_gml(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'karate.gml'), label=None)
        rv = gn_inner_routine(g)

if __name__ == '__main__':
    unittest.main()