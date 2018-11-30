from keras.models import load_model
classifier = load_model("saved_model.h5")
import numpy as np
from keras.preprocessing import image
def image_tester(img):
    test_image = image.load_img(img, target_size = (64, 64))
    test_image = image.img_to_array(test_image)
    test_image = np.expand_dims(test_image, axis = 0)
    result = classifier.predict(test_image)
    #training_set.class_indices
    if result[0][0] == 1:
        prediction = 'dog'
        print("DOG")
    else:
        prediction = 'cat'
        print("CAT")

image_tester("image/cunchat.jpeg")
image_tester("image/test.jpg")
image_tester("image/th.jpeg")
