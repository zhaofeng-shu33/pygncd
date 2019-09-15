import os
import unittest
from GN import GN
class TestGN(unittest.TestCase):
    def test_gn_wrapper(self):
        current_dir = os.path.dirname(os.path.realpath(__file__))
        g = nx.readwrite.gml.read_gml(os.path.join(current_dir, 'karate.gml'), label=None)
        gn = GN()
        gn.fit(g)
        print(gn.tree)
