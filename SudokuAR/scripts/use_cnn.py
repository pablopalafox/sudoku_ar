import cv2
from keras.models import model_from_json
from preparing_dataset import *
from keras import backend as k
import glob
from params import *
import os
#from keras.applications.imagenet_utils import decode_predictions
from time import time
from keras.applications import imagenet_utils

dir_path = os.path.dirname(__file__)
#dir_path = "./scripts"
#dir_path = "./"

import re
numbers = re.compile(r'(\d+)')
def numericalSort(value):
    parts = numbers.split(value)
    parts[1::2] = map(int, parts[1::2])
    return parts


def read_images(dir):
    test_images = [prepare_image(cv2.imread(file, GRAYSCALE)) for file in sorted(glob.glob(dir+'/*.png'), key=numericalSort)]

    if len(test_images) == 0:
        print("No image loaded. Sought directory", dir)
        return -1
    else:
        return np.asanyarray(test_images)

#def read_images(dir):
#    test_images = [prepare_image(cv2.imread(file, GRAYSCALE)) for file in glob.glob(dir+'/*.png')]
#    return np.asanyarray(test_images)


def predict(input_imgs):

    if k.image_data_format() == 'channels_first':
        x_test = input_imgs.reshape(input_imgs.shape[LEN], 1, digit_w, digit_h)
    else:
        x_test = input_imgs.reshape(input_imgs.shape[LEN], digit_w, digit_h, 1)

    x_test = x_test.astype('float32')
    x_test /= 255

    # LOAD THE NETWORK
    with open(dir_path + '/trained_net/char74k_architecture.json', 'r') as f:
        model = model_from_json(f.read())

    # Loading the weights
    model.load_weights(dir_path + '/trained_net/char74k_weights.h5')

    # RUN and KEEP PREDICTIONS
    predicted_classes = model.predict_classes(x_test)

    for image in x_test:
        image = np.expand_dims(image, axis=0)


    predictions = model.predict(x_test)
    #print("New prediction:", prediction)
    #results = imagenet_utils.decode_predictions(predictions)
    #print('Predicted:', results)

    return predicted_classes

def disp_predictions(input_imgs, predictions):
    # SHOW IMAGES AND PREDICTIONS
    for i, img in enumerate(predictions):
        win_name = 'Prediction: ' + str(img)
        cv2.namedWindow(win_name)
        cv2.moveWindow(win_name, 700, 400)
        enlarged_im = cv2.resize(input_imgs[i], (400, 400))
        cv2.imshow(win_name, enlarged_im)
        cv2.waitKey()
        cv2.destroyWindow(win_name)


#  Takes in imags list, returns predicted classes
def cpp_sudoku_call(imgs_list):
    th_imgs = [prepare_image(cv2.imread(im, GRAYSCALE)) for im in imgs_list]
    predicted_classes = predict(th_imgs)
    print(type(predicted_classes))
    return predicted_classes


def run_predictions(imgs_path=dir_path + '/gray_imgs/', displ=False, write_file=True):
    # PROCESS THE IMAGES
    imgs_array = read_images(imgs_path)
    predictions = predict(imgs_array)

    if displ:
        disp_predictions(imgs_array, predictions)

    if write_file:
        with open(dir_path + '/predictions/results.txt', 'w+') as f:
            #print(f)
            #np.savetxt()
            f.write(" ".join(map(str, predictions)))

start_time = time()
run_predictions()
print("--- %s seconds for the neural network to run ---" % (time() - start_time))


#predict(im_list)

#print(dir_path+"/extracted_numbers/only_nums")

#run_predictions(dir_path+"/extracted_numbers/only_nums")
#cpp_sudoku_call()

""" 
EXTRA CODE
#predictions = model.predict(x_test, verbose=True)



"""
