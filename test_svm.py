__author__ = 'david'

import numpy as np
import tensorflow as tf
import time
from mnist import Datasets as _mda
from cifar import Datasets as _cda
from common import WriteImages as wi
from tensorflow.contrib.learn.python.learn.estimators import SVM

validation_percent = 0
zero_center = 1
which_data = 'cifar' # 'mnist' or 'cifar'

if which_data == 'mnist':
    da = _mda.Datasets('/home/david/workspace/machine_learning/djl1/mnist_data', include_bias=0, validation_percent=validation_percent, zero_center=zero_center)
    f1 = tf.contrib.layers.real_valued_column('p', dimension=784) # Just a pixel for mnist
    feature_columns = [f1]
else:
    da = _cda.Datasets('/home/david/workspace/machine_learning/djl1/cifar_data', include_bias=0, validation_percent=validation_percent, zero_center=zero_center)
    f1 = tf.contrib.layers.real_valued_column('r', dimension=1024) # But the RGB values for cifar
    f2 = tf.contrib.layers.real_valued_column('g', dimension=1024)
    f3 = tf.contrib.layers.real_valued_column('b', dimension=1024)
    feature_columns = [f1, f2, f3]

x_times = 0

def xxget_input_fn(dataset, batch_size, capacity=50000, min_after_dequeue=1000):
    if which_data == 'mnist':
        def _input_fn():
            global x_times
            print('Asking for %d records %d times' % (batch_size, x_times))
            x_times += 1
            images, labels = tf.train.shuffle_batch(
                tensors = [dataset.image_data, dataset.label_data],
                batch_size=batch_size,
                capacity=capacity,
                min_after_dequeue=min_after_dequeue,
                enqueue_many=True,
                num_threads=4)
            return {'p': images}, tf.cast(labels, tf.int32)
        return _input_fn
    else:
        def _input_fn():
            global x_times
            print('Asking for %d records %d times' % (batch_size, x_times))
            x_times += 1
            images, labels = tf.train.shuffle_batch(
                tensors = [dataset.image_data, dataset.label_data],
                batch_size=batch_size,
                capacity=capacity,
                min_after_dequeue=min_after_dequeue,
                enqueue_many=True,
                num_threads=4)
            im2 = tf.reshape(images, [batch_size, 3, 1024])
            return {'r': im2[:,0], 'g': im2[:,1], 'b': im2[:,2]}, labels
        return _input_fn

#
#   The original input_fn for the SVN module, which doesn't seem to really work
#
def xxxget_input_fn(dataset, batch_size):
    def _input_fn():
        global x_times
        print('Calling input data ' + repr(x_times) + ' times')
        if which_data == 'mnist':
            id, labels = dataset.random_batch(batch_size, one_hot=0)
            return {
                'p': tf.constant(id)
              }, tf.constant(labels, dtype=np.int32)
        else:
            id, labels = dataset.random_batch(batch_size, one_hot=0)
            id = id.reshape(batch_size, 3, 32*32)
            return {
                'r': tf.constant(id[:,0]),
                'g': tf.constant(id[:,1]),
                'b': tf.constant(id[:,2])
              }, tf.constant(labels)
    return _input_fn

def get_input_fn(dataset):
    def _input_fn():
        if which_data == 'mnist':
            return {
                'p': tf.constant(dataset.image_data)
              }, tf.constant(dataset.label_data, dtype=np.int32)
        else:
            id = dataset.image_data.reshape(dataset.image_data.shape[0], 3, 32*32)
            return {
                'r': tf.constant(id[:,0]),
                'g': tf.constant(id[:,1]),
                'b': tf.constant(id[:,2])
              }, tf.constant(dataset.label_data)
    return _input_fn
#
# The SVM classifier, which doesn't seem to really work
#
# svm_classifier = tf.contrib.learn.SVM(
#     feature_columns=feature_columns,
#     example_id_column='example_id'
# )
#
# svm_classifier.fit(input_fn=input_fn, steps=1000)
# metrics = svm_classifier.evaluate(input_fn=input_fn, steps=1)
# print 'Loss', metrics['loss'], '\nAccuracy', metrics['accuracy']
train_input_fn = get_input_fn(da.training)
eval_input_fn = get_input_fn(da.test)

estimator = tf.contrib.learn.LinearClassifier(feature_columns=feature_columns, n_classes=da.label.count)

if True:
    start = time.time()
    estimator.fit(input_fn=train_input_fn, steps=1000)
    end = time.time()
else:
    data, labels = train_input_fn()
    start = time.time()
    estimator.fit(x=data, y=labels, batch_size=100, steps=1000)
    end = time.time()

print('Elapsed time: {} seconds'.format(end-start))
eval_metrics = estimator.evaluate(input_fn=eval_input_fn, steps=1)
print(eval_metrics)