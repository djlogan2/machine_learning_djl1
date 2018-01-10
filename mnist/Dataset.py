__author__ = 'david'

import numpy as np
from common import ImageDataset as da

# Label file
#[offset] [type]          [value]          [description]
#0000     32 bit integer  0x00000801(2049) magic number (MSB first)
#0004     32 bit integer  60000            number of items
#0008     unsigned byte   ??               label
#0009     unsigned byte   ??               label
#........
#xxxx     unsigned byte   ??               label
#The labels values are 0 to 9.

# Image file
#[offset] [type]          [value]          [description]
#0000     32 bit integer  0x00000803(2051) magic number
#0004     32 bit integer  60000            number of images
#0008     32 bit integer  28               number of rows
#0012     32 bit integer  28               number of columns
#0016     unsigned byte   ??               pixel
#0017     unsigned byte   ??               pixel
#........
#xxxx     unsigned byte   ??               pixel

class Dataset(da.ImageDataset):
    def __init__(self, image_file, label_file, label_count, include_bias, zero_center, training_mean=None):
        self.include_bias = include_bias
        self._load_image_file(image_file)
        self._load_label_file(label_file)
        da.ImageDataset.__init__(self, label_count, include_bias, zero_center, training_mean=training_mean)

    @property
    def raw_image_data(self):
        return self._raw_image_data

#    @property
#    def pil_data(self):
#        return Dataset.convert_1d_to_pil(self._raw_image_data)

    @property
    def label_data(self):
        return self._label_data

    def _load_image_file(self, file):
        with open(file, 'rb') as fo:
            magic, num_images, num_rows, num_columns = np.fromfile(fo, dtype=np.dtype('>i4'), count=4)
            if magic != 2051:
                raise Exception('Invalid image file magic number')
            self._raw_image_data = np.fromfile(fo, dtype=np.uint8, count=-1)
            self._raw_image_data = self._raw_image_data.reshape(num_images, num_columns, num_rows)

    def _load_label_file(self, file):
        with open(file, 'rb') as fo:
            magic, num_items = np.fromfile(fo, dtype=np.dtype('>i4'), count=2)
            if magic != 2049:
                raise Exception('Invalid label file magic number')
            self._label_data = np.fromfile(fo, dtype=np.uint8, count=-1)
            if self._label_data.shape[0] != num_items:
                raise Exception('Invalid labels in file')
