import cv2
import os
import numpy as np
from keras.datasets import mnist
from params import *
dir_path = os.path.dirname(__file__)


# FUNCTIONS
# Returns the directory path, and the number it will process
def dir_and_digit():
    directories = range(0, 10)  # Names of the directories
    dir_and_digit = []
    for dir in directories:
        dir_and_digit.append((os.path.join(dir_path, "cnn_train_digits/" + str(dir) + "/"), dir))
    return dir_and_digit


# NEEDED FOR THE TRACK BAR
def nothing(x):
    pass


def generate_white_imgs(title_digit):
    white_image = np.full((128, 128), 255, np.uint8)
    for name in range(1016):
        cv2.imwrite(dir_path+'/cnn_train_digits/'+title_digit+'/'+str(name)+'.png', white_image)


def resize(image):
    return cv2.resize(image, (digit_w, digit_h))


def threshold(image):
    return cv2.threshold(image, THRESHOLD_VAL, 255, cv2.THRESH_BINARY_INV)
    #return cv2.bitwise_not(image, image)


# Randomise the list of tuples
def scramble_dataset(list):
    np.random.shuffle(list)
    return list


def prepare_image(image):
    image = resize(image)
    #ret, image = threshold(image)
    return image


# Iterates through the directories and returns pairs of x and y
def read_dirs():
    x = []
    y = []
    list = []
    for i in range(0, 10):
        for image in os.listdir(dir_and_digit()[i][0]):
            path = os.path.join(dir_path, "cnn_train_digits/" + str(i) + "/" + image)
            image = cv2.imread(path, 0)
            image = prepare_image(image)
            x.append(image)
            y.append(dir_and_digit()[i][1])
            list.append((image, dir_and_digit()[i][1]))
    return np.asanyarray(list)


def exclusions(exclude_label, x_train, y_train, x_test, y_test):

    for i, item in enumerate(y_train):
        if item == exclude_label:
            x_train[i] = np.zeros([img_cols, img_rows], dtype=np.uint8)
            x_train[i].fill(0)

    for i, item in enumerate(y_test):
        if item == exclude_label:
            x_test[i] = np.zeros([img_cols, img_rows], dtype=np.uint8)
            x_test[i].fill(0)

    return x_train, y_train, x_test, y_test


# Divides the dataset into Training and Test sets and returns (x_train, y_train), (x_test, y_test)
def split_dataset(list_in, combined=True, exclude_label=None, even_training=True):

    len_char74k = len(list_in)

    train_set_len = int(len_char74k*0.8571)

    train, test = list_in[:train_set_len, :], list_in[train_set_len:, :]

    len_train = len(train)
    len_test = len(test)

    print("Length of train set:", len_train)
    print("Length of test set:", len_test)
    print(len_char74k, "images of Char74k in total.")

    x_train = []
    y_train = []
    x_test = []
    y_test = []
    for x in train:
        x_train.append(x[X])
    for y in train:
        y_train.append(y[Y])
    for x in test:
        x_test.append(x[X])
    for y in test:
        y_test.append(y[Y])

    x_train = np.asanyarray(x_train)
    y_train = np.asanyarray(y_train)
    x_test = np.asanyarray(x_test)
    y_test = np.asanyarray(y_test)

    # This adds MNIST to the loaded dataset
    if combined is True:
        (x_train_mn, y_train_mn), (x_test_mn, y_test_mn) = mnist.load_data()

        if even_training is True:
            x_train_mn = x_train_mn[:len_train, :, :]
            x_test_mn = x_test_mn[:len_test, :, :]
            y_train_mn = y_train_mn[:len_train]
            y_test_mn = y_test_mn[:len_test]
            print("The MNIST dataset has been trimmed to", x_train_mn.shape[0] + x_test_mn.shape[0], "as well.")

        x_train = np.concatenate((x_train, x_train_mn))
        y_train = np.concatenate((y_train, y_train_mn))
        x_test = np.concatenate((x_test, x_test_mn))
        y_test = np.concatenate((y_test, y_test_mn))

    # If exclude was chosen, it calls a function to remove labels
    if exclude_label is not None:
        x_train, y_train, x_test, y_test = exclusions(exclude_label, x_train, y_train, x_test, y_test)

    return (x_train, y_train), (x_test, y_test)


# Visualise 10 items from the dataset about to train
def see_samples(x, y, nSamples):
    for i in range(nSamples):
        win_name = "y = " + str(y[i])
        cv2.namedWindow(win_name)
        cv2.moveWindow(win_name, 700, 400)
        enlarged_im = cv2.resize(x[i], (100, 100))
        cv2.imshow(win_name, enlarged_im)
        cv2.waitKey()
        cv2.destroyWindow(win_name)


# Returns list of tuples:
def load_our_dataset(dataset='combination', whiten=None, even_training=True):

    if dataset is 'mnist':
        if whiten is not None:
            (x_train, y_train), (x_test, y_test) = mnist.load_data()
            x_train, y_train, x_test, y_test = exclusions(whiten, x_train, y_train, x_test, y_test)
            return (x_train, y_train), (x_test, y_test)

        else:
            return mnist.load_data()

    else:
        # Read the Directories with the CHAR74k images stored
        sorted_tuples = read_dirs()

        # Scramble the order
        unsorted_tuples = scramble_dataset(sorted_tuples)

        if dataset is 'combination':
            return split_dataset(list_in=unsorted_tuples,
                                 combined=True,
                                 exclude_label=whiten,
                                 even_training=even_training)

        if dataset is 'char74k':
            return split_dataset(list_in=unsorted_tuples,
                                 combined=False,
                                 exclude_label=whiten,
                                 even_training=False)


def save_model(model, title_dataset, time, net_details=None):

    # Saving the weights
    model.save_weights('trained_net/'+title_dataset+'_weights_'+time+'.h5')

    # Saving the network architecture
    with open('trained_net/'+title_dataset+'_architecture_'+time+'.json', 'w') as f:
        f.write(model.to_json())

    if net_details is not None:
        # Saving the network architecture
        with open('trained_net/'+title_dataset+'_EXTRA_'+time+'.txt', 'w') as f:
            f.write("Hello.")


#def something():
#    img_array = image.img_to_array(img)
#    img_array_expanded_dims = np.expand_dims(img_array, axis=0)
#    return keras.applications.mobilened.preprocess_input(img_array_expanded_dims)