__author__ = 'david'

from common import LabelDataset as ld
import numpy as np

class Labels(ld.LabelDataset):
    def __init__(self, rgb=True):
        self._rgb = rgb

    @property
    def strings(self):
        return ['red', 'green', 'blue'] if self._rgb else ['smiley']
