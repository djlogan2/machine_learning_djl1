__author__ = 'david'

from common import LabelDataset as ld
import numpy as np

class Labels(ld.LabelDataset):
    def __init__(self):
        pass

    @property
    def strings(self):
        return np.array(['0', '1', '2', '3', '4', '5', '6', '7', '8', '9'])
