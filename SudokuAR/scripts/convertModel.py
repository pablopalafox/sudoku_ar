import numpy as np
from keras.models import Sequential
from keras.layers import Dense
from keras.models import model_from_json
from params import *


# LOAD THE NETWORK
dir_path = "./"
with open(dir_path + '/trained_net/char74k_architecture.json', 'r') as f:
    model = model_from_json(f.read())

# Loading the weights
model.load_weights(dir_path + '/trained_net/char74k_weights.h5')

from kerasify import export_model
export_model(model, 'example.model')
