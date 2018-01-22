__author__ = 'david'

import numpy as np

class MultiSVM:
    def __init__(self, attribute_count, class_count, delta):
        self._attribute_count = attribute_count
        self._class_count = class_count
        self._delta = delta

        self._W = np.zeros([attribute_count, class_count], dtype=np.float32)
        pass

    def train(self, training_data, labels):  # training_data = [#records, #attributes]  labels=[#records]
        pass
