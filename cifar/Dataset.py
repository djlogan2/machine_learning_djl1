__author__ = 'david'

import numpy as np
from common import ImageDataset as da
import cPickle

#
# raw_data:
#   batch_label     - Just a string, naming this data. 'training batch 1 of 1'
#   coarse_labels   -   List of 50,000 numbers (coarse categories)
#   data            -   np.ndarray of 50,000 lists of uint8. Each int is 3072 bytes
#   filenames       -   List of 50,000 string filenames
#   fine_labels     -   List of 50,000 numbers (fine categories)


class Dataset(da.ImageDataset):
    def __init__(self, file, label_counts, include_bias, which_labels, zero_center, training_mean=None):
        self.label_type = which_labels + '_labels'
        with open(file, 'rb') as fo:
            self.raw_data = cPickle.load(fo)
        self.raw_data = self.raw_data.reshape(self.raw_data.shape[0], 3, 32, 32)
        self.raw_data = self.raw_data.transpose(axes=(0,2,3,1))

        da.ImageDataset.__init__(self, label_counts, include_bias, zero_center, training_mean=training_mean)

#    @property
#    def pil_data(self):
#        return Dataset.convert_1d_to_pil(self.raw_data['data'])

    @property
    def raw_image_data(self):
        return self.raw_data['data']

    @property
    def label_data(self):
        return np.array(self.raw_data[self.label_type])
