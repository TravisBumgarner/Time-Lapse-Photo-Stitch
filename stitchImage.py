from PIL import Image, ImageDraw
import os
import time
def getImages():
    imageList = []
    for folderName, subfolders, filenames in os.walk(".\\imagesToStich"):
        for filename in filenames:
            if filename.endswith('.png') or filename.endswith('.jpg') or filename.endswith('.gif'):
                imageList.append(filename)
            else:
                print(filename + " is not a valid image file. Only use png, jpg, gif.")
    return imageList

def createTestImages():
    colors = ["Black", "Maroon", "FireBrick", "Crimson", "Gold"]
    i = 0
    for color in colors:
        im = Image.new("RGBA",(600,200),color)
        im.save(".\\imagesToStich\\" + str(i) + ".png")
        i += 1

def verifyImageSizes(images):
    #Takes in list of images an verifies they're all the same size
    finalImageSize = Image.open(".\\imagesToStich\\" + images[0]).size
    for image in images:
        if finalImageSize == Image.open(".\\imagesToStich\\" + image).size:
            continue
        else:
            print("Image " + image + " is not the same size as the other images.")
            return False
    print("All image files of the same dimensions.")
    return True

def genFileName(fileName):
    #Generates unique filenames
    fname, fextension = os.path.splitext(fileName)
    i = 0
    newFileName = fname + str(i) + fextension
    for folderName,subfolders, filenames in os.walk("."):
        while newFileName in filenames:
            i += 1
            newFileName = fname + str(i) + fextension
            if newFileName not in filenames:
                break
        return newFileName
                
def stitchImages(images):
    #images is an list of image file names
    #Grab finalImageSize from first image in list
    finalImageSize = Image.open(".\\imagesToStich\\" + images[0]).size
    xPosition = 0
    #Creates a pixel width based on the number of images and the width of each image
    xWidth = int(finalImageSize[0]/len(images))
    finalImage = Image.new('RGBA',finalImageSize,"White")
    print("Now Creating image with size: " + str(finalImageSize) + " with pixels/image of " + str(xWidth))
    for image in images:
        openImage = Image.open(".\\imagesToStich\\" + image)
        #Crop doesn't go, top, left, width, height.
        #It goes x1,y1,x2,y2
        cropImage = openImage.crop((xPosition,0,xPosition+xWidth,finalImageSize[1]))
        finalImage.paste(cropImage,(xPosition,0))
        xPosition += xWidth
        openImage.close()
    uniqueFileName = genFileName("finalImage.png")
    finalImage.save(uniqueFileName)
    return uniqueFileName

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

def main():
    try:
    #Gets images from \imagesToStich
        images = getImages()
        verifyImageSizes(images)
        
        startTime = time.time()
        #Stitches images and saves to finalImage.png
        savedImage = stitchImages(images)
        print("Image created successfully in " + "--- %s seconds ---" % (time.time() - startTime))

        startTime = time.time()
        removeWhiteSpace(savedImage)
        print("Whitespace removed successfully in " + "--- %s seconds ---" % (time.time() - startTime))
    except IndexError:
        print("Images folder is empty")
if __name__ == '__main__':
    main()
    input("Press enter to close.")
