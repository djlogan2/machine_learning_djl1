__author__ = 'david'

import unittest
import numpy as np

from cifar import Datasets as cifar
from common import ImageDataset
from mnist import Datasets as mnist


class DatasetTests(unittest.TestCase):

    def loadMNIST(self, validation_percent=0, batch_percent=100, include_bias=1):
        mn = mnist.Datasets('/home/david/workspace/machine_learning/djl1/mnist_data', validation_percent=validation_percent, batch_percent=batch_percent, include_bias=include_bias)
        if validation_percent == 0:
            self.assertIsNone(mn.validation)
            data_records = 60000
        else:
            self.assertIsNotNone(mn.validation)
            self.assertIsNotNone(mn.validation.image_data)
            self.assertIsNotNone(mn.validation.label_data)
            self.assertEqual(mn.validation.image_data.shape[0], 60000 * validation_percent / 100)
            self.assertEqual(mn.validation.label_data.shape[0], 60000 * validation_percent / 100)
            data_records = 60000 - mn.validation.image_data.shape[0]

        if batch_percent == 100:
            #xxx = mn.training.PILImage(0)
            self.assertEqual(mn.training.image_data.shape[0], data_records)
            self.assertEqual(mn.training.label_data.shape[0], data_records)
            data, labels = mn.training.next_batch
            self.assertTrue(np.array_equal(mn.training.image_data, data))
            self.assertTrue(np.array_equal(mn.training.label_data, labels))
        else:
            count = data_records * batch_percent / 100
            batches = data_records / count
            self.assertEqual(mn.training.image_data.shape[0], batches)
            self.assertEqual(mn.training.image_data.shape[1], count)
            all_data, all_labels = mn.training.next_batch
            for x in range(1,batches):
                data_batch, label_batch = mn.training.next_batch
                all_data = np.append(all_data,data_batch , axis=0)
                all_labels = np.append(all_labels, label_batch, axis=0)
            all_data = all_data.reshape([batches, count] + list(mn.training.image_data.shape[2:]))
            all_labels = all_labels.reshape([batches] + list(mn.training.label_data.shape[1:]))
            self.assertTrue(np.array_equal(mn.training.image_data, all_data))
            self.assertTrue(np.array_equal(mn.training.label_data, all_labels))

        if include_bias == 1:
            self.assertEqual(mn.training.image_data.shape[-1], 785)
            self.assertEqual(mn.test.image_data.shape[-1], 785)
            if validation_percent != 0:
                self.assertEqual(mn.validation.image_data.shape[-1], 785)
        else:
            self.assertEqual(mn.training.image_data.shape[-1], 784)
            self.assertEqual(mn.test.image_data.shape[-1], 784)
            if validation_percent != 0:
                self.assertEqual(mn.validation.image_data.shape[-1], 784)
        
    def loadCIFAR(self, validation_percent=0, batch_percent=100, include_bias=1):
        cf = cifar.Datasets('/home/david/workspace/machine_learning/djl1/cifar_data',
                            validation_percent=validation_percent,
                            batch_percent=batch_percent,
                            include_bias=include_bias)

        if validation_percent == 0:
            self.assertIsNone(cf.validation)
            data_records = 50000
        else:
            self.assertIsNotNone(cf.validation)
            self.assertIsNotNone(cf.validation.image_data)
            self.assertIsNotNone(cf.validation.label_data)
            self.assertEqual(cf.validation.image_data.shape[0], 50000 * validation_percent / 100)
            self.assertEqual(cf.validation.label_data.shape[0], 50000 * validation_percent / 100)
            data_records = 50000 - cf.validation.image_data.shape[0]

        if batch_percent == 100:
            self.assertEqual(cf.training.image_data.shape[0], data_records)
            self.assertEqual(cf.training.label_data.shape[0], data_records)
            data, labels = cf.training.next_batch
            self.assertTrue(np.array_equal(cf.training.image_data, data))
            self.assertTrue(np.array_equal(cf.training.label_data, labels))
        else:
            count = data_records * batch_percent / 100
            batches = data_records / count
            self.assertEqual(cf.training.image_data.shape[0], batches)
            self.assertEqual(cf.training.image_data.shape[1], count)
            all_data, all_labels = cf.training.next_batch
            for x in range(1,batches):
                data_batch, label_batch = cf.training.next_batch
                all_data = np.append(all_data,data_batch , axis=0)
                all_labels = np.append(all_labels, label_batch, axis=0)
            all_data = all_data.reshape([batches, count] + list(cf.training.image_data.shape[2:]))
            all_labels = all_labels.reshape([batches] + list(cf.training.label_data.shape[1:]))
            self.assertTrue(np.array_equal(cf.training.image_data, all_data))
            self.assertTrue(np.array_equal(cf.training.label_data, all_labels))

        if include_bias == 1:
            self.assertEqual(cf.training.image_data.shape[-1], 3073)
            self.assertEqual(cf.test.image_data.shape[-1], 3073)
            if validation_percent != 0:
                self.assertEqual(cf.validation.image_data.shape[-1], 3073)
        else:
            self.assertEqual(cf.training.image_data.shape[-1], 3072)
            self.assertEqual(cf.test.image_data.shape[-1], 3072)
            if validation_percent != 0:
                self.assertEqual(cf.validation.image_data.shape[-1], 3072)

    def test_CIFAR(self):
        self.loadCIFAR()
        self.loadCIFAR(include_bias=0)
        self.loadCIFAR(batch_percent=10)
        self.loadCIFAR(batch_percent=10, include_bias=0)
        self.loadCIFAR(validation_percent=10)
        self.loadCIFAR(validation_percent=10, include_bias=0)
        self.loadCIFAR(validation_percent=10, batch_percent=10)
        self.loadCIFAR(validation_percent=10, batch_percent=10, include_bias=0)
        print 'done'

    def test_MNIST(self):
        self.loadMNIST()
        self.loadMNIST(include_bias=0)
        self.loadMNIST(batch_percent=10)
        self.loadMNIST(batch_percent=10, include_bias=0)
        self.loadMNIST(validation_percent=10)
        self.loadMNIST(validation_percent=10, include_bias=0)
        self.loadMNIST(validation_percent=10, batch_percent=10)
        self.loadMNIST(validation_percent=10, batch_percent=10, include_bias=0)
        print 'done'

    def test_random_shuffle(self):
        id = []
        ld = []

        for rc in range(0, 1000):
            ld.append(rc)
            id.append([])
            for cc in range(0, 100):
                id[rc].append(rc * 100 + cc)

        id = ImageDataset.ImageDataset.fromData(1, np.array(id), np.array(ld), 0)
        i2 = id.random_split(50)

        for rc in range(0, 500):
            label_value = id.label_data[rc]
            for cc in range(0, 100):
                self.assertEqual(id.image_data[rc,cc], (label_value * 100) + cc)

        for rc in range(0, 500):
            label_value = i2.label_data[rc]
            for cc in range(0, 100):
                self.assertEqual(i2.image_data[rc,cc], (label_value * 100) + cc)

if __name__ == '__main__':
    unittest.main()