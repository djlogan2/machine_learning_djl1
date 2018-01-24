__author__ = 'david'

from common import Datasets as da
from . import Dataset, Labels


class Datasets(da.Datasets):
    def __init__(self, validation_percent=0, include_bias=True, zero_center=True, rgb=True, record_count=10000):

        self._labels = Labels.Labels(rgb=rgb)
        self._training = Dataset.Dataset(include_bias, zero_center, rgb=rgb, record_count=record_count)
        self._test = Dataset.Dataset(include_bias, zero_center, training_mean = self._training.mean_without_bias, rgb=rgb, record_count=record_count)

        da.Datasets.__init__(self, validation_percent)