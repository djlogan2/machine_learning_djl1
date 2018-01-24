__author__ = 'david'

import numpy as np
from PIL import Image
import math


class WriteImages:
    @staticmethod
    def write(array, textarray, tofile, type='box'): # type='box' type='line' type='column'
        if len(array.shape) == 3: # count, x, y
            top = WriteImages.write_image(np.array(array), 'L', type)
        elif len(array.shape) == 4 and array.shape[3] == 3: # count, x, y, rgb
            top = WriteImages.write_image(np.array(array), 'RGB', type)
        elif len(array.shape) == 4 and array.shape[3] == 4: # count, x, y, rgba
            top = WriteImages.write_image(np.array(array), 'RGBA', type)
        else:
            raise Exception('Dont know how to write this shape')
        top.save(tofile, 'png')

    @staticmethod
    def redimension(array, type):
        if type == 'row':
            return array.shape[0], 1
        elif type == 'column':
            return 1, array.shape[0]
        width = int(math.ceil(math.sqrt(array.shape[0])))
        height = int(math.ceil(float(array.shape[0]) / width))
        return width, height

    @staticmethod
    def write_image(array, mode, type):
        x_count, y_count = WriteImages.redimension(array, type)
        image_width = x_count * array.shape[1]
        image_height = y_count * array.shape[2]
        img = Image.new(mode, (image_width, image_height), 'black')
        for h in range(y_count):
            for w in range(x_count):
                if (h * x_count) + w >= array.shape[0]:
                    break
                img.paste(Image.fromarray(array[h * x_count + w]), (array.shape[1] * w, array.shape[2] * h))
        return img
