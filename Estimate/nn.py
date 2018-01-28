import numpy as np
import os
import time
import tensorflow as tf
print("[Info] Tensorflow loaded")
batch_size=128

def weight_variable(shape):
    stddev = np.sqrt(2.0 / (sum(shape)))
    initial = tf.truncated_normal(shape, stddev=stddev)
    return tf.Variable(initial)

def bias_variable(shape):
    initial = tf.constant(0.0, shape=shape)
    return tf.Variable(initial)

def bn_bias_variable(shape):
    initial = tf.constant(0.0, shape=shape)
    return tf.Variable(initial, trainable=False)

def conv2d(x, W):
    return tf.nn.conv2d(x, W, data_format='NCHW',
                        strides=[1, 1, 1, 1], padding='SAME')

#Now max_pooling is not useful
def max_pool(x):
    return tf.nn.max_pool(x, ksize=[1, 2, 2, 1], strides=[1,2,2,1],padding='SAME')

BSIZE = 15

class TFProcess:
    def __init__(self, parafile = None, trainingfile = None):
        gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.5)
        config = tf.ConfigProto(gpu_options=gpu_options)

        self.graph=tf.Graph()
        self.session=tf.Session(config=config, graph=self.graph)
        
        with self.graph.as_default():
            # For exporting
            self.weights = []

            # TF variables
            #self.next_batch = next_batch
            self.global_step = tf.Variable(0, name='global_step', trainable=False)
            '''
            self.x = next_batch[0] 
            self.y_ = next_batch[1]
            self.z_ = next_batch[2]
            '''
            self.x = tf.placeholder(tf.float32, [None, 2, BSIZE * BSIZE])
            self.y_ =tf.placeholder(tf.float32, [None, BSIZE*BSIZE])
            self.z_ =tf.placeholder(tf.float32, [None, 1])

            self.training = tf.placeholder(tf.bool)
            self.batch_norm_count = 0
            self.y_conv, self.z_conv = self.construct_net(self.x)
            self.y_policy=tf.nn.softmax(self.y_conv)
            print("[Info] Model constructed")
            if trainingfile is None:
                self.init_forward(parafile)
            else:
                self.init_training(parafile, trainingfile)

    def loadParas(self, parafile):
        checkpoint_dir=parafile
        #返回checkpoint文件中checkpoint的状态
        ckpt = tf.train.get_checkpoint_state(checkpoint_dir)
        #print(ckpt)
        if ckpt and ckpt.model_checkpoint_path:#如果存在以前保存的模型
            print('[Info] Restore the model from checkpoint %s' % ckpt.model_checkpoint_path)
            # Restores from checkpoint
            self.saver.restore(self.session, ckpt.model_checkpoint_path)#加载模型
            #start_step = int(ckpt.model_checkpoint_path.split('/')[-1].split('-')[-1])
        else:
            print("[Error] Failed to find model")

    def init_forward(self, parafile):
        if not parafile is None:
            self.saver=tf.train.Saver()
            self.loadParas(parafile)
        else:
            self.init = tf.global_variables_initializer()
            self.session.run(self.init)
        
    def init_training(self, parafile, trainingfile):
        # Calculate loss on policy head
        cross_entropy = \
            tf.nn.softmax_cross_entropy_with_logits(labels=self.y_,
                                                    logits=self.y_conv)
        self.policy_loss = tf.reduce_mean(cross_entropy)
        
        # Loss on value head
        self.mse_loss = \
            tf.reduce_mean(tf.squared_difference(self.z_, self.z_conv))

        # Regularizer
        regularizer = tf.contrib.layers.l2_regularizer(scale=0.0001)
        reg_variables = tf.trainable_variables()
        reg_term = \
            tf.contrib.layers.apply_regularization(regularizer, reg_variables)

        loss = 1.0 * self.policy_loss + 1.0 * self.mse_loss + reg_term

        opt_op = tf.train.MomentumOptimizer(
            learning_rate=0.05, momentum=0.9, use_nesterov=True)

        self.update_ops = tf.get_collection(tf.GraphKeys.UPDATE_OPS)
        with tf.control_dependencies(self.update_ops):
            self.train_op = \
                opt_op.minimize(loss, global_step=self.global_step)

        correct_prediction = \
            tf.equal(tf.argmax(self.y_conv, 1), tf.argmax(self.y_, 1))
        correct_prediction = tf.cast(correct_prediction, tf.float32)
        self.accuracy = tf.reduce_mean(correct_prediction)

        self.avg_policy_loss = None
        self.avg_mse_loss = None
        self.time_start = None

        # Summary part
        self.test_writer = tf.summary.FileWriter(
            os.path.join(os.getcwd(), "logs/test"), self.session.graph)
        self.train_writer = tf.summary.FileWriter(
            os.path.join(os.getcwd(), "logs/train"), self.session.graph)
        print("[Info] Training steps loaded")
        self.saver=tf.train.Saver()
        self.save_file=trainingfile
        
        if not parafile is None:
            self.loadParas(parafile)
        else:
            self.init = tf.global_variables_initializer()
            self.session.run(self.init)
    
    def restore(self, file):
        print("Restoring from {0}".format(file))
        self.saver.restore(self.session, file)

    def forward(self, input):
        y,z=self.session.run([self.y_policy, self.z_conv],\
                             feed_dict={self.x:input.reshape([-1,2,BSIZE*BSIZE]), self.training:False});
        return y,z
    
    def process(self, batch):
        # Run training for this batch
        policy_loss, mse_loss, _ = self.session.run(
            [self.policy_loss, self.mse_loss, self.train_op],
            feed_dict={self.training: True,
                       self.x:batch[0], self.y_:batch[1], self.z_: batch[2]})
        steps = tf.train.global_step(self.session, self.global_step)
        
        # Keep running averages
        # XXX: use built-in support like tf.moving_average_variables?
        # Google's paper scales MSE by 1/4 to a [0, 1] range, so do the same to
        # get comparable values.
        # mse_loss = mse_loss / 4.0
        if self.avg_policy_loss:
            self.avg_policy_loss = 0.99 * self.avg_policy_loss + 0.01 * policy_loss
        else:
            self.avg_policy_loss = policy_loss
        if self.avg_mse_loss:
            self.avg_mse_loss = 0.99 * self.avg_mse_loss + 0.01 * mse_loss
        else:
            self.avg_mse_loss = mse_loss
        if steps % 100 == 0:
            time_end = time.time()
            speed = 0
            if self.time_start:
                elapsed = time_end - self.time_start
                speed = batch_size * (100.0 / elapsed)
            print("step {}, policy loss={:g} mse={:g} ({:g} pos/s)".format(
                steps, self.avg_policy_loss, self.avg_mse_loss, speed))
            train_summaries = tf.Summary(value=[
                tf.Summary.Value(tag="Policy Loss", simple_value=self.avg_policy_loss),
                tf.Summary.Value(tag="MSE Loss", simple_value=self.avg_mse_loss)])
            self.train_writer.add_summary(train_summaries, steps)
            self.time_start = time_end
            
        # Ideally this would use a seperate dataset and so on...
        if steps % 2000 == 0:
            '''
            sum_accuracy = 0
            sum_mse = 0
            for _ in range(0, 10):
                train_accuracy, _ = self.session.run(
                    [self.accuracy, self.next_batch],
                    feed_dict={self.training: False})
                train_mse, _ = self.session.run(
                    [self.mse_loss, self.next_batch],
                    feed_dict={self.training: False})
                sum_accuracy += train_accuracy
                sum_mse += train_mse
            sum_accuracy /= 10.0
            # Additionally rescale to [0, 1] so divide by 4
            sum_mse /= (10.0)
            test_summaries = tf.Summary(value=[
                tf.Summary.Value(tag="Accuracy", simple_value=sum_accuracy),
                tf.Summary.Value(tag="MSE Loss", simple_value=sum_mse)])
            self.test_writer.add_summary(test_summaries, steps)
            print("step {}, training accuracy={:g}%, mse={:g}".format(
                steps, sum_accuracy*100.0, sum_mse))
            '''
            path = os.path.join(os.getcwd(), self.save_file)
            save_path = self.saver.save(self.session, path, global_step=steps)
            print("Model saved in file: {}".format(save_path))

        if steps % 16000 == 0:
            return False
        return True
    
    def save_weights(self, filename):
        with open(filename, "w") as file:
            # Version tag
            file.write("1")
            for weights in self.weights:
                # Newline unless last line (single bias)
                file.write("\n")
                work_weights = None
                # Keyed batchnorm weights
                if isinstance(weights, str):
                    work_weights = tf.get_default_graph().get_tensor_by_name(weights)
                elif weights.shape.ndims == 4:
                    work_weights = tf.transpose(weights, [3, 2, 0, 1])
                elif weights.shape.ndims == 2:
                    work_weights = tf.transpose(weights, [1, 0])
                else:
                    # Biases, batchnorm etc
                    work_weights = weights
                nparray = work_weights.eval(session=self.session)
                wt_str = [str(wt) for wt in np.ravel(nparray)]
                file.write(" ".join(wt_str))

    def get_batchnorm_key(self):
        result = "bn" + str(self.batch_norm_count)
        self.batch_norm_count += 1
        return result

    def conv_block(self, inputs, filter_size, input_channels, output_channels):
        W_conv = weight_variable([filter_size, filter_size,
                                  input_channels, output_channels])
        b_conv = bn_bias_variable([output_channels])
        self.weights.append(W_conv)
        self.weights.append(b_conv)
        # The weights are internal to the batchnorm layer, so apply
        # a unique scope that we can store, and use to look them back up
        # later on.
        weight_key = self.get_batchnorm_key()
        self.weights.append(weight_key + "/batch_normalization/moving_mean:0")
        self.weights.append(weight_key + "/batch_normalization/moving_variance:0")

        with tf.variable_scope(weight_key):
            h_bn = \
                tf.layers.batch_normalization(
                    tf.nn.bias_add(conv2d(inputs, W_conv),
                                   b_conv, data_format='NCHW'),
                    epsilon=1e-5, axis=1, fused=True,
                    center=False, scale=False,
                    training=self.training)
        h_conv = tf.nn.relu(h_bn)
        return h_conv

    def residual_block(self, inputs, channels):
        # First convnet
        orig = tf.identity(inputs)
        W_conv_1 = weight_variable([3, 3, channels, channels])
        b_conv_1 = bn_bias_variable([channels])
        self.weights.append(W_conv_1)
        self.weights.append(b_conv_1)
        weight_key_1 = self.get_batchnorm_key()
        self.weights.append(weight_key_1 + "/batch_normalization/moving_mean:0")
        self.weights.append(weight_key_1 + "/batch_normalization/moving_variance:0")

        # Second convnet
        W_conv_2 = weight_variable([3, 3, channels, channels])
        b_conv_2 = bn_bias_variable([channels])
        self.weights.append(W_conv_2)
        self.weights.append(b_conv_2)
        weight_key_2 = self.get_batchnorm_key()
        self.weights.append(weight_key_2 + "/batch_normalization/moving_mean:0")
        self.weights.append(weight_key_2 + "/batch_normalization/moving_variance:0")

        with tf.variable_scope(weight_key_1):
            h_bn1 = \
                tf.layers.batch_normalization(
                    tf.nn.bias_add(conv2d(inputs, W_conv_1),
                                   b_conv_1, data_format='NCHW'),
                    epsilon=1e-5, axis=1, fused=True,
                    center=False, scale=False,
                    training=self.training)
        h_out_1 = tf.nn.relu(h_bn1)
        with tf.variable_scope(weight_key_2):
            h_bn2 = \
                tf.layers.batch_normalization(
                    tf.nn.bias_add(conv2d(h_out_1, W_conv_2),
                                   b_conv_2, data_format='NCHW'),
                    epsilon=1e-5, axis=1, fused=True,
                    center=False, scale=False,
                    training=self.training)
        h_out_2 = tf.nn.relu(tf.add(h_bn2, orig))
        return h_out_2

    def construct_net(self, planes):
        # Network structure
        RESIDUAL_FILTERS = 48
        RESIDUAL_BLOCKS = 5

        # NCHW format
        # batch, 2 channels, 15 x 15
        x_planes = tf.reshape(planes, [-1, 2, BSIZE, BSIZE])

        # Input convolution
        flow = self.conv_block(x_planes, filter_size=3,
                               input_channels=2,
                               output_channels=RESIDUAL_FILTERS)
        # Residual tower
        for _ in range(0, RESIDUAL_BLOCKS):
            flow = self.residual_block(flow, RESIDUAL_FILTERS)

        # Policy head
        conv_pol = self.conv_block(flow, filter_size=1,
                                   input_channels=RESIDUAL_FILTERS,
                                   output_channels=2)
        h_conv_pol_flat = tf.reshape(conv_pol, [-1, 2*BSIZE*BSIZE])
        W_fc1 = weight_variable([2 * BSIZE * BSIZE, (BSIZE * BSIZE)])
        b_fc1 = bias_variable([(BSIZE * BSIZE)])
        self.weights.append(W_fc1)
        self.weights.append(b_fc1)
        h_fc1 = tf.add(tf.matmul(h_conv_pol_flat, W_fc1), b_fc1)

        # Value head
        conv_val = self.conv_block(flow, filter_size=1,
                                   input_channels=RESIDUAL_FILTERS,
                                   output_channels=1)
        h_conv_val_flat = tf.reshape(conv_val, [-1, BSIZE*BSIZE])
        W_fc2 = weight_variable([BSIZE * BSIZE, 128])
        b_fc2 = bias_variable([128])
        self.weights.append(W_fc2)
        self.weights.append(b_fc2)
        h_fc2 = tf.nn.relu(tf.add(tf.matmul(h_conv_val_flat, W_fc2), b_fc2))
        W_fc3 = weight_variable([128, 1])
        b_fc3 = bias_variable([1])
        self.weights.append(W_fc3)
        self.weights.append(b_fc3)
        h_fc3 = tf.nn.tanh(tf.add(tf.matmul(h_fc2, W_fc3), b_fc3))

        return h_fc1, h_fc3
