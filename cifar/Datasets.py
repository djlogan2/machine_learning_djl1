__author__ = 'david'

from common import Datasets as da
import Dataset
import Labels


class Datasets(da.Datasets):
    def __init__(self, dir, validation_percent=0, include_bias=1, which_labels='coarse', zero_center=1):

        self._labels = Labels.Labels(dir + '/meta', which_labels=which_labels)
        self._training = Dataset.Dataset(dir + '/train', self._labels.count, include_bias, which_labels, zero_center)
        self._test = Dataset.Dataset(dir + '/test', self._labels.count, include_bias, which_labels, zero_center,
                                     training_mean=self._training.mean_without_bias)

        da.Datasets.__init__(self, validation_percent)