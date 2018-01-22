__author__ = 'david'

import numpy as np
import tensorflow as tf
from mnist import Datasets as _da
#from cifar import Datasets as _da
from common import WriteImages as wi

validation_percent=0    # Percentage to shave off for validation
batch_size = 100        # Break training data up into batches of x records
batch_count = 1000      # Number of times to run through the batch
zero_center = 1         # Whether to zero center or not

da = _da.Datasets('/home/david/workspace/machine_learning/djl1/mnist_data', include_bias=True, validation_percent=validation_percent, zero_center=zero_center)
#da = _da.Datasets('/home/david/workspace/machine_learning/djl1/cifar_data', include_bias=True, validation_percent=validation_percent, zero_center=zero_center)
#wi.WriteImages.write(da.training.raw_image_data, ['a','b'], 'images')
x = tf.placeholder(tf.float32, [None, da.training.image_data.shape[1]])
W = tf.Variable(tf.zeros([da.training.image_data.shape[1], da.label.count]))
#b = tf.Variable(tf.zeros([da.label.count]))
y = tf.nn.softmax(tf.matmul(x,W))

y_ = tf.placeholder(tf.float32, [None, da.label.count])
#cross_entropy = tf.reduce_mean(-tf.reduce_sum(y_ * tf.log(y), reduction_indices=[1]))
cross_entropy = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=y_, logits=y))

train_step = tf.train.GradientDescentOptimizer(0.5).minimize(cross_entropy)
sess = tf.InteractiveSession()

tf.global_variables_initializer().run()

for i in range(batch_count):
    if i % 100 == 0:
        print('loop ' + repr(i))
ß    data, labels = da.training.random_batch(batch_size)

    if np.amax(data) > 1.0:
        raise Exception('data values exceed 1.0')
    if np.amin(data) < -1.0:
        raise Exception('data values fall below 1.0')
    if np.sum(data[:,-1]) != batch_size:
        raise Exception('bias values incorrect')

    sess.run(train_step, feed_dict={x:data, y_: labels})

Wt = sess.run(W)

Wt_im, bias_im = da.training.convert_weights_to_pil(Wt)
wi.WriteImages.write(Wt_im, ['a','b'], 'weights')

#wi.WriteImages.write(bias_im, ['a', 'b'], '')
#bt = sess.run(b)

correct_prediction = tf.equal(tf.argmax(y,1), tf.argmax(y_,1))
accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
testdata = da.test.image_data
print(sess.run(accuracy, feed_dict={x: da.test.image_data, y_: da.test.one_hot_label_data}))
#print(sess.run(accuracy, feed_dict={x: da.training.image_data, y_: da.training.one_hot_label_data}))