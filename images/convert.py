# Base code from here 
# https://stackoverflow.com/questions/51205502/convert-a-black-and-white-image-to-array-of-numbers/51207135

from PIL import Image
import sys
filename = sys.argv[1]

# img = Image.open('image.png').convert('L')  # convert image to 8-bit grayscale
img = Image.open(filename).convert('1')  # convert image to 1-bit black and white
WIDTH, HEIGHT = img.size

data = list(img.getdata()) # convert image data to a list of integers
# convert that to 2D list (list of lists of integers)
data = [data[offset:offset+WIDTH] for offset in range(0, WIDTH*HEIGHT, WIDTH)]

# At this point the image's pixels are all in memory and can be accessed
# individually using data[row][col].

f = open(filename+".h","w+")
# For example:
for row in data:
#    f.write(','.join('{:3}'.format(round(value/3,0)) for value in row))
    f.write(','.join('{:1}'.format(value!=0) for value in row))
    f.write(',\n')

f.close()
