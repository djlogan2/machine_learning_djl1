__author__ = 'david'

import unittest
from common import MultiSVM

one_record = [[1,2,3],
              [4,5,6],
              [7,8,9]]
class testSVM(unittest.TestCase):
    def test_SVM(self):
        svm = MultiSVM.MultiSVM(9, 5, 0.5)
        pass