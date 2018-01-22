__author__ = 'david'

import numpy as np


class ImageDataset:
    def __init__(self, label_count, include_bias, zero_center, data=None, labels=None, training_mean=None):
        self._label_count = label_count
        self._include_bias = include_bias
        self._zero_center = zero_center

        if data is not None:
            self.image_data = data
            self.label_data = labels

        if training_mean is not None:
            self._mean_without_bias = training_mean

    @classmethod
    def fromData(cls, label_count, data, labels, include_bias, zero_center, training_mean=None):
        return cls(label_count, include_bias, zero_center, data=data, labels=labels, training_mean=training_mean)
    #
    # Virtual methods/properties
    #
    # raw_image_data - an any-d array with records and attributes
    # label_data - a 1d array with #records of labels
    #
    # End of virtual methods
    #

    @property
    def mean(self):
        try:
            return self._mean
        except AttributeError:
            self._mean = self.mean_without_bias
            if self._include_bias == True:
                self._mean = np.column_stack([self.mean_without_bias, [0]])
            else:
                self._mean = self.mean_without_bias
            return self._mean

    @property
    def mean_without_bias(self):
        try:
            return self._mean_without_bias
        except AttributeError:
            self._mean_without_bias = (np.sum(self.raw_image_data.T, axis=-1) / self.raw_image_data.shape[0])\
                .reshape(np.prod(self.raw_image_data.shape[1:]))
            return self._mean_without_bias

    @property
    def image_data(self):
        try:
            return self._image_data
        except AttributeError:
            rc = self.raw_image_data.shape[0]
            cc = np.prod(self.raw_image_data.shape[1:])
            self._image_data = np.array(self.raw_image_data,dtype=np.float32).reshape(rc, cc)
            if self._zero_center == True:
                self._image_data -= self.mean_without_bias.flatten()
            self._image_data /= 255.0

            if self._include_bias == True:
                self._image_data = np.column_stack([self.image_data, np.full((rc), 1)])

            return self._image_data

    def random_batch(self, num_records, one_hot=True):

        try:
            temp_array = self._combined_array
        except AttributeError:
            self._combined_array = temp_array = np.column_stack([self.image_data, self.label_data])

        np.random.shuffle(temp_array)
        temp_array = temp_array[:num_records,:]

        new_labels = np.array(temp_array[:, temp_array.shape[-1] - 1], dtype=np.int32)
        if one_hot == True:
            new_labels = np.eye(self._label_count)[new_labels]

        new_data = np.delete(temp_array, temp_array.shape[-1] - 1, axis=1)
        return new_data, new_labels

    def random_split(self, split_percent):
        total = self.image_data.shape[0]
        small_count = total * split_percent / 100
        big_count = total - small_count
        temp_array = np.column_stack([self.image_data, self.label_data])
        np.random.shuffle(temp_array)
        new_labels = np.array(temp_array[:, temp_array.shape[-1] - 1], dtype=np.uint32)
        new_data = np.delete(temp_array, temp_array.shape[-1] - 1, axis=1)
        self.image_data, split_data = new_data[:big_count,:], new_data[big_count:,:]
        self.label_data, split_labels = new_labels[:big_count], new_labels[big_count:]
        return ImageDataset.fromData(self._label_count, split_data, split_labels, 0, self._zero_center, training_mean=self.mean_without_bias) # Never include bias in the validation dataset. If it's set, we've added it to the training data already!

    @property
    def one_hot_label_data(self):
        return np.eye(self._label_count)[self.label_data]

    #
    # array = (rgb*w*h, categories)
    #
    def convert_weights_to_pil(self, array):
        bias = None
        if array.shape[0] != np.prod(self.raw_image_data.shape[1:]):
            if self._include_bias != 1 or np.prod(self.raw_image_data.shape[1:]) + 1 != array.shape[0]:
                raise 'Unexpected array size for the first dimension'
            array = array.T
            bias = np.array(array[:, array.shape[-1] - 1])
            array = np.delete(array, array.shape[-1] - 1, axis=1)
        else:
            array = array.T

        if array.shape[0] != self._label_count:
            raise 'Did you send in weights? The array does not match label counts'

        array *= 255
        if bias is not None:
            bias *= 255

        if self._zero_center == True:
            array += self.mean_without_bias
            if bias is not None:
                bias += 127
            #if bias is not None:
            #    bias += self.mean_without_bias

        if bias is not None:
            bias = np.array(bias, dtype=np.uint8)

        newshape = [self._label_count] + list(self.raw_image_data.shape[1:])
        array = np.array(array).reshape(newshape)

        return np.array(array, dtype=np.uint8), bias
