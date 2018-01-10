__author__ = 'david'

from common import LabelDataset as ld
import cPickle
import numpy as np

#
#   raw_data:
#       coarse_label_names: List of coarse label names
#       fine_label_names:   List of fine label names
#
class Labels(ld.LabelDataset):
    def __init__(self, file, which_labels='coarse'):
        self._which = which_labels
        with open(file, 'rb') as fo:
            self._raw_data = cPickle.load(fo)

    @property
    def strings(self):
        return np.array(self._raw_data[self._which + '_label_names'])