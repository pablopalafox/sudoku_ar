import numpy as np
np.random.seed(1337)
from keras.models import Sequential, model_from_json
import json
import argparse

np.set_printoptions(threshold=np.inf)
dir_path = '.'
arch = open(dir_path + '/trained_net/char74k_architecture.json').read()
model = model_from_json(arch)
model.load_weights(dir_path + '/trained_net/char74k_weights.h5')
#model.compile(loss='categorical_crossentropy', optimizer='adadelta')
arch = json.loads(arch)

with open('output.nnet', 'w') as fout:
    fout.write('layers ' + str(len(model.layers)) + '\n')

    layers = []
    for ind, l in enumerate(arch["config"]):
        print (ind, l)
        fout.write('layer ' + str(ind) + ' ' + l['class_name'] + '\n')

        print (str(ind), l['class_name'])
        layers += [l['class_name']]
        if l['class_name'] == 'Convolution2D':
            #fout.write(str(l['config']['nb_filter']) + ' ' + str(l['config']['nb_col']) + ' ' + str(l['config']['nb_row']) + ' ')

            #if 'batch_input_shape' in l['config']:
            #    fout.write(str(l['config']['batch_input_shape'][1]) + ' ' + str(l['config']['batch_input_shape'][2]) + ' ' + str(l['config']['batch_input_shape'][3]))
            #fout.write('\n')

            W = model.layers[ind].get_weights()[0]
            print (W.shape)
            fout.write(str(W.shape[0]) + ' ' + str(W.shape[1]) + ' ' + str(W.shape[2]) + ' ' + str(W.shape[3]) + ' ' + l['config']['border_mode'] + '\n')

            for i in range(W.shape[0]):
                for j in range(W.shape[1]):
                    for k in range(W.shape[2]):
                        fout.write(str(W[i,j,k]) + '\n')
            fout.write(str(model.layers[ind].get_weights()[1]) + '\n')

        if l['class_name'] == 'Activation':
            fout.write(l['config']['activation'] + '\n')
        if l['class_name'] == 'MaxPooling2D':
            fout.write(str(l['config']['pool_size'][0]) + ' ' + str(l['config']['pool_size'][1]) + '\n')
        #if l['class_name'] == 'Flatten':
        #    print l['config']['name']
        if l['class_name'] == 'Dense':
            #fout.write(str(l['config']['output_dim']) + '\n')
            W = model.layers[ind].get_weights()[0]
            print (W.shape)
            fout.write(str(W.shape[0]) + ' ' + str(W.shape[1]) + '\n')


            for w in W:
                fout.write(str(w) + '\n')
            fout.write(str(model.layers[ind].get_weights()[1]) + '\n')
