__author__ = 'david'

import numpy as np

class Datasets:
    def __init__(self, validation_percent):
        self._validation_percent = validation_percent
        if self._validation_percent != 0:
            self._validation = self._training.random_split(self._validation_percent)
        else:
            self._validation = None

    @property
    def label(self):
        return self._labels

    @property
    def training(self):
        return self._training

    @property
    def validation(self):
        return self._validation

    @property
    def test(self):
        return self._test
