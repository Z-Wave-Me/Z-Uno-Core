from PIL import Image
import numpy as np 
import argparse

def print_im_array(lst, w, threshold = 5):
    text = ""
    for i in range(len(lst)):
        if i%w == 0:
            text += "\n"
        if lst[i] > threshold:
            text += "XX"
        else:
            text += "  "
        
    return text
def conv2Code(lst, w, h, threshold = 5, array_repr = True):
    num_raws = h / 8
    if array_repr:
        text = "{ \n%02d, %02d, "%(w,num_raws)
    else:
        text = "\"\\x%02x\\x%02x\""%(w,num_raws)
    size = 2
    for i in range(num_raws):
        text += "\n"
        if not array_repr:
            text += "\""    
        for j in range(w):
            cb = 0x00
            for k in range(8):
                mask =  1 << k
                index = (i*8 + k) * w + j
                if lst[index] > threshold:
                    cb |= mask
            if array_repr:
                text += "0x%02x"%(cb)
                if (i!=num_raws-1 or j!=w-1):
                    text += ", "
            else:
                text += "\\x%02x"%(cb)
            size += 1
            
        if not array_repr:
            text += "\""  
    if array_repr:
        text += "\n}"
    print "Total size:%d bytes"%(size)
    return text

def convImageFunc(args):
    img = Image.open(args.filename)
    if args.size:
        sizes = args.size.split(",")
        w = 0
        h = 0
        print "Native Size: %dx%d"%(img.size[0],img.size[1])
        if sizes[0]=='x':
            h = int(sizes[1])
            w = int(((h*1.0) / img.size[1])*img.size[0])

        elif sizes[1]=='x':
            w = int(sizes[0])
            h = int(((w*1.0) / img.size[0])*img.size[1])
        else:
            w = int(sizes[0])
            h = int(sizes[1])
        print "New Size: %dx%d"%(w,h)
        img = img.resize((w,h), Image.ANTIALIAS)
    else:
        w = img.size[0]
        h = img.size[1]
    img = img.convert('L')
    image_list = list(img.getdata())
    if args.draw:
        print print_im_array(image_list, w)
    arr_mode = False
    if args.bytes:
        arr_mode = True
    print conv2Code(image_list, w, h, array_repr = arr_mode)

if __name__ == "__main__":

    def Main():
        parser = argparse.ArgumentParser(description='ZWave>ME Image converter tool for OLED\n Welcome :)')
        subparsers = parser.add_subparsers()

        parserConvImg = subparsers.add_parser('conv', help="Convert image to 1bit OLED format")
        parserConvImg.add_argument('filename', help="Source image file")
        parserConvImg.add_argument('-s',"--size", help="Setting up output image size. Format size=\"w,h\"")
        parserConvImg.add_argument('-d',"--draw", nargs='?', type=bool, const=True, default=False, help="Draw result in ASCII")
        parserConvImg.add_argument('-b',"--bytes", nargs='?', type=bool, const=True, default=False, help="Draw result in ASCII")
        
        
        parserConvImg.set_defaults(func=convImageFunc)

        args = parser.parse_args()
        args.func(args)


    Main()
