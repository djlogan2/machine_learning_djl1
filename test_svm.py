__author__ = 'david'

import numpy as np
import tensorflow as tf
import time
from mnist import Datasets as _mda
from smiley import Datasets as _sda
from cifar import Datasets as _cda
from common import WriteImages as wi
from tensorflow.contrib.learn.python.learn.estimators import SVM

validation_percent = 0
zero_center = 1
which_data = 'cifar' # 'mnist' or 'cifar' or 'smiley'

if which_data == 'mnist':
    da = _mda.Datasets('/home/david/workspace/machine_learning/djl1/mnist_data', include_bias=0, validation_percent=validation_percent, zero_center=zero_center)
    f1 = tf.contrib.layers.real_valued_column('p', dimension=784) # Just a pixel for mnist
    feature_columns = [f1]
elif which_data == 'smiley':
    da = _sda.Datasets(include_bias=0, validation_percent=validation_percent, zero_center=zero_center, record_count=12000)
    f1 = tf.contrib.layers.real_valued_column('r', dimension=1024) # But the RGB values for cifar
    f2 = tf.contrib.layers.real_valued_column('g', dimension=1024)
    f3 = tf.contrib.layers.real_valued_column('b', dimension=1024)
    feature_columns = [f1, f2, f3]
else:
    da = _cda.Datasets('/home/david/workspace/machine_learning/djl1/cifar_data', include_bias=0, validation_percent=validation_percent, zero_center=zero_center)
    f1 = tf.contrib.layers.real_valued_column('r', dimension=1024) # But the RGB values for cifar
    f2 = tf.contrib.layers.real_valued_column('g', dimension=1024)
    f3 = tf.contrib.layers.real_valued_column('b', dimension=1024)
    feature_columns = [f1, f2, f3]

if which_data == 'mnist':
    wi.WriteImages.write(da.training.raw_image_data, [], 'images.png')
else:
    wi.WriteImages.write(da.training.raw_image_data.transpose(0, 2, 3, 1), [], 'images.png')

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
    estimator.fit(input_fn=train_input_fn, steps=1)
    end = time.time()
else:
    data, labels = train_input_fn()
    start = time.time()
    estimator.fit(x=data, y=labels, batch_size=100, steps=1000)
    end = time.time()

if which_data == 'mnist':
    p_weights = estimator.get_variable_value('linear/p/weight').reshape(28, 28, da.label.count).transpose(2, 0, 1)
    p_weights = ((p_weights * 127.5) + 127.5).astype(np.uint8)
    wi.WriteImages.write(p_weights, [], 'p_weights.png')
else:
    r_weights = estimator.get_variable_value('linear/r/weight').reshape(32,32,da.label.count).transpose(2,0,1)
    g_weights = estimator.get_variable_value('linear/g/weight').reshape(32,32,da.label.count).transpose(2,0,1)
    b_weights = estimator.get_variable_value('linear/b/weight').reshape(32,32,da.label.count).transpose(2,0,1)

    r_weights = ((r_weights * 127.5) + 127.5).astype(np.uint8)
    g_weights = ((g_weights * 127.5) + 127.5).astype(np.uint8)
    b_weights = ((b_weights * 127.5) + 127.5).astype(np.uint8)

    wi.WriteImages.write(r_weights, [], 'r_weights.png')
    wi.WriteImages.write(g_weights, [], 'g_weights.png')
    wi.WriteImages.write(b_weights, [], 'b_weights.png')

    #all = np.dstack((r_weights, g_weights, b_weights)).reshape(3, da.label.count, 32, 32).transpose(1,2,3,0)   # 3, lc, 32, 32
    #wi.WriteImages.write(all, [], 'all_weights.png')

print('Elapsed time: {} seconds'.format(end-start))
eval_metrics = estimator.evaluate(input_fn=eval_input_fn, steps=1)
print(eval_metrics)