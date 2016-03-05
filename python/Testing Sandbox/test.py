from PIL import Image, ImageDraw
'''
im = Image.open("finalImage.png")
im = im.convert("RGB")
imSize = im.size
imCrop = im.crop((imSize[0]-1,0,imSize[0],imSize[1]))
colors = {}
for color in imCrop.getdata():
    colors[color] = colors.get(color, 0) + 1
if colors.get((255,255,255),0) == imSize[1]:
    #If number of white pixels is equal to the height of the image, remove column.
'''

def removeWhiteSpace(finalImage):
    def checkColForWhiteSpace(imCrop):
        colors = {}
        for color in imCrop.getdata():
            colors[color] = colors.get(color, 0) + 1
        if colors.get((255,255,255),0) == imSize[1]:
            #If number of white pixels is equal to the height of the image, remove column.
            return True
        else:
            return False        
    im = Image.open(finalImage)
    im = im.convert("RGB")
    imSize = im.size
    #Crop right side
    currentCol = imSize[0]
    colsToRemoveRightSide = 0
    while True:
        #While all white columns exist
        imCrop = im.crop((currentCol-1,0,currentCol,imSize[1]))
        if checkColForWhiteSpace(imCrop) == True:
            currentCol -= 1
            colsToRemoveRightSide += 1
        else:
            break
    im.crop((0,0,imSize[0]-colsToRemoveRightSide,imSize[1])).save(finalImage)

        


removeWhiteSpace("finalImage.png")
