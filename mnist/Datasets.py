__author__ = 'david'

from common import Datasets as da
import Dataset
import Labels


class Datasets(da.Datasets):
    def __init__(self, dir, validation_percent=0, include_bias=1, zero_center=1):

        self._labels = Labels.Labels()
        self._training = Dataset.Dataset(dir + '/train-images-idx3-ubyte', dir + '/train-labels-idx1-ubyte', self._labels.count, include_bias, zero_center)
        self._test = Dataset.Dataset(dir + '/t10k-images-idx3-ubyte', dir + '/t10k-labels-idx1-ubyte',
                                     self._labels.count, include_bias, zero_center, training_mean = self._training.mean_without_bias)

        da.Datasets.__init__(self, validation_percent)