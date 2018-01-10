__author__ = 'david'

import numpy as np
import Image
import math


class WriteImages:
    @staticmethod
    def write(array, textarray, tofile, type='box'): # type='box' type='line' type='column'
        if len(array.shape) == 3: # count, x, y
            #imagearray = WriteImages.convert_one_to_rgba(array)
            #top = WriteImages.write_rgba(imagearray, type)
            top = WriteImages.write_bw(np.array(array), type)
        elif len(array.shape) == 4 and array.shape[3] == 3: # count, x, y, rgb
            top = WriteImages.write_rgb(np.array(array), type)
        elif len(array.shape) == 4 and array.shape[3] == 4: # count, x, y, rgba
            top = WriteImages.write_rgba(np.array(array), type)
        else:
            raise Exception('Dont know how to write this shape')
        top.show()
        print 'here'

    @staticmethod
    def convert_one_to_rgba(array):
        if np.amax(array) <= 1.0:
            array *= 255
        array = np.array(array, dtype=np.uint8)
        arrayshape = list(array.shape) + [1]
        newshape = list(array.shape) + [3]
        width = array.shape[1]
        height = array.shape[2]
        data = np.repeat([255,255,255], np.prod(array.shape)).reshape(newshape)
        data = np.concatenate([data, array.reshape(arrayshape)],axis=3)
        return np.array(data, dtype=np.uint8) # Image.frombytes('RGBA', (width, height), data)

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
    def write_bw(array, type):
        x_count, y_count = WriteImages.redimension(array, type)
        image_width = x_count * array.shape[1]
        image_height = y_count * array.shape[2]
        img = Image.new('L', (image_width, image_height), 'black')
        for h in range(y_count):
            for w in range(x_count):
                if (h * x_count) + w >= array.shape[0]:
                    break
                #Image.fromarray(array[h * x_count + w]).show()
                img.paste(Image.fromarray(array[h * x_count + w]), (array.shape[1] * w, array.shape[2] * h))
        return img

    @staticmethod
    def write_rgb(array, type):
        x_count, y_count = WriteImages.redimension(array, type)
        image_width = x_count * array.shape[1]
        image_height = y_count * array.shape[2]
        img = Image.new('RGB', (image_width, image_height), 'black')
        for h in range(y_count):
            for w in range(x_count):
                if (h * x_count) + w >= array.shape[0]:
                    break
                img.paste(Image.fromarray(array[h * x_count + w]), (array.shape[1] * w, array.shape[2] * h))
        return img

    @staticmethod
    def write_rgba(array, type):
        x_count, y_count = WriteImages.redimension(array, type)
        image_width = x_count * array.shape[1]
        image_height = y_count * array.shape[2]
        img = Image.new('RGBA', (image_width, image_height), 'black')
        for h in range(y_count):
            for w in range(x_count):
                if (h * x_count) + w >= array.shape[0]:
                    break
                Image.fromarray(array[h * x_count + w]).show()
                img.paste(Image.fromarray(array[h * x_count + w]), (array.shape[1] * w, array.shape[2] * h))
        return img
